#include <iostream>
#include <string>
#include "parser/html/tokenizer.hpp"
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
	std::string txt {"        surpriseeeeeee kos kash"s};
	Tokenizer test{txt};
	test.remove_spaces();
	std::cout << test.consume_word() << std::endl;
	test.remove_spaces();
	std::cout << test.consume_word() << std::endl;
	return 0;
}
