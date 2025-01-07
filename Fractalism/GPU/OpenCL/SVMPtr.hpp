#ifndef _FRACTALISM_SVM_PTR_HPP_
#define _FRACTALISM_SVM_PTR_HPP_

#include <format>
#include <vector>
#include <type_traits>
#include <functional>

#include <Fractalism/Utils.hpp>
#include <Fractalism/Exceptions.hpp>
#include <Fractalism/GPU/OpenCL/CLUtils.hpp>
#include <Fractalism/GPU/OpenCL/CLCommon.hpp>

namespace fractalism::gpu::opencl {
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
      * @param itemCount Number of elements in the SVM.
      * @param flags Memory flags for allocation.
      * @param alignment Memory alignment.
      */
    SVMPointer(
        size_t itemCount,
        cl_svm_mem_flags flags = CL_MEM_READ_WRITE,
        cl_uint alignment = 0) :
          itemCount(itemCount),
          flags(flags),
          alignment(alignment),
          ptr(itemCount ? reinterpret_cast<T*>(clSVMAlloc(clutils::getClContext()(), flags, itemCount * sizeof(T), alignment)) : nullptr) {
      if (itemCount && !ptr) {
        throw CLSVMAllocationError("Could not allocate SVM buffer");
      }
    }

    SVMPointer(const SVMPointer<T>& other) :
          itemCount(other.itemCount),
          flags(other.flags),
          alignment(other.alignment),
          ptr(std::exchange(other.ptr, nullptr)) {}

    SVMPointer(SVMPointer<T>&& other) noexcept :
          itemCount(std::move(other.itemCount)),
          flags(std::move(other.flags)),
          alignment(std::move(other.alignment)),
          ptr(std::exchange(other.ptr, nullptr)) {}

    /**
      * @brief Destructor that releases the SVM memory.
      */
    ~SVMPointer() {
      free();
    }

    void free() {
      if (ptr) {
        clSVMFree(clutils::getClContext()(), ptr);
        itemCount = 0;
        ptr = nullptr;
      }
    }

    /**
      * @brief Assignment operator to transfer ownership of the SVM pointer.
      * @param other SVMPointer to assign from.
      * @return Reference to this SVMPointer.
      */
    SVMPointer<T>& operator=(SVMPointer<T>& other) noexcept {
      free();
      itemCount = other.itemCount;
      flags = other.flags;
      alignment = other.alignment;
      ptr = std::exchange(other.ptr, nullptr);
      return *this;
    }

    SVMPointer<T>& operator=(SVMPointer<T>&& other) noexcept {
      free();
      itemCount = std::move(other.itemCount);
      flags = std::move(other.flags);
      alignment = std::move(other.alignment);
      ptr = std::exchange(other.ptr, nullptr);
      return *this;
    }

    template<typename... Args, clutils::SVMCallback<T, Args...> Callable>
    inline void modify(Callable&& callback, const std::vector<cl::Event>& waitEvents, cl::Event& doneEvent, Args&&... args) {
      clutils::modifySVM(ptr, itemCount, callback, waitEvents, doneEvent, args...);
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
    cl_svm_mem_flags flags;    ///< Memory flags used for the SVM allocation.
    cl_uint alignment;         ///< Memory alignment of the SVM allocation.
    mutable T* ptr;            ///< Raw SVM pointer.
  };

  /**
    * @class SVMPtrArray
    * @brief Manages Shared Virtual Memory (SVM) pointers for OpenCL device communication.
    * @tparam T Data type for SVM.
    */
  template <typename T>
  class SVMPointerArray {
  public:
    /**
      * @brief Constructs an SVM pointer array.
      * @param itemCount Number of items in the array.
      * @param flags Memory flags for SVM.
      * @param alignment Alignment requirement.
      */
    SVMPointerArray(
      size_t itemCount,
      cl_svm_mem_flags flags = CL_MEM_READ_WRITE,
      cl_uint alignment = 0) :
      itemCount(itemCount),
      ptr(itemCount ? ((itemCount * sizeof(T)) / clutils::getMaxMemAllocSize()) + 1 : 0, flags, alignment) {
      if (itemCount) {
        size_t maxBufferItemCount = clutils::getMaxMemAllocSize() / sizeof(T);
        std::vector<SVMPointer<T>> subBuffers;
        subBuffers.reserve(ptr.itemCount);
        forEachUnmappedPointer([](
          T* pointer,
          size_t size,
          size_t index,
          cl_svm_mem_flags flags,
          cl_uint alignment,
          std::vector<SVMPointer<T>>& subBuffers) {
            try {
              subBuffers.emplace_back(size, flags, alignment);
            }
            catch (const CLSVMAllocationError& e) {
              throw CLSVMAllocationError(std::format("Could not allocate SVM buffer #{}", index));
            }
          }, flags, alignment, subBuffers);

        // Don't need to keep the individual wrapper objects long-term.
        // They're just used for RAII to make this exception-safe.
        cl::Event doneEvent;
        cl::UserEvent waitEvent(clutils::getClContext());
        const std::vector<cl::Event> waitEvents{};
        ptr.modify([](T** data, size_t length, std::vector<SVMPointer<T>>& subBuffers) {
          for (size_t i = 0; i < length; i++) {
            data[i] = subBuffers[i].claim();
          }
        }, waitEvents, doneEvent, subBuffers);
        waitEvent.setStatus(CL_COMPLETE);
        doneEvent.wait();
      }
    }

    SVMPointerArray(
      cl::NDRange& range,
      cl_svm_mem_flags flags = CL_MEM_READ_WRITE,
      cl_uint alignment = 0) :
      SVMPointerArray(range[0] * range[1] * range[2], flags, alignment) {
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
          clSVMFree(clutils::getClContext()(), ptr.ptr[i]);
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

    template<typename... Args, clutils::SVMCallback<T, size_t, Args...> Callable>
    inline void modify(Callable&& callback, const std::vector<cl::Event>& waitEvents, cl::Event& doneEvent, Args&&... args) {
      forEachUnmappedPointer([](
            T* pointer,
            size_t itemCount,
            size_t index,
            Callable& callback,
            const std::vector<cl::Event>& waitEvents,
            cl::Event& doneEvent,
            Args&... args) {
          clutils::modifySVM(pointer, itemCount, callback, waitEvents, doneEvent, index, args...);
        }, callback, waitEvents, doneEvent, args...);
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

    template<typename... Args, clutils::SVMCallback<T, size_t, Args...> Callable>
    auto forEachUnmappedPointer(Callable&& callback, Args&&... args) {
      using Return = std::invoke_result_t<Callable, T*, size_t, size_t, Args...>;
      constexpr bool isVoidReturn = std::is_void_v<Return>;
      size_t maxBufferItemCount = clutils::getMaxMemAllocSize() / sizeof(T);
      std::vector<std::conditional_t<isVoidReturn, char, Return>> results;
      if constexpr (!isVoidReturn) {
        results = std::vector<Return>();
        results.reserve(ptr.itemCount);
      }
      for (size_t i = 0; i < ptr.itemCount - 1; i++) {
        if constexpr (isVoidReturn) {
          std::invoke(std::forward<Callable>(callback),
            ptr.ptr[i],
            maxBufferItemCount,
            i,
            std::forward<Args>(args)...);
        } else {
          results.push_back(std::invoke(std::forward<Callable>(callback),
            ptr.ptr[i],
            maxBufferItemCount,
            i,
            std::forward<Args>(args)...));
        }
      }

      if constexpr (isVoidReturn) {
        std::invoke(std::forward<Callable>(callback),
          ptr.ptr[ptr.itemCount - 1],
          itemCount % maxBufferItemCount,
          ptr.itemCount - 1,
          std::forward<Args>(args)...);
      } else {
        results.push_back(std::invoke(std::forward<Callable>(callback),
          ptr.ptr[ptr.itemCount - 1],
          itemCount % maxBufferItemCount,
          ptr.itemCount - 1,
          std::forward<Args>(args)...));
        return results;
      }
    }
    size_t itemCount;
    SVMPointer<T*> ptr;
  };

  template<typename T>
  class BackBufferedSvmArrayPtr {
  public:
    /**
      * @brief Constructs a back-buffered SVM pointer array.
      * @param itemCount Number of items in the array.
      * @param flags Memory flags for SVM.
      * @param alignment Alignment requirement.
      */
    BackBufferedSvmArrayPtr(
        size_t itemCount,
        cl_svm_mem_flags flags = CL_MEM_READ_WRITE,
        cl_uint alignment = 0) :
            ptr(itemCount, flags, alignment),
            buffers(),
            activeBufferIndex(0) {}

    BackBufferedSvmArrayPtr(
      cl::NDRange& range,
      cl_svm_mem_flags flags = CL_MEM_READ_WRITE,
      cl_uint alignment = 0) :
        BackBufferedSvmArrayPtr(range[0] * range[1] * range[2], flags, alignment) {}

    BackBufferedSvmArrayPtr() : BackBufferedSvmArrayPtr(0, 0) {}

    void resize(size_t itemCount) {
      ptr.free();
      ptr = SVMPointerArray<T>(itemCount, ptr.ptr.flags, ptr.ptr.alignment);
      for (std::vector<std::vector<T>>& buffer : buffers) {
        buffer.resize(ptr.itemCount);
        ptr.forEachUnmappedPointer([](
            T* pointer,
            size_t size,
            size_t index,
            std::vector<std::vector<T>>& buffer) {
          buffer[index].resize(size);
        }, buffer);
      }
    }

    void resize(cl::NDRange& range) {
      resize(range[0] * range[1] * range[2]);
    }

    void addBuffer() {
      buffers.push_back(ptr.forEachUnmappedPointer([](
          T* pointer,
          size_t size,
          size_t index) {
        std::vector<T> bufferElement;
        bufferElement.resize(size);
        return bufferElement;
      }));
    }

    void removeBuffer() {
      buffers.resize(buffers.size() - 1);
    }

    void useBuffer(size_t bufferIndex, const std::vector<cl::Event>& waitEvents, cl::Event& doneEvent) {
      if (activeBufferIndex != bufferIndex) {
        std::vector<std::vector<T>>& targetBuffer = buffers[bufferIndex];
        // Get the currently used data out of the SVM, and put the target data into it.
        ptr.modify([](T* data, size_t itemCount, size_t index, std::vector<std::vector<T>>& targetBuffer) {
          utils::memswap(data, targetBuffer[index].data(), itemCount * sizeof(T));
        }, waitEvents, doneEvent, targetBuffer);
        // Put the data we got out of the SVM back into the last used buffer.
        targetBuffer.swap(buffers[activeBufferIndex]);
        activeBufferIndex = bufferIndex;
      } else {
        cl::UserEvent noOp(clutils::getClContext());
        noOp.setStatus(CL_COMPLETE);
        doneEvent = noOp;
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
#endif // !_SVM_PTR_HPP_
