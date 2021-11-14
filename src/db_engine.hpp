#ifndef __DB_ENGINE__
#define __DB_ENGINE__

#include <climits>
#include <iostream>
#include <memory>
#include <sqlite3.h>
#include <stdio.h>
#include <functional>

#include "exception.hpp"

namespace libs {
	namespace db {
		/**
		 * Wraps up native db queries, by this providing an high level interface sql interaction interface
		 */
class db_engine {
	private:
		sqlite3* m_db;
		mutable std::string m_db_name{};
		int m_status{INT_MIN};
		using key_val_pair = std::pair<std::string, std::string>;
		std::vector<key_val_pair> m_res{};
	private:
		static int callback(void* obj, int argc, char** argv, char** col_name)
		{
			db_engine* this_obj = static_cast<db_engine*>(obj);
			for (int i = 0; i < argc; i++) {
				key_val_pair freq_pair = std::make_pair(static_cast<std::string>(col_name[i]), static_cast<std::string>(argv[i]));
				this_obj->m_res.emplace_back(freq_pair);
			}
			return 0;
		}
	public:
		/**
		 * Defaulted constructor
		 */		 
		db_engine() = default;
		/**
		 * Constructor with an argument
		 * \param db_name is a `std::string` which represents the database name
		 */		 
		db_engine(const std::string& db_name): 
			m_db_name(db_name) {}
		/**
		 * Destructor uses RAII to close connection with db
		 */		 
		~db_engine() {
			try {
				close();
			} catch(std::exception& exp) {
				std::cout << "Error: " << exp.what() << "\n";
			}
		}
		/**
		 * Opens a db connection
		 * \param db_name is a `std::string` which represents the database name
		 * @returns `int` which indecates whether or not the open succeeded
		 */		 
		int open(const std::string& db_name) {
			m_status = sqlite3_open(db_name.c_str(), &m_db);
			//m_status = sqlite3_open_v2(db_name.c_str(), &m_db, SQLITE_OPEN_READWRITE, NULL);
			if (m_status) {
				const std::string err_msg("Error: Can't open database: " + db_name + ", " + sqlite3_errmsg(m_db));
				throw libs::exception::custom_exception(err_msg.c_str());
			}
			return 0;
		}
		/**
		 * Checks whether or not the db connectiom is established
		 * @returns `bool`
		 */
		bool is_open() const {
			return m_status == 0 && m_db != nullptr;
		}
		/**
		 * Executes sql query
		 * \param cmd the sql command that should be executed	 *
		 * @returns `int` which indicates whether the sql query succeeded
		 */
		int execute_command(const std::string& cmd) {
			char* err_msg;
			int status = sqlite3_exec(m_db, cmd.c_str(), callback, this, &err_msg);
			if (status != SQLITE_OK) {
				sqlite3_free(err_msg);
				const std::string err_msg("Error: Can't excute command: " + cmd);
				throw libs::exception::custom_exception(err_msg.c_str());
			}
			return 0;
		}
		/**
		 * Closes the db connection
		 * @returns `void`
		 */
		void close() {
			if(m_db) {
				try {
					sqlite3_close(m_db);
				} catch (std::exception& exp) {
					std::cout << "Error: can't close db connection" << exp.what() << "\n";
				}
			}
		}
		/**
		 * Gets the last query result
		 * @returns `std::vector<std::pair<std::string, std::string>`
		 */
		std::vector<key_val_pair> get_last_query_result() {
			return m_res;
		}
		/**
		 * Gets the last query result and cleans-up it
		 * @returns `std::vector<std::pair<std::string, std::string>`
		 */
		std::vector<key_val_pair> get_and_clear_last_query_result() {
			std::vector<key_val_pair> ret(m_res);
			m_res.clear();
			return ret;
		}
		/**
		 * Cleans the last query result
		 * @returns `void`
		 */
		void clear_last_result() {
			m_res.clear();
		}
};
}
}

#endif // __DB_ENGINE__
