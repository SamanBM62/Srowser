#include <iostream>
#include <sstream>
#include <string>
#include "parser/html/parser.hpp"
#include <fstream>
#include <filesystem>

int main() {
	using namespace std::string_literals;
	std::cout << "Current directory: "
          << std::filesystem::current_path() << '\n';

	std::fstream htmlFile {"src/test.html"};
	std::stringstream buffer {};

	buffer << htmlFile.rdbuf();

	std::string text{};
	Parser test{buffer.str()};
	test.main_loop();

	std::cout << "test" << std::endl;
	
	return 0;
}
