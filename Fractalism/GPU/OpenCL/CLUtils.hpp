#ifndef _FRACTALISM_CL_UTILS_HPP_
#define _FRACTALISM_CL_UTILS_HPP_

#include <thread>
#include <type_traits>
#include <vector>

#include <Fractalism/Exceptions.hpp>
#include <Fractalism/GPU/OpenCL/CLCommon.hpp>

namespace fractalism::gpu::opencl::clutils {

/**
 * @brief Returns a string representation of an OpenCL error code.
 * @param err The OpenCL error code.
 * @return A string describing the error.
 */
const char* getCLErrorString(cl_int err);

/**
 * @brief Gets the current OpenCL context.
 * @return A reference to the current OpenCL context.
 */
const cl::Context& getClContext();

/**
 * @brief Gets the current OpenCL command queue.
 * @return A reference to the current OpenCL command queue.
 */
const cl::CommandQueue& getQueue();

/**
 * @brief Gets the maximum memory allocation size for the OpenCL context.
 * @return The maximum memory allocation size in bytes.
 */
cl_ulong getMaxMemAllocSize();

template <class Callable, typename Type, typename... Args>
concept SVMCallback = std::is_invocable_v<Callable, Type*, std::size_t, Args...>;

/**
 * @brief Modifies a Shared Virtual Memory (SVM) buffer using a callback
 * function.
 * @tparam Type The type of the elements in the SVM buffer.
 * @tparam Callable The type of the callback function.
 * @tparam Args The types of the additional arguments to the callback function.
 * @param ptr Pointer to the SVM buffer.
 * @param count Number of elements in the SVM buffer.
 * @param callback The callback function to modify the SVM buffer.
 * @param waitEvents A vector of events to wait for before mapping the SVM
 * buffer.
 * @param doneEvent The event to signal when the modification is done.
 * @param args Additional arguments to pass to the callback function.
 */
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
} // namespace fractalism::gpu::opencl::clutils

#endif
