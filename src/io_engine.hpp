#ifndef __IO_ENGINE__
#define __IO_ENGINE__

#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <vector>

#include "analyze_stats_engine.hpp"
#include "db_engine.hpp"
#include "exception.hpp"
#include "task_queue.hpp"


 /// file: io_engine.hpp
 

namespace libs {
	namespace proccesing {
/**
 * @brief Defines the main engine which is responsible for files, DB-queries and task distributions.
 * \tparam T the type of data stored in the map as a key
 * \tparam U the type of data stored in the map as a value
 */
template <typename T, typename U>
class io_engine {
	private:
		using callback = std::function<void(void)>;
		using callback2 = std::function<T(size_t)>;
		void handler(std::tuple<T, U, U>&& tuple, callback&& cb) {
			if(m_queue) {
				m_queue.get()->push(std::move(tuple));
			}
			cb();
		}
		void init() {
			if(!std::filesystem::exists(m_file_path)) {
				std::error_code ec;
				throw std::filesystem::filesystem_error("Cant' find file " + m_file_path, 
						std::move(m_file_path), ec);
			}
			if(!m_db_name.empty()) {
				m_db = std::move(std::make_unique<libs::db::db_engine>(m_db_name));
				if(m_db.get()->open(m_db_name)) {
					throw new libs::exception::custom_exception("Error: Can't open database");
				}
				if(m_db.get()->execute_command("DROP TABLE IF EXISTS FREQUENCY;")) {

					throw new libs::exception::custom_exception("Error: Can't create table");
				}
				if(m_db.get()->execute_command("CREATE TABLE FREQUENCY (NAME TEXT PRIMARY KEY, ID INT);")) {
					throw new libs::exception::custom_exception("Error: Can't create table");
				}
				if(m_db.get()->execute_command("DROP TABLE IF EXISTS SMILEYS;")) {
					throw new libs::exception::custom_exception("Error: Can't create table");
				}
				if(m_db.get()->execute_command("CREATE TABLE SMILEYS (CODE TEXT PRIMARY KEY, POS TEXT);")) {
					throw new libs::exception::custom_exception("Error: Can't create table");
				}
			}
		}
	public:
		/**
		 * The constructor with arguments
		 *
		 * \param file_path the path of the input text file
		 * \param block_size the size of a block which is using to read the input file by chunks
		 * \param db_name the name of a database which could be used to process very large files that can't loaded into theram-memory at once.
		 *      It has default empty string value `""`. If this argument is defined then the database will be used to keep datas on a persisent disk, 
		 *      othewise the ram-memory will be used instead.
		 */
		io_engine(const std::string& file_path, size_t block_size, const std::string& db_name=""): 
			m_file_path(file_path), 
			m_block_size(block_size),
		        m_queue(std::make_unique<libs::safe_datastructure::task_queue<T, U>>()),
	                m_db_name(db_name) {
				init();
			}
		/**
		 * The copy constructor deleted
		 */		 
		io_engine(const io_engine&) = delete;
		/**
		 * The assignement operator deleted
		 */		 
		io_engine& operator=(const io_engine&) = delete;
		/**
		 * The move constructor deleted
		 */		 
		io_engine(const io_engine&&) = delete;
		/**
		 * The move assignement operator deleted
		 */		 
		io_engine& operator=(const io_engine&&) = delete;
		/**
		 * Reads the input text file by chunks and distributes the firther processing to several threads
		 * @returns void
		 */
		void read() {
			std::ifstream is(m_file_path);
			is.seekg (0, is.end);
			int length = is.tellg();
			is.seekg (0, is.beg);
			if(m_block_size > length) {
				m_block_size = length;
			}
			std::vector<char> buffer (m_block_size, 0);
			while (!is.eof()) {
				std::istream& ist = is.read(buffer.data(), buffer.size());
				std::streamsize size = is.gcount();
				int c = ist.peek();
				std::string val(buffer.begin(), buffer.begin() + size);
				if(!is.eof() && c != ' ') {
					std::size_t found = val.find_last_of(" ");
					val = val.substr(0, found);
					is.seekg(is.tellg() - (unsigned)(m_block_size - found), std::ios_base::beg);
				}
				if(m_queue) {
					size_t pos = is.tellg();
					std::unordered_map<T, U> local_word_freq{};
					std::unordered_map<T, std::vector<U>> local_smileys{};
					if(is.eof()) {
						pos = length;
					}
					handler({val, pos, val.length()}, [this, &local_word_freq, &local_smileys](){
							libs::analysis::analyze_stats_engine<T, U> stats(std::move(m_queue));
							stats.analyze();
							std::unordered_map<T, U> word_freq = stats.get_map();
							std::unordered_map<T, std::vector<U>> smileys = stats.get_smileys();
							local_word_freq = stats.get_map();
							local_smileys = stats.get_smileys();
							for(auto&[word, freq]: word_freq) {
							    m_word_freq[word] += freq;
							}
							m_smileys.insert(smileys.begin(), smileys.end());
							m_queue = std::move(stats.get_task_queue());
							;});
					if(!m_db_name.empty()) {
						for(auto& [word, freq]: local_word_freq) {
							if(m_db.get()->execute_command("INSERT INTO FREQUENCY (NAME, ID) VALUES (\"" + word + "\","+ std::to_string(freq) + ") ON CONFLICT(NAME) DO UPDATE SET ID = ID + " + std::to_string(freq) + ";")) {
								throw new libs::exception::custom_exception("Error: Can't insert/update table");
							}
						}
						for(auto& [code, positions]: local_smileys) {
							std::string pos_str{};
							for(auto& pos: positions) {
								pos_str += std::to_string(pos) + " ";
							}
							if(m_db.get()->execute_command("INSERT INTO SMILEYS (CODE, POS) VALUES('" + code + "','" + pos_str + "');")) {
								throw new libs::exception::custom_exception("Error: Can't insert/update table");
							}
						}
					}
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
		std::unordered_map<T, std::vector<U>> get_smileys_map() {
			return m_smileys;
		}
		/**
		 * Performs a db query, obtains smiles and their positions then converts it `std::vector`
		 * @returns `std::vector<std::pair<T, T>>` where the key is a smiley character and the value is it's positions
		 */
		std::vector<std::pair<T, T>> get_smileys(callback2&& cb) {
			if(!m_db_name.empty()) {
				if(m_db.get()->execute_command("SELECT CODE, POS FROM SMILEYS;")) {
					throw new libs::exception::custom_exception("Error: query failed");
				}
				return m_db.get()->get_and_clear_last_query_result();
			}
			std::vector<std::pair<T, T>> ret{};
			for(auto& [code, positions]: m_smileys) {
				for(auto& pos: positions) {
					T key = code;
					T cast = cb(pos);
					T value = cast;
					std::pair<T, T> pair_one = std::make_pair("Code", key);
					std::pair<T, T> pair_two = std::make_pair("Id", value);
					ret.push_back(pair_one);
					ret.push_back(pair_two);
				}
			}
			return ret;
		}
		/**
		 * Performs a db query, obtains n most frequent words
		 * @returns `std::vector<std::pair<T, T>>` where the key is a word and the value is frequency in the input text
		 */
		std::vector<std::pair<T, T>> query_n_most_frequent(const size_t n, callback2&& cb) {
			if(!m_db_name.empty()) {
				if(m_db.get()->execute_command("SELECT * FROM FREQUENCY order by ID desc limit " + std::to_string(n) + ";")) {
					throw new libs::exception::custom_exception("Error: query failed");
				}
				return m_db.get()->get_and_clear_last_query_result();
			}
			const size_t size = m_word_freq.size();
			std::vector<std::vector<T>> counting_vector = std::vector(size, std::vector<T>());
			for(auto& [word, freq]: m_word_freq) {
				counting_vector[freq].push_back(word);
			}
			std::vector<std::pair<T, T>> ret{};
			int k = 0;
			for(int i = counting_vector.size() - 1; i >=0; --i) {
				if(!counting_vector[i].empty()) {
					for(int j = 0; j < counting_vector[i].size(); ++j) {
						if(k >= n) {
							break;
						}
						T key = counting_vector[i][j];
						T cast = cb(i);
						T value = cast;
						std::pair<T, T> pair_one = std::make_pair("Word", key);
						std::pair<T, T> pair_two = std::make_pair("Id", value);
						ret.push_back(pair_one);
						ret.push_back(pair_two);
						++k;
					}
				} else if(k >= n) {
					break;
				}
			}
			return ret;
		}
		/**
		 * Sets input file path
		 * \param file_path the path of input file
		 * @returns `void`
		 */
		void set_file_path(const std::string& file_path) {
#if !defined(_TEST_)
			if(!std::filesystem::exists(file_path)) {
				std::error_code ec;
				throw std::filesystem::filesystem_error("Cant' find file " + file_path, 
						std::move(file_path), ec);
			}
#endif
			m_file_path = file_path;
		}
		/**
		 * Gets input file path
		 * @returns `std::string`
		 */
		std::string get_file_path() const {
			return m_file_path;
		}
	private:
		mutable std::string m_file_path{};
		size_t m_block_size{};
		std::unique_ptr<libs::safe_datastructure::task_queue<T, U>> m_queue;
		const std::string m_db_name;
		std::unique_ptr<libs::db::db_engine> m_db;
		std::unordered_map<T, U> m_word_freq{};
		std::unordered_map<T, std::vector<U>> m_smileys{};
};
}
}
#endif // __IO_ENGINE__
