#ifndef __ANALYZE_STATISTICS__
#define __ANALYZE_STATISTICS__

#include <boost/algorithm/string.hpp>
#include <memory>
#include <mutex>
#include <regex>
#include <thread>
#include <unordered_map>
#include <vector>
#include "task_queue.hpp"
#include "utils.hpp"

namespace libs {
	namespace analysis {

/**
 * \brief Defines the main engine which is responsible for mining the required usefull information.
 * \tparam T the type of data stored in the map as a key
 * \tparam U the type of data stored in the map as a value
 */
template <typename T, typename U>
class analyze_stats_engine
{
	private:
		std::unordered_map<T, U> m_word_freq{};
		std::unordered_map<T, std::vector<U>> m_smileys{};
		std::unique_ptr<libs::safe_datastructure::task_queue<T, U>> m_queue{};
		std::vector<std::thread> m_threads{};
		std::mutex m_mtx;
		std::condition_variable m_cv;	
	public:
		/**
		 * Constructor with an argument
		 * \param queue the which holds the pending tasks
		 */
		analyze_stats_engine(std::unique_ptr<libs::safe_datastructure::task_queue<T, U>> queue): 
			m_queue(std::move(queue)) {}
		/**
		 * Destructor
		 */
		~analyze_stats_engine() {
		}
		/**
		 * Extracts the tasks from task queue and mines the required information i.e. smileys and their positions, words and their freequencies.
		 * @returns `void`
		 */
		void analyze() {
			if(m_queue) {
				size_t size = m_queue.get()->size();
				for(int i = 0; i < size; ++i) {
					m_threads.emplace_back(std::thread([this]()
					{
						if(m_queue) {
						    auto front = m_queue.get()->pop();
						    m_mtx.lock();
						    libs::utils::search_smileys<T, U>(*front.get(), m_smileys);
						    m_mtx.unlock();
						    std::vector<T> words = libs::utils::split_by_any_of_special_character(std::get<0>(*front.get()));
						    for(auto& word: words) {
						        m_mtx.lock();
							if(!word.empty()) {
							    ++m_word_freq[word];
							}
							m_mtx.unlock();
						    }
						}
                                          
					  ;})
					);
				}
				for(int i = 0; i < m_threads.size(); ++i) {
					m_threads[i].join();
				}
			}
		}
		/**
		 * Gets the task queue
		 * @returns task queue object
		 */
		std::unique_ptr<libs::safe_datastructure::task_queue<T, U>> get_task_queue() {
			return std::move(m_queue);
		}
		/**
		 * Gets the word-frequency hash map
		 * @returns `std::unordered_map<T, U>` where keys are the words and the values are their frequencies
		 */
		std::unordered_map<T, U> get_map() {
			return m_word_freq;
		}
		/**
		 * Gets a hash map which represents smileys and their positions in the input text
		 * @returns `std::unordered_map<T, std::vector<U>>>` where the key is a smiley character and the value is it's positions
		 */
		std::unordered_map<T, std::vector<U>> get_smileys() {
			return m_smileys;
		}
};
}
}

#endif // __ANALYZE_STATISTICS__
