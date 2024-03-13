#pragma once

#include <uchar.h>

namespace cfg
{

template<typename CHAR_T, CHAR_T epsilon_char>
class Grammar
{
	
};

typedef Grammar<char, '\f'> GrammarASCII;
typedef Grammar<char32_t, 0x000003B5> GrammarUTF8;

}