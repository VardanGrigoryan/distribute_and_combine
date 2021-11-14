#ifndef __TASK_QUEUE__
#define __TASK_QUEUE__

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace libs {
	namespace safe_datastructure {

/**
 * \brief Implements a thread safe queue which is required to hold pending tasks.
 * \tparam T the type of data stored in the map as a key
 * \tparam U the type of data stored in the map as a value
 */
template <typename T, typename U>
class task_queue
{
public:
  /**
   * The default constructor
   */
  task_queue() {}
  /**
   * The destructor
   */
  ~task_queue() {}
  /**
   * Thread-safely pushes the task into the queue
   * \param t is a tuple which holds the word/actua value
   * \tparam T the value type
   * \tparam U the global position type
   * \tparam value type's length
   * @return `void`
   */
  void push(std::tuple<T, U, U>&& t)
  {
    std::unique_ptr<std::tuple<T, U, U>> value(std::make_unique<std::tuple<T, U, U>>(t));
    std::lock_guard<std::mutex> lck(m_mtx);
    m_queue.push(std::move(value));
    m_cnd.notify_one();
  }
  /**
   * Thread-safely pops the task from the queue
   * \param t is a tuple which holds the word/actua value
   * \tparam T the value type
   * \tparam U the global position type
   * \tparam value type's length
   * @return `std::unique_ptr<std::tuple<T, U, U>>`
   */
  std::unique_ptr<std::tuple<T, U, U>> pop()
  {
    std::unique_lock<std::mutex> lck(m_mtx);
    if(m_queue.empty()) {
	    return nullptr;
    }
    m_cnd.wait(lck, [&]{ return !m_queue.empty();});
    std::unique_ptr<std::tuple<T, U, U>> val(std::move(m_queue.front()));
    m_queue.pop();
    return val;
  }
  /**
   * Checks whether the queue is empty
   * @returns `bool`
   */
  bool empty() const
  {
	  std::lock_guard<std::mutex> lck(m_mtx);
	  return m_queue.empty();
  }
  /**
   * Gets the queue size
   * @returns `size_t`
   */
  size_t size() const
  {
	  std::lock_guard<std::mutex> lck(m_mtx);
	  return m_queue.size();
  }
private:
  task_queue(const task_queue<T, U>&)=delete;
  task_queue& operator=(const task_queue<T, U>&)=delete;
  task_queue(const task_queue<T, U>&&)=delete;
  task_queue& operator=(const task_queue<T, U>&&)=delete;
private:
  std::condition_variable m_cnd;
  mutable std::mutex m_mtx;
  std::queue<std::unique_ptr<std::tuple<T, U, U>>> m_queue;
};
}
}
#endif // __TASK_QUEUE__

