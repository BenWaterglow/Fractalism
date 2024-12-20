#ifndef _FRACTALISM_CL_UTILS_HPP_
#define _FRACTALISM_CL_UTILS_HPP_

#include <type_traits>

#include <Fractalism/Exceptions.hpp>
#include <Fractalism/GPU/OpenCL/CLIncludeHelper.hpp>

namespace fractalism {
  namespace gpu {
    namespace opencl {
      namespace clutils {
        const char* getCLErrorString(cl_int err);

        template <class Callable, typename Type, typename... Args>
        concept SVMCallback = std::is_invocable_v<Callable, Type*, std::size_t, Args...>;

        template <class Callable, typename Type, typename... Args>
        using SVMCallbackResult = std::invoke_result_t<Callable, Type*, std::size_t, Args...>;

        template<typename Type, class Callable, typename... Args>
          requires SVMCallback<Callable, Type, Args...>
        inline SVMCallbackResult<Callable, Type, Args...> modifySVM(
          cl::CommandQueue queue,
          Type* ptr,
          size_t count,
          Callable&& callback,
          Args&&... args) {
          using Return = SVMCallbackResult<Callable, Type, Args...>;
          try {
            queue.enqueueMapSVM(ptr, CL_TRUE, CL_MAP_READ | CL_MAP_WRITE, count * sizeof(Type));
          } catch (const cl::Error& e) {
            throw CLError("Could not map OpenCL Shared Virtual Memory buffer", e);
          }

          try {
            // Handle void return differently.
            if constexpr (std::is_void_v<Return>) {
              std::invoke(std::forward<Callable>(callback), ptr, count, std::forward<Args>(args)...);
              queue.enqueueUnmapSVM(ptr);
            }
            else {
              Return result = std::invoke(std::forward<Callable>(callback), ptr, count, std::forward<Args>(args)...);
              queue.enqueueUnmapSVM(ptr);
              return result;
            }
          } catch (const cl::Error& e) {
            throw CLError("Could not unmap OpenCL Shared Virtual Memory buffer", e);
          }
        }
      }
    }
  }
}

#endif