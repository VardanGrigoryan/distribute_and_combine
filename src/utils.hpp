#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <iterator>
#include <regex>
#include <unordered_map>
#include <vector>

namespace libs {

namespace utils {
	/**
	 * Splits the text chunk using most severa; special characters as a delimiters
	 * \param input text chunk
	 * \param eCompress indicates whether to compress intermediate whitespaces
	 * @returns `std::vector<std::string>` splitted and/or compressed text chunk 
	 */
	std::vector<std::string> split_by_any_of_special_character(const std::string& input, 
			boost::algorithm::token_compress_mode_type eCompress=boost::token_compress_on) {
		std::vector<std::string> words{};
		boost::split(words, input, boost::is_any_of("\t\n,.;`\"?<>/+-*!@#$%^&~({[)}]: "), eCompress);
		return words;	
	}
	/**
	 * Uses regular expresions to extract smileys and calculates their global positions into the whole text
	 * \tparam T the key type/smiley character
	 * \tparam U the value type/smileys position
	 * \param tuple holds input text, the global position of it's end and the length of that text
	 * \param smileys represents a reference to an hash map variable which holds smileys and their positions
	 * @returns `void`
	 */
	template <typename T, typename U>
	void search_smileys(const std::tuple<T, U, U>& tuple, 
			std::unordered_map<T, std::vector<U>>& smileys) {
		std::smatch res;
		const std::regex pat("(:-?(\\)|\\}|\\]|\\(|\\{|\\[))");
		const T& item = std::get<0>(tuple);
		for(std::sregex_iterator it = 
				std::sregex_iterator(item.cbegin(), item.cend(), pat); 
				it != std::sregex_iterator(); ++it) {
			std::smatch match = *it;
			smileys[match.str()].push_back(std::get<1>(tuple) - std::get<2>(tuple) + match.position() + 1);
		}
	}
}
}

#endif // __UTILS_HPP__
