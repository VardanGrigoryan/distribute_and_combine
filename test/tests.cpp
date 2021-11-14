#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE TEST_IOENGINE
#include <boost/test/included/unit_test.hpp>
#include <unordered_map>
#include <vector>

#include "io_engine.hpp"

struct file_op_fixture
{
	public:
	file_op_fixture(): obj("./test/test_files/file.txt", 64), 
	obj_db("./test/test_files/file.txt", 64, "test_db.db") {
	}
	~file_op_fixture() {}
	libs::proccesing::io_engine<std::string, size_t> obj;
	libs::proccesing::io_engine<std::string, size_t> obj_db;
};

// Testing two generasl statistics, i.e. obtained words and smyles count. 
BOOST_FIXTURE_TEST_CASE(TEST_INPUT_EMPTY_FILE, file_op_fixture)
{
	obj.read();
	// check word cound with no db and concole output configuration
	std::unordered_map<std::string, size_t> freq = obj.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 236);
	// check smileys count
	std::unordered_map<std::string, std::vector<size_t>> smyleis = obj.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis.size(), 5);
}
// Testing input empty file. 
BOOST_FIXTURE_TEST_CASE(TEST_OBTAINED_WORD_AND_SMYLEIS_COUNT, file_op_fixture)
{
	obj.set_file_path("./test/test_files/empty.txt");
	obj.read();
	std::unordered_map<std::string, size_t> freq = obj.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 0);
	std::unordered_map<std::string, std::vector<size_t>> smyleis = obj.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis.size(), 0);
}
// Testing for just one smile. 
BOOST_FIXTURE_TEST_CASE(TEST_JUST_ONE_SMYLE, file_op_fixture)
{
	obj.set_file_path("./test/test_files/just_one_smyle.txt");
	obj.read();
	std::unordered_map<std::string, size_t> freq = obj.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 0);
	std::unordered_map<std::string, std::vector<size_t>> smyleis = obj.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis.size(), 1);
}
// Testing for just one word. 
BOOST_FIXTURE_TEST_CASE(TEST_JUST_ONE_WORD, file_op_fixture)
{
	obj.set_file_path("./test/test_files/just_one_word.txt");
	obj.read();
	std::unordered_map<std::string, size_t> freq = obj.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 1);
	std::unordered_map<std::string, std::vector<size_t>> smyleis = obj.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis.size(), 0);
}
// Testing for no smyle text. 
BOOST_FIXTURE_TEST_CASE(TEST_NO_SMYLE, file_op_fixture)
{
	obj.set_file_path("./test/test_files/no_smyles_text.txt");
	obj.read();
	std::unordered_map<std::string, size_t> freq = obj.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 236);
	std::unordered_map<std::string, std::vector<size_t>> smyleis = obj.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis.size(), 0);
}
// Testing for no words text. 
BOOST_FIXTURE_TEST_CASE(TEST_NO_WORDS, file_op_fixture)
{
	obj.set_file_path("./test/test_files/no_words_text.txt");
	obj.read();
	std::unordered_map<std::string, size_t> freq = obj.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 0);
	std::unordered_map<std::string, std::vector<size_t>> smyleis = obj.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis.size(), 6);
	std::unordered_map<std::string, std::vector<size_t>> golden = {
		{":-)", {1, 9, 20}}, {":)", {31}},
		{":]" , {5, 23, 39}}, {":}", {7, 35}},
		{":-}", {12, 26}}, {":-]",{15}}};
	bool result = (smyleis == golden);
	BOOST_CHECK_EQUAL(result, true);
	
}
// Testing smileys by comparing two different ways of obtaining it i.e hash map vs vector
BOOST_FIXTURE_TEST_CASE(TEST_SMILEYS_MAP_VS_VECTOR, file_op_fixture)
{
	obj.set_file_path("./test/test_files/no_words_text.txt");
	obj.read();
	std::unordered_map<std::string, size_t> freq = obj.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 0);
	/* The function get_smileys_map() returns vector of pairs in regards of SQL query notation i.e. one pair of items describes 'CODE' and smiley character
	 * and the second pair describes 'ID' and position value. Here we have 24 items in the vector, so we should take 24/2 and taking ito acount that there are 6 duplicates the actual number becomes 6.
	 * Below is the format:
	 * {{first = "Code", second = ":-)"}, {first = "Id", second = "1"}, {first = "Code", second = ":-)"}, {first = "Id", second = "9"}, 
	 * {first = "Code", second = ":-)"}, {first = "Id", second = "20"}, {first = "Code", second = ":)"}, {first = "Id", second = "31"}, 
	 * {first = "Code", second = ":]"}, {first = "Id", second = "5"}, {first = "Code", second = ":]"}, {first = "Id", second = "23"}, 
	 * {first = "Code", second = ":]"}, {first = "Id", second = "39"}, {first = "Code", second = ":}"}, {first = "Id", second = "7"}, 
	 * {first = "Code", second = ":}"}, {first = "Id", second = "35"}, {first = "Code", second = ":-}"}, {first = "Id", second = "12"}, 
	 * {first = "Code", second = ":-}"}, {first = "Id", second = "26"}, {first = "Code", second = ":-]"}, {first = "Id", second = "15"}}
	 * The content of the map above is convertin to another map which has a format shown in the comment [2].
	 * */
	std::unordered_map<std::string, std::vector<size_t>> smyleis_map = obj.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis_map.size(), 6);
	/* [2] The smyleis_vector has the below given format:
	 * {[":-)"] = std::vector of length 3, capacity 3 = {1, 9, 20}, [":)"] = std::vector of length 1, capacity 1 = {31},
            [":]"] = std::vector of length 3, capacity 3 = {5, 23, 39}, [":}"] = std::vector of length 2, capacity 2 = {7, 35},
            [":-}"] = std::vector of length 2, capacity 2 = {12, 26}, [":-]"] = std::vector of length 1, capacity 1 = {15}}
	 * */
	std::vector<std::pair<std::string, std::string>> smyleis_vector = obj.get_smileys([](size_t pos){return std::to_string(pos);});
	//checking smileys positions
	std::unordered_map<std::string, std::vector<size_t>> golden{};
	for(int i = 0; i < smyleis_vector.size(); i += 2) {
		golden[smyleis_vector[i].second].push_back(std::stoi(smyleis_vector[i + 1].second));
	}
	BOOST_CHECK_EQUAL(golden.size(), smyleis_map.size());
	BOOST_CHECK_EQUAL(smyleis_map.size(), golden.size());
	for(auto& [code, vec]: smyleis_map) {
		std::set<std::vector<size_t>> set_golden;
		set_golden.insert(vec);
		std::set<std::vector<size_t>> set_smyleis_map;
		set_smyleis_map.insert(vec);
		bool is_equal = (set_golden == set_smyleis_map);
		BOOST_CHECK_EQUAL(is_equal, true);
	}
	
}
// Testing for invalid smyles text. 
BOOST_FIXTURE_TEST_CASE(TEST_INVALID_SMYLE, file_op_fixture)
{
	obj.set_file_path("./test/test_files/invalid_smyles.txt");
	obj.read();
	std::unordered_map<std::string, size_t> freq = obj.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 0);
	std::unordered_map<std::string, std::vector<size_t>> smyleis = obj.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis.size(), 0);
}

// TESTS WITH SQL DATABASE CONNECTIONS
// Testing two generasl statistics, i.e. obtained words and smyles count. 
BOOST_FIXTURE_TEST_CASE(TEST_DB_INPUT_EMPTY_FILE, file_op_fixture)
{
	obj_db.read();
	// check word cound with no db and concole output configuration
	std::unordered_map<std::string, size_t> freq = obj_db.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 236);
	// check smileys count
	std::unordered_map<std::string, std::vector<size_t>> smyleis = obj_db.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis.size(), 5);
}
// Testing input empty file. 
BOOST_FIXTURE_TEST_CASE(TEST_DB_OBTAINED_WORD_AND_SMYLEIS_COUNT, file_op_fixture)
{
	obj_db.set_file_path("./test/test_files/empty.txt");
	obj_db.read();
	std::unordered_map<std::string, size_t> freq = obj_db.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 0);
	std::unordered_map<std::string, std::vector<size_t>> smyleis = obj_db.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis.size(), 0);
}
// Testing for just one smile. 
BOOST_FIXTURE_TEST_CASE(TEST_DB_JUST_ONE_SMYLE, file_op_fixture)
{
	obj_db.set_file_path("./test/test_files/just_one_smyle.txt");
	obj_db.read();
	std::unordered_map<std::string, size_t> freq = obj_db.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 0);
	std::unordered_map<std::string, std::vector<size_t>> smyleis = obj_db.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis.size(), 1);
}
// Testing for just one word. 
BOOST_FIXTURE_TEST_CASE(TEST_DB_JUST_ONE_WORD, file_op_fixture)
{
	obj_db.set_file_path("./test/test_files/just_one_word.txt");
	obj_db.read();
	std::unordered_map<std::string, size_t> freq = obj_db.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 1);
	std::unordered_map<std::string, std::vector<size_t>> smyleis = obj_db.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis.size(), 0);
}
// Testing for no smyle text. 
BOOST_FIXTURE_TEST_CASE(TEST_DB_NO_SMYLE, file_op_fixture)
{
	obj_db.set_file_path("./test/test_files/no_smyles_text.txt");
	obj_db.read();
	std::unordered_map<std::string, size_t> freq = obj_db.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 236);
	std::unordered_map<std::string, std::vector<size_t>> smyleis = obj_db.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis.size(), 0);
}
// Testing for no words text. 
BOOST_FIXTURE_TEST_CASE(TEST_DB_NO_WORDS, file_op_fixture)
{
	obj_db.set_file_path("./test/test_files/no_words_text.txt");
	obj_db.read();
	std::unordered_map<std::string, size_t> freq = obj_db.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 0);
	std::unordered_map<std::string, std::vector<size_t>> smyleis = obj_db.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis.size(), 6);
	std::unordered_map<std::string, std::vector<size_t>> golden = {
		{":-)", {1, 9, 20}}, {":)", {31}},
		{":]" , {5, 23, 39}}, {":}", {7, 35}},
		{":-}", {12, 26}}, {":-]",{15}}};
	bool result = (smyleis == golden);
	BOOST_CHECK_EQUAL(result, true);
	
}
// Testing smileys by comparing two different ways of obtaining it i.e hash map vs vector
BOOST_FIXTURE_TEST_CASE(TEST_DB_SMILEYS_MAP_VS_VECTOR, file_op_fixture)
{
	obj_db.set_file_path("./test/test_files/no_words_text.txt");
	obj_db.read();
	std::unordered_map<std::string, size_t> freq = obj_db.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 0);
	/* The function get_smileys_map() returns vector of pairs in regards of SQL query notation i.e. one pair of items describes 'CODE' and smiley character
	 * and the second pair describes 'ID' and position value. Here we have 24 items in the vector, so we should take 24/2 and taking ito acount that there are 6 duplicates the actual number becomes 6.
	 * Below is the format:
	 * {{first = "Code", second = ":-)"}, {first = "Id", second = "1"}, {first = "Code", second = ":-)"}, {first = "Id", second = "9"}, 
	 * {first = "Code", second = ":-)"}, {first = "Id", second = "20"}, {first = "Code", second = ":)"}, {first = "Id", second = "31"}, 
	 * {first = "Code", second = ":]"}, {first = "Id", second = "5"}, {first = "Code", second = ":]"}, {first = "Id", second = "23"}, 
	 * {first = "Code", second = ":]"}, {first = "Id", second = "39"}, {first = "Code", second = ":}"}, {first = "Id", second = "7"}, 
	 * {first = "Code", second = ":}"}, {first = "Id", second = "35"}, {first = "Code", second = ":-}"}, {first = "Id", second = "12"}, 
	 * {first = "Code", second = ":-}"}, {first = "Id", second = "26"}, {first = "Code", second = ":-]"}, {first = "Id", second = "15"}}
	 * The content of the map above is convertin to another map which has a format shown in the comment [2].
	 * */
	std::unordered_map<std::string, std::vector<size_t>> smyleis_map = obj_db.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis_map.size(), 6);
	/* [2] The smyleis_vector has the below given format:
	 * {[":-)"] = std::vector of length 3, capacity 3 = {1, 9, 20}, [":)"] = std::vector of length 1, capacity 1 = {31},
            [":]"] = std::vector of length 3, capacity 3 = {5, 23, 39}, [":}"] = std::vector of length 2, capacity 2 = {7, 35},
            [":-}"] = std::vector of length 2, capacity 2 = {12, 26}, [":-]"] = std::vector of length 1, capacity 1 = {15}}
	 * */
	std::vector<std::pair<std::string, std::string>> smyleis_vector = obj_db.get_smileys([](size_t pos){return std::to_string(pos);});
	//checking smileys positions
	std::unordered_map<std::string, std::vector<size_t>> golden{};
	for(int i = 0; i < smyleis_vector.size(); i += 2) {
		golden[smyleis_vector[i].second].push_back(std::stoi(smyleis_vector[i + 1].second));
	}
	BOOST_CHECK_EQUAL(golden.size(), smyleis_map.size());
	BOOST_CHECK_EQUAL(smyleis_map.size(), golden.size());
	for(auto& [code, vec]: smyleis_map) {
		std::set<std::vector<size_t>> set_golden;
		set_golden.insert(vec);
		std::set<std::vector<size_t>> set_smyleis_map;
		set_smyleis_map.insert(vec);
		bool is_equal = (set_golden == set_smyleis_map);
		BOOST_CHECK_EQUAL(is_equal, true);
	}
	
}
// Testing for invalid smyles text. 
BOOST_FIXTURE_TEST_CASE(TEST_DB_INVALID_SMYLE, file_op_fixture)
{
	obj_db.set_file_path("./test/test_files/invalid_smyles.txt");
	obj_db.read();
	std::unordered_map<std::string, size_t> freq = obj_db.get_map();
	BOOST_CHECK_EQUAL(freq.size(), 0);
	std::unordered_map<std::string, std::vector<size_t>> smyleis = obj_db.get_smileys_map();
	BOOST_CHECK_EQUAL(smyleis.size(), 0);
}
