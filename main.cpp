#include <boost/program_options.hpp>
#include <iostream>
#include <variant>

#include "io_engine.hpp"
#include "report_generator.hpp"

namespace po = boost::program_options;

std::variant<po::variables_map, size_t> argparse(int argc, char** argv) {
	po::variables_map vm;
	std::variant<po::variables_map, size_t> var;
	po::options_description arg_desc("Options");
	arg_desc.add_options()
		("help,h", "Show usage")
		("chunk_size,c", po::value<size_t>(), "Indicates in which portions the input text file should be processed.")
		("input_file_path,i", po::value<std::string>(), "The Input file path.")
		("db_path,d", po::value<std::string>(), "Indicates the database file full path if it is going to be used.")
		("top,n", po::value<size_t>(), "Gets n most frequent words.")
		("output_format,f", po::value<std::string>(), "Indicates in which format to represent the output.")
		("output_file_path,o", po::value<std::string>(), "The output file path.");
	try {
		po::store(po::parse_command_line(argc, argv, arg_desc), vm);
		po::notify(vm);
		var = vm;
	} catch(std::exception& ex) {
		size_t status = 1;
		var = status;
	}
	return var;
}

void usage(char** argv) {
	std::cout << "Usage: " << argv[0] << " -c [chunk size] -i [input_file_path] -d [db_path]" << 
		" -n [top] -f [output format] -o [output_file_path]" <<
		"\nArguments descriptions:\n" << "\t-i | input_file_path, The Input file path\n" <<
		"\t-n | top, Gets n most frequent words\n" <<
		"\t-f | output_format, supported formats [xml | file | console], Indicates in which format to represent the output\n" <<
		"\nOptional Arguments:\n" <<
		"\t-c | chunk_size, Indicates in which portions the input text file should be processed\n" <<
		"\t-d | db_path, Indicates the database name if it is going to be used\n" <<
		"\t-o | output_file_path, The output file path\n";
}

int main(int argc, char** argv) {
	if(argc < 7 || argc > 13) {
		std::cout << "Invalid usage, please see the usage below.\n";
		usage(argv);
		return 1;
	}
	std::variant<po::variables_map, size_t> args_var = argparse(argc, argv);
	auto type_var = std::get_if<po::variables_map>(&args_var);
	if(type_var == nullptr) {
		std::cout << "Invalid usage, please see the usage below.\n";
		usage(argv);
		return 1;
	}
	po::variables_map vm = *type_var;
	if(vm.count("help")) { 
		usage(argv);
		return 1;
	}
	if(!vm.count("input_file_path")) { 
		std::cout << "Usage error: Input file path dosen't specified\n";
		return 1;
	}
	size_t chunk_size = 64;
	if(vm.count("chunk_size")) {
		chunk_size = vm["chunk_size"].as<size_t>();
	}
	std::string input_path = vm["input_file_path"].as<std::string>();
	try {
		std::string db_path{};
		if(vm.count("db_path")) {
			db_path = vm["db_path"].as<std::string>();
		}
		libs::proccesing::io_engine<std::string, size_t> io_obj(input_path, chunk_size, db_path);
		io_obj.read();
		if(!vm.count("top")) {
			std::cout << "Usage error: frequency dosen't specified\n";
			return 1;
		}
		size_t top = vm["top"].as<size_t>();
		std::vector<std::pair<std::string, std::string>> response = 
			io_obj.query_n_most_frequent(top, [](size_t n){return std::to_string(n);});
		std::vector<std::pair<std::string, std::string>> smilyes = 
			io_obj.get_smileys([](size_t n){return std::to_string(n);});
		if(vm.count("output_format")) {
			std::string format = vm["output_format"].as<std::string>();
			if((format == "xml" || format == "file") && !vm.count("output_file_path")) {
				std::cout << "Usage error: Missing output file path\n";
				return 1;
			}
			libs::report_generator::report_generator out_gen(response, smilyes);
			if(format == "xml") {
				std::string out_path = vm["output_file_path"].as<std::string>();
				std::ofstream output(out_path);
				out_gen.generate_xml(output);
			} else if(format == "file") {
				std::string out_path = vm["output_file_path"].as<std::string>();
				std::ofstream output(out_path);
				out_gen.generate_file(output);
			} else if(format == "console") {
				out_gen.console_log(std::cout);
			} else {
				std::cout << "Usage error: Invalid output format: " << format << "\n";
				return 1;
			}
		}
	} catch(libs::exception::custom_exception& exp) {
		std::cout << exp.what() << "\n";
		return 1;
	} catch(std::filesystem::filesystem_error const& exp) {
		std::cout << exp.what() << "\n";
		return 1;
	} catch(std::exception& exp) {
		std::cout << exp.what() << "\n";
		return 1;
	} catch(...) {
		std::cout << "Error: Undefined exception" << "\n";
		return 1;
	}
	return 0;
}
