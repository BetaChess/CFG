#include <iostream>

#include <context_free_grammar.hpp>
#include <string>

int main()
{
	setlocale(LC_ALL, "");

	cfg::GrammarUTF8 grammar = cfg::GrammarBuilder(
		U"<A> -> ε <Q> | s | ε\n"
		"<B> -> h <A>gklllll | e | gglll <C>\n"
		"<C> -> ε | <B> | <A>"
	).build();
	
	auto test = grammar.grammar_to_stringlike<std::u32string>();

	for (wchar_t c: test)
		std::wcout << c;

	return 0;
}
