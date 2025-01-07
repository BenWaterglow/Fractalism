#ifndef _FRACTALISM_CL_UTILS_HPP_
#define _FRACTALISM_CL_UTILS_HPP_

#include <type_traits>
#include <vector>
#include <thread>

#include <Fractalism/Exceptions.hpp>
#include <Fractalism/GPU/OpenCL/CLCommon.hpp>

namespace fractalism::gpu::opencl::clutils {
  const char* getCLErrorString(cl_int err);
  
  const cl::Context& getClContext();
  const cl::CommandQueue& getQueue();
  cl_ulong getMaxMemAllocSize();

  template <class Callable, typename Type, typename... Args>
  concept SVMCallback = std::is_invocable_v<Callable, Type*, std::size_t, Args...>;

  template<typename Type, class Callable, typename... Args>
    requires SVMCallback<Callable, Type, Args...>
  inline void modifySVM(
    Type* ptr,
    size_t count,
    Callable&& callback,
    const std::vector<cl::Event>& waitEvents,
    cl::Event& doneEvent,
    Args&&... args) {
    const cl::CommandQueue& queue = getQueue();
    cl::Event mappedEvent;
    try {
      queue.enqueueMapSVM(ptr, CL_FALSE, CL_MAP_READ | CL_MAP_WRITE, count * sizeof(Type), &waitEvents, &mappedEvent);
    } catch (const cl::Error& e) {
      throw CLError("Could not map OpenCL Shared Virtual Memory buffer", e);
    }

    cl::UserEvent modifiedEvent(getClContext());
    /*std::thread([&callback, ptr, count, &args...](
        cl::Event mappedEvent,
        cl::UserEvent modifiedEvent) {*/
      mappedEvent.wait();
      std::invoke(std::forward<Callable>(callback), ptr, count, std::forward<Args>(args)...);
      modifiedEvent.setStatus(CL_COMPLETE);
    /*}, mappedEvent, modifiedEvent).detach();*/

    const std::vector<cl::Event> modifiedEvents{};
    try {
      queue.enqueueUnmapSVM(ptr, &modifiedEvents, &doneEvent);
    } catch (const cl::Error& e) {
      throw CLError("Could not unmap OpenCL Shared Virtual Memory buffer", e);
    }
  }
}

#endif