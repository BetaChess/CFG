#include <iostream>

#include <context_free_grammar.hpp>
#include <string>

int main()
{
	setlocale(LC_ALL, "");

	std::u32string test;
	test.resize(4);
	test[0] = 0x000003B5;
	test[1] = 'b';
	test[2] = 'a';
	test[3] = 'c';
	
	for (wchar_t c : test)
		std::wcout << c;
	
	return 0;
}