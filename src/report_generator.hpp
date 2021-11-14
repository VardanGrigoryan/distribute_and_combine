#ifndef __DB2XML_HPP__
#define __DB2XML_HPP__

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <fstream>
#include <memory>

#include "exception.hpp"

namespace libs {
	namespace report_generator {

	class report_generator {
	private:
		struct Node {
			std::string word;
			std::string frequency;
		};
		using key_val_pair = std::pair<std::string, std::string>;
		using array_of_entries = std::vector<key_val_pair>;
		std::unique_ptr<array_of_entries> m_frequency_entries;
		std::unique_ptr<array_of_entries> m_smilye_entries;
		using array_of_frequency_nodes = std::vector<Node>;
		using array_of_smilye_nodes = std::vector<Node>;
		std::unique_ptr<array_of_frequency_nodes> m_frequency_nodes;
		std::unique_ptr<array_of_smilye_nodes> m_smilye_nodes;
	private:
		void init() {
			if(m_frequency_entries && m_frequency_nodes) {
				//if(m_frequency_entries.get()->empty() || m_smilye_entries.get()->empty()) {
				//	const std::string err_msg("Error: Can't generate output report, empty response");
				//	throw custom_exception(err_msg.c_str()); 
				//}
				const size_t size = m_frequency_entries.get()->size();
				for(int i = 0; i < size; i += 2) {
					try {
						Node node{m_frequency_entries.get()->at(i).second, m_frequency_entries.get()->at(i + 1).second};
						m_frequency_nodes.get()->push_back(std::move(node));
					} catch (const std::out_of_range& err) {
						const std::string err_msg("Error: Trying to access to the word which dosen't exists");
						throw libs::exception::custom_exception(err_msg.c_str()); 
					}
				}
				const size_t sz = m_smilye_entries.get()->size();
				for(int i = 0; i < sz; i += 2) {
					try {
						Node node{m_smilye_entries.get()->at(i).second, m_smilye_entries.get()->at(i + 1).second};
						m_smilye_nodes.get()->push_back(std::move(node));
					} catch (const std::out_of_range& err) {
						const std::string err_msg("Error: Trying to access out of range element");
						throw libs::exception::custom_exception(err_msg.c_str()); 
					}
				}
			}
		}
	public:
		/**
		 * @brief Constructor with arguments
		 * \param frequency_entries represents a vector of pairs of words and frequencies
		 * \param smiley_entries represents a vector of pairs of smileys and positions
		 */
		report_generator(const array_of_entries& frequency_entries, const array_of_entries& smiley_entries): 
			m_frequency_entries(std::make_unique<array_of_entries>()), 
			m_smilye_entries(std::make_unique<array_of_entries>()), 
			m_frequency_nodes(std::make_unique<array_of_frequency_nodes>()),
			m_smilye_nodes(std::make_unique<array_of_smilye_nodes>()) {
				m_frequency_entries.get()->insert(m_frequency_entries.get()->end(), 
						frequency_entries.begin(), frequency_entries.end());
				m_smilye_entries.get()->insert(m_smilye_entries.get()->end(), 
						smiley_entries.begin(), smiley_entries.end());
				init();
			}
		/**
		 * Generates the output XML file
		 * \param os a `std::ostream&` object
		 * @returns `std::ostream&`
		 */
		std::ostream& generate_xml(std::ostream& os) {
			boost::property_tree::ptree pt;
			BOOST_FOREACH(report_generator::Node n, *m_frequency_nodes.get()) {
				boost::property_tree::ptree& node = pt.add("Report.Word", "");
				node.put("word", n.word);
				node.put("frequency", n.frequency);
			}
			BOOST_FOREACH(report_generator::Node n, *m_smilye_nodes.get()) {
				boost::property_tree::ptree& node = pt.add("Report.Smiley", "");
				node.put("code", n.word);
				node.put("position", n.frequency);
			}
			boost::property_tree::write_xml(os, pt);
			return os;
		}
		friend std::ostream& operator<<(std::ostream& os, const report_generator& this_obj);
		/**
		 * Generates the output TEXT file
		 * \param os a `std::ostream&` object
		 * @returns `std::ostream&`
		 */
		std::ostream& generate_file(std::ostream& os) {
			return operator<<(os, *this);
		}
		/**
		 * Prints the output into console
		 * \param os a `std::ostream&` object
		 * @returns `std::ostream&`
		 */
		std::ostream& console_log(std::ostream& os) {
			return operator<<(os, *this);
		}
};

std::ostream& operator<<(std::ostream& os, const report_generator& this_obj) {
	const size_t size = this_obj.m_frequency_nodes.get()->size();
	os << "Words and their frequencies\n";
	for(int i = 0; i < size; ++i) {
		os << "Word: " << this_obj.m_frequency_nodes.get()->at(i).word << 
			",\n" << "Frequency: " << 
			this_obj.m_frequency_nodes.get()->at(i).frequency << ";\n";
	}
	const size_t sz = this_obj.m_smilye_nodes.get()->size();
	os << "\nSmileys and their positions\n";
	for(int i = 0; i < sz; ++i) {
		os << "Smiley: " << this_obj.m_smilye_nodes->at(i).word << 
			",\n" << "Position: " << 
			this_obj.m_smilye_nodes->at(i).frequency << ";\n";
	}
	return os;
}
}
}

#endif //__DB2XML_HPP__
