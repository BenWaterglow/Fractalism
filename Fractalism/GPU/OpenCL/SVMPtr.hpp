#ifndef _FRACTALISM_SVM_PTR_HPP_
#define _FRACTALISM_SVM_PTR_HPP_

#include <format>
#include <vector>
#include <type_traits>
#include <functional>

#include <Fractalism/UI/WxIncludeHelper.hpp>
#include "wx/log.h"
#include <Fractalism/Utils.hpp>
#include <Fractalism/Exceptions.hpp>
#include <Fractalism/GPU/GPUContext.hpp>
#include <Fractalism/GPU/OpenCL/CLUtils.hpp>
#include <Fractalism/GPU/OpenCL/CLIncludeHelper.hpp>

#include <thread>
#include <source_location>

namespace fractalism {
  namespace gpu {
    namespace opencl {
      /**
       * @class SVMPointer
       * @brief Manages Shared Virtual Memory (SVM) pointers for OpenCL operations.
       * @tparam T Data type for the SVM.
       */
      template <typename T>
      class SVMPointer {
      public:

        /**
         * @brief Constructs an SVM pointer with the specified parameters.
         * @param ctx OpenCL context.
         * @param flags Memory flags for allocation.
         * @param itemCount Number of elements in the SVM.
         * @param alignment Memory alignment.
         */
        SVMPointer(GPUContext& ctx,
          size_t itemCount,
          cl_svm_mem_flags flags = CL_MEM_READ_WRITE,
          cl_uint alignment = 0) :
          ctx(ctx),
          itemCount(itemCount),
          ptr(reinterpret_cast<T*>(clSVMAlloc(ctx.clCtx(), flags, itemCount * sizeof(T), alignment))) {
          if (!ptr) {
            throw CLSVMAllocationError("Could not allocate SVM buffer");
          }
        }

        SVMPointer(const SVMPointer<T>& other) : ptr(std::exchange(other.ptr, nullptr)), itemCount(other.itemCount), ctx(other.ctx) {}

        SVMPointer(SVMPointer<T>&& other) noexcept : ptr(std::exchange(other.ptr, nullptr)), itemCount(std::move(other.itemCount)), ctx(other.ctx) {}

        /**
         * @brief Destructor that releases the SVM memory.
         */
        ~SVMPointer() {
          free();
        }

        void free() {
          clSVMFree(ctx.clCtx(), ptr);
          itemCount = 0;
          ptr = nullptr;
        }

        /**
         * @brief Assignment operator to transfer ownership of the SVM pointer.
         * @param rhs Right-hand SVMPointer to assign from.
         * @return Reference to this SVMPointer.
         */
        SVMPointer<T>& operator=(SVMPointer<T>& rhs) noexcept {
          free();
          ptr = std::exchange(rhs.ptr, nullptr);
          itemCount = rhs.itemCount;
          ctx = rhs.ctx;
          return *this;
        }

        SVMPointer<T>& operator=(SVMPointer<T>&& other) noexcept {
          free();
          ptr = std::exchange(other.ptr, nullptr);
          itemCount = std::move(other.itemCount);
          ctx = std::move(other.ctx);
          return *this;
        }

        template<class Callable, typename... Args>
          requires clutils::SVMCallback<Callable, T, Args...>
        inline void modify(Callable&& callback, Args&&... args) {
          return clutils::modifySVM(ctx.queue, ptr, itemCount, callback, args...);
        }

        /**
         * @brief Claims ownership of the SVM pointer and nullifies the original.
         * @return Pointer to the claimed SVM memory.
         */
        inline T* claim() const {
          return std::exchange(ptr, nullptr);
        }

        inline size_t size() const {
          return itemCount * sizeof(T);
        }

        /**
         * @brief Sets the SVM pointer as a kernel argument.
         * @param kernel The kernel to set the argument for.
         * @param index The argument index.
         */
        void asKernelArg(cl::Kernel& kernel, cl_uint index) const {
          try {
            kernel.setArg(index, ptr);
          }
          catch (const cl::Error& e) {
            throw CLKernelArgError("Shared Virtual Memory pointer", kernel, index, e);
          }
        }

        size_t itemCount;          ///< Number of items in the SVM.
        GPUContext& ctx;           ///< GPU context associated with this SVM pointer.
        mutable T* ptr;            ///< Raw SVM pointer.
      };

      /**
         * @class CLSolver::SVMPtrArray
         * @brief Manages Shared Virtual Memory (SVM) pointers for OpenCL device communication.
         * @tparam T Data type for SVM.
         */
      template <typename T>
      class SVMPointerArray {
      public:
        /**
         * @brief Constructs an SVM pointer array.
         * @param ctx GPU context.
         * @param itemCount Number of items in the array.
         * @param flags Memory flags for SVM.
         * @param alignment Alignment requirement.
         */
        SVMPointerArray(
          GPUContext& ctx,
          size_t itemCount,
          cl_svm_mem_flags flags = CL_MEM_READ_WRITE,
          cl_uint alignment = 0) :
          itemCount(itemCount),
          ptr(ctx, flags, ((itemCount * sizeof(T)) / ctx.maxMemAllocSize) + 1, alignment) {

          size_t maxBufferItemCount = ctx.maxMemAllocSize / sizeof(T);
          std::vector<SVMPointer<T>> subBuffers;
          subBuffers.reserve(ptr.itemCount);
          forEachUnmappedPointer([](
            T* pointer,
            size_t size,
            GPUContext& ctx,
            size_t index,
            cl_svm_mem_flags flags,
            cl_uint alignment,
            std::vector<SVMPointer<T>>& subBuffers) {
              try {
                subBuffers.emplace_back(ctx, size, flags, alignment);
              }
              catch (const CLSVMAllocationError& e) {
                throw CLSVMAllocationError(std::format("Could not allocate SVM buffer #{}", index));
              }
            }, flags, alignment, subBuffers);

          // Don't need to keep the individual wrapper objects long-term.
          // They're just used for RAII to make this exception-safe.
          ptr.modify([](T** data, size_t length, std::vector<SVMPointer<T>>& subBuffers) {
            for (size_t i = 0; i < length; i++) {
              data[i] = subBuffers[i].claim();
            }
            }, subBuffers);
        }

        SVMPointerArray(
          GPUContext& ctx,
          cl::NDRange& range,
          cl_svm_mem_flags flags = CL_MEM_READ_WRITE,
          cl_uint alignment = 0) :
          SVMPointerArray(ctx, range[0] * range[1] * range[2], flags, alignment) {
        }

        SVMPointerArray(const SVMPointerArray<T>& other) : ptr(other.ptr) {}

        SVMPointerArray(SVMPointerArray<T>&& other) noexcept : ptr(std::move(other.ptr)) {}

        /**
         * @brief Destructor that frees the SVM pointers.
         */
        ~SVMPointerArray() {
          free();
        }

        void free() {
          if (ptr.ptr) {
            for (size_t i = 0; i < ptr.itemCount; i++) {
              // Manually free, since we explicity claimed ownership of the pointers
              clSVMFree(ptr.ctx.clCtx(), ptr.ptr[i]);
            }
          }
          itemCount = 0;
          ptr.free();
        }

        SVMPointerArray<T>& operator=(SVMPointerArray<T>& other) noexcept {
          free();
          ptr = other.ptr;
          itemCount = other.itemCount;
          return *this;
        }

        SVMPointerArray<T>& operator=(SVMPointerArray<T>&& other) noexcept {
          free();
          ptr = std::move(other.ptr);
          itemCount = std::move(other.itemCount);
          return *this;
        }

        template<class Callable, typename... Args>
          requires clutils::SVMCallback<Callable, T, size_t, Args...>
        inline auto modify(Callable&& callback, Args&&... args) {
          return forEachUnmappedPointer([](
            T* pointer,
            size_t size,
            GPUContext& ctx,
            size_t index,
            Callable& callback,
            Args&... args) {
              return clutils::modifySVM(ctx.queue, pointer, size, callback, index, args...);
            }, callback, args...);
        }

        /**
         * @brief Sets the SVM pointer as a kernel argument.
         * @param kernel The kernel to set the argument for.
         * @param index The argument index.
         */
        void asKernelArg(cl::Kernel& kernel, cl_uint index) const {
          ptr.asKernelArg(kernel, index);
          cl_int err = clSetKernelExecInfo(
            kernel(),
            CL_KERNEL_EXEC_INFO_SVM_PTRS,
            ptr.size(),
            ptr.ptr);
          if (err != CL_SUCCESS) {
            throw CLKernelArgError("Shared Virtual Memory exec info", kernel, index, cl::Error(err, "clSetKernelExecInfo"));
          }
        }

        template<class Callable, typename... Args>
          requires clutils::SVMCallback<Callable, T, GPUContext&, std::size_t, Args...>
        auto forEachUnmappedPointer(Callable&& callback, Args&&... args) {
          using Return = clutils::SVMCallbackResult<Callable, T, GPUContext&, std::size_t, Args...>;
          constexpr bool isVoidReturn = std::is_void_v<Return>;
          size_t maxBufferItemCount = ptr.ctx.maxMemAllocSize / sizeof(T);
          std::vector<std::conditional_t<isVoidReturn, int, Return>> results;
          if constexpr (!isVoidReturn) {
            results = std::vector<Return>();
            results.reserve(ptr.itemCount);
          }
          for (size_t i = 0; i < ptr.itemCount - 1; i++) {
            if constexpr (isVoidReturn) {
              std::invoke(callback,
                ptr.ptr[i],
                maxBufferItemCount,
                ptr.ctx,
                i,
                args...);
            }
            else {
              results.push_back(std::invoke(callback,
                ptr.ptr[i],
                maxBufferItemCount,
                ptr.ctx,
                i,
                args...));
            }
          }

          if constexpr (isVoidReturn) {
            std::invoke(callback,
              ptr.ptr[ptr.itemCount - 1],
              itemCount % maxBufferItemCount,
              ptr.ctx,
              ptr.itemCount - 1,
              std::forward<Args>(args)...);
          }
          else {
            results.push_back(std::invoke(callback,
              ptr.ptr[ptr.itemCount - 1],
              itemCount % maxBufferItemCount,
              ptr.ctx,
              ptr.itemCount - 1,
              args...));
            return results;
          }
        }
      private:
        size_t itemCount;
        SVMPointer<T*> ptr;
      };

      template<typename T, size_t bufferCount = 2>
      class BackBufferedSvmArrayPtr {
      public:
        /**
         * @brief Constructs a back-buffered SVM pointer array.
         * @param ctx GPU context.
         * @param itemCount Number of items in the array.
         * @param flags Memory flags for SVM.
         * @param alignment Alignment requirement.
         */
        BackBufferedSvmArrayPtr(
          GPUContext& ctx,
          size_t itemCount,
          cl_svm_mem_flags flags = CL_MEM_READ_WRITE,
          cl_uint alignment = 0) :
          ptr(ctx, itemCount, flags, alignment),
          buffers(),
          activeBufferIndex(0) {
          buffers.reserve(bufferCount);
          for (size_t i = 0; i < bufferCount; i++) {
            buffers.push_back(ptr.forEachUnmappedPointer([](
              T* pointer,
              size_t size,
              GPUContext& ctx,
              size_t index) {
                std::vector<T> bufferElement = std::vector<T>();
                bufferElement.reserve(size);
                return bufferElement;
            }));
          }
        }

        BackBufferedSvmArrayPtr(
          GPUContext& ctx,
          cl::NDRange& range,
          cl_svm_mem_flags flags = CL_MEM_READ_WRITE,
          cl_uint alignment = 0) :
          BackBufferedSvmArrayPtr(ctx, range[0] * range[1] * range[2], flags, alignment) {
        }

        void useBuffer(size_t bufferIndex) {
          size_t targetBufferIndex = bufferIndex % bufferCount;
          if (activeBufferIndex != targetBufferIndex) {
            std::vector<std::vector<T>>& targetBuffer = buffers[targetBufferIndex];
            // Get the currently used data out of the SVM, and put the target data into it.
            ptr.modify([](T* data, size_t length, size_t index, std::vector<std::vector<T>>& targetBuffer) {
              utils::memswap(data, targetBuffer[index].data(), length * sizeof(T));
            }, targetBuffer);
            // Put the data we got out of the SVM back into the last used buffer.
            targetBuffer.swap(buffers[activeBufferIndex]);
            activeBufferIndex = targetBufferIndex;
          }
        }

        void free() {
          ptr.free();
          buffers.clear();
          activeBufferIndex = 0;
        }

        /**
         * @brief Sets the SVM pointer as a kernel argument.
         * @param kernel The kernel to set the argument for.
         * @param index The argument index.
         */
        void asKernelArg(cl::Kernel& kernel, cl_uint index) const {
          ptr.asKernelArg(kernel, index);
        }
      private:
        SVMPointerArray<T> ptr;
        std::vector<std::vector<std::vector<T>>> buffers;
        size_t activeBufferIndex;
      };
    }
  }
}
#endif // !_SVM_PTR_HPP_
