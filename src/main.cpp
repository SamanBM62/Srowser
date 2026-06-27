#include <iostream>
#include <sstream>
#include <string>
#include "parser/html/tokenizer.hpp"
#include <fstream>
#include <filesystem>
struct Tracker {
	Tracker(std::string const& txt):kir{txt}{
		puts("kos kash");
	};
    Tracker() { puts("default"); }
    Tracker(const Tracker&) { puts("COPY"); }
    Tracker(Tracker&&) { puts("MOVE"); }
    ~Tracker() { puts("dtor"); }
	std::string const kir;
};


Tracker fun1() { return Tracker{"kos"}; }
Tracker fun2() { return fun1(); }

int main() {
	using namespace std::string_literals;
	std::cout << "Current directory: "
          << std::filesystem::current_path() << '\n';

	std::fstream htmlFile {"src/test.html"};
	std::stringstream buffer {};

	buffer << htmlFile.rdbuf();

	Tokenizer test{buffer.str()};
	test.main_loop();
	
	return 0;
}
