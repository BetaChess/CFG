#pragma once

#include <optional>
#include <uchar.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace cfg
{
template<typename T, typename U>
concept compatible_string_type = std::ranges::random_access_range<T> && sizeof(U) <= sizeof(decltype((T())[0]));

template<typename CHAR_T, CHAR_T epsilon_char>
class Grammar
{
public:
	typedef std::string VarType;

	struct Var
	{
		VarType identifier;
	};

	struct Terminal
	{
		CHAR_T terminal;
	};

	class VarOrTerminal
	{
	public:
		constexpr VarOrTerminal() : is_var_(false), terminal_(epsilon_char)
		{
		}

		constexpr explicit VarOrTerminal(Var var)
			: is_var_(true), var_(var)
		{
		}

		constexpr explicit VarOrTerminal(Terminal terminal)
			: is_var_(false), terminal_(terminal)
		{
		}

		// Copy constructor

		constexpr VarOrTerminal(const VarOrTerminal &other)
			: is_var_(other.is_var_)
		{
			if (is_var_)
			{
				new(&var_) Var(other.var_);
			} else
			{
				new(&terminal_) Terminal(other.terminal_);
			}
		}

		// Copy assignment

		constexpr VarOrTerminal &operator=(const VarOrTerminal &other)
		{
			if (this == &other)
			{
				return *this;
			}

			if (is_var_)
			{
				if (other.is_var_)
				{
					var_ = other.var_;
				} else
				{
					var_.~Var();
					new(&terminal_) Terminal(other.terminal_);
				}
			} else
			{
				if (other.is_var_)
				{
					terminal_.~Terminal();
					new(&var_) Var(other.var_);
				} else
				{
					terminal_ = other.terminal_;
				}
			}

			is_var_ = other.is_var_;

			return *this;
		}

		~VarOrTerminal()
		{
			if (is_var_) var_.~Var();
			else terminal_.~Terminal();
		};

		[[nodiscard]] constexpr bool is_var() const { return is_var_; };

		[[nodiscard]] constexpr bool is_terminal() const { return !is_var_; };

		[[nodiscard]] constexpr Var get_var_unsafe() const
		{
			return var_;
		};

		[[nodiscard]] constexpr Terminal get_terminal_unsafe() const
		{
			return terminal_;
		};

		[[nodiscard]] constexpr std::optional<Var> get_var() const
		{
			if (is_var_)
			{
				return var_;
			} else
			{
				return std::nullopt;
			}
		};

		[[nodiscard]] constexpr std::optional<Terminal> get_terminal() const
		{
			if (!is_var_)
			{
				return terminal_;
			} else
			{
				return std::nullopt;
			}
		};

		[[nodiscard]] constexpr bool operator==(const VarOrTerminal &other) const
		{
			if (is_var_ != other.is_var_)
			{
				return false;
			}
			if (is_var_)
			{
				return var_.identifier == other.var_.identifier;
			} else
			{
				return terminal_.terminal == other.terminal_.terminal;
			}
		};

		[[nodiscard]] constexpr bool operator!=(const VarOrTerminal &other) const
		{
			return !(*this == other);
		};

		[[nodiscard]] constexpr bool operator==(const Var &var) const
		{
			if (is_var_)
			{
				return var_.identifier == var.identifier;
			} else
			{
				return false;
			}
		};

		[[nodiscard]] constexpr bool operator!=(const Var &var) const
		{
			return !(*this == var);
		};

		[[nodiscard]] constexpr bool operator==(const Terminal &terminal) const
		{
			if (!is_var_)
			{
				return terminal_.terminal == terminal.terminal;
			} else
			{
				return false;
			}
		};

		[[nodiscard]] constexpr bool operator!=(const Terminal &terminal) const
		{
			return !(*this == terminal);
		};

	private:
		bool is_var_;

		union
		{
			Var var_;
			Terminal terminal_;
		};

		friend Grammar;
	};

	class DerivationString
	{
	public:
		constexpr DerivationString() = default;

		constexpr DerivationString(VarOrTerminal var_or_terminal)
		{
			add_val_or_terminal(var_or_terminal);
		}

		~DerivationString() = default;

		constexpr void add_val_or_terminal(VarOrTerminal val_or_terminal)
		{
			if (!val_or_terminal.is_var_)
			{
				if (val_or_terminal.get_terminal()->terminal == epsilon_char)
				{
					return;
				}
			}
			
			str.emplace_back(val_or_terminal);
		}

		constexpr void add_val_or_terminal(Var var)
		{
			str.emplace_back(var);
		}

		constexpr void add_val_or_terminal(Terminal terminal)
		{
			if (terminal.terminal == epsilon_char)
				return;
			
			str.emplace_back(terminal);
		}

	private:
		std::vector<VarOrTerminal> str{};

		friend Grammar;
	};

	class ProductionRule
	{
	public:
		ProductionRule() = default;

		~ProductionRule() = default;

		constexpr void add_derivation(DerivationString derivation_string)
		{
			derivations_.emplace_back(derivation_string);
		}

	private:
		std::vector<DerivationString> derivations_{};

		friend Grammar;
	};

	class Alphabet
	{
	public:
		Alphabet() = default;

		explicit Alphabet(std::vector<Terminal> terminals)
			: symbols_(terminals)
		{
		}

		explicit Alphabet(std::vector<CHAR_T> terminal_symbols)
		{
			terminal_symbols.reserve(terminal_symbols.size());
			for (auto symbol: terminal_symbols)
			{
				symbols_.emplace_back(symbol);
			}
		}

		~Alphabet() = default;

		[[nodiscard]] constexpr bool is_in_alphabet(CHAR_T symbol) const
		{
			for (auto terminal: symbols_)
			{
				if (terminal.terminal == symbol)
				{
					return true;
				}
			}
			return false;
		}

		[[nodiscard]] constexpr bool is_in_alphabet(Terminal symbol) const
		{
			for (auto terminal: symbols_)
			{
				if (terminal.terminal == symbol)
				{
					return true;
				}
			}
			return false;
		}

	private:
		std::vector<Terminal> symbols_{};

		friend Grammar;
	};

public:
	static constexpr CHAR_T epsilon = epsilon_char;

public:
	///// Constructors
	// The empty grammar.
	constexpr Grammar() = default;

	///// Methods
	/// Mutators

	constexpr void add_rule(VarType ruleName, ProductionRule rule)
	{
		rules_[ruleName] = rule;
	}

	/// Getters

	template<typename T>
		requires compatible_string_type<T, CHAR_T>
	constexpr T grammar_to_stringlike()
	{
		constexpr CHAR_T lf = static_cast<CHAR_T>(0x0A);
		T str;
		str += static_cast<CHAR_T>('G');
		str += static_cast<CHAR_T>(' ');
		str += static_cast<CHAR_T>('=');
		str += static_cast<CHAR_T>(' ');
		str += static_cast<CHAR_T>('{');

		for (const auto &[identifier, rule]: rules_)
		{
			str += lf;
			str += static_cast<CHAR_T>('\t');
			str += static_cast<CHAR_T>('<');
			for (auto c: identifier)
				str += static_cast<CHAR_T>(c);
			str += static_cast<CHAR_T>('>');
			str += static_cast<CHAR_T>(' ');
			str += static_cast<CHAR_T>('-');
			str += static_cast<CHAR_T>('>');


			if (rule.derivations_.empty())
			{
				str += static_cast<CHAR_T>(' ');
				str += epsilon_char;
			}
			else
			{
				if (rule.derivations_[0].str.empty())
				{
					str += static_cast<CHAR_T>(' ');
					str += epsilon_char;
				}
				
				for (const auto &val_or_terminal: rule.derivations_[0].str)
				{
					str += static_cast<CHAR_T>(' ');
					if (val_or_terminal.is_terminal())
					{
						str += val_or_terminal.get_terminal_unsafe().terminal;
					} else
					{
						str += static_cast<CHAR_T>('<');
						for (auto c: val_or_terminal.get_var_unsafe().identifier)
							str += static_cast<CHAR_T>(c);
						str += static_cast<CHAR_T>('>');
					}
				}
			}

			for (size_t i = 1; i < rule.derivations_.size(); i++)
			{
				const auto &derivation = rule.derivations_[i];

				str += static_cast<CHAR_T>(' ');
				str += static_cast<CHAR_T>('|');

				if (derivation.str.empty())
				{
					str += static_cast<CHAR_T>(' ');
					str += epsilon_char;
				}

				for (const auto &val_or_terminal: derivation.str)
				{
					str += static_cast<CHAR_T>(' ');
					if (val_or_terminal.is_terminal())
					{
						str += val_or_terminal.get_terminal_unsafe().terminal;
					} else
					{
						str += static_cast<CHAR_T>('<');
						for (auto c: val_or_terminal.get_var_unsafe().identifier)
							str += static_cast<CHAR_T>(c);
						str += static_cast<CHAR_T>('>');
					}
				}
			}
		}

		return str;
	}

private:
	std::unordered_map<VarType, ProductionRule> rules_{};
};

typedef Grammar<char, '\f'> GrammarASCII;
typedef Grammar<char32_t, 0x000003B5> GrammarUTF8;

class GrammarBuilder
{
public:
	constexpr GrammarBuilder(std::u32string str)
		: str_(str), curr_(str_.begin())
	{
	}

	constexpr GrammarBuilder(std::u32string str, GrammarUTF8::Alphabet alphabet)
		: str_(str), curr_(str_.begin()), alphabet_(alphabet), uses_alphabet_(true)
	{
	}

	[[nodiscard]] GrammarUTF8 build()
	{
		GrammarUTF8 grammar;

		while (!is_eof())
		{
			auto next_var = get_next_var();
			if (!next_var.has_value())
				break;

			GrammarUTF8::ProductionRule rule;

			if (!consume_next_implies())
				throw std::invalid_argument("Could not consume implication");

			while (!is_eof() && peek(0) != U'\n')
			{
				GrammarUTF8::DerivationString der;

				while (!is_eof() && peek(0) != U'\n')
				{
					auto next_var_or_terminal = get_next_var_or_terminal();
					if (!next_var_or_terminal.has_value())
						throw std::invalid_argument("Could not consume next var or terminal");

					der.add_val_or_terminal(next_var_or_terminal.value());

					// Remove '|' if it's there and break to the next derivation.
					consume_white_space();
					if (peek(0) == U'|')
					{
						consume();
						break;
					}
				}

				rule.add_derivation(der);
			}

			grammar.add_rule(next_var.value(), rule);
		}

		return grammar;
	}

private:
	std::u32string str_;
	std::u32string::const_iterator curr_;

	GrammarUTF8::Alphabet alphabet_;
	bool uses_alphabet_ = false;

	[[nodiscard]] constexpr char32_t peek(unsigned long offset = 1) const
	{
		return *(curr_ + static_cast<long>(offset));
	}

	[[nodiscard]] constexpr char32_t consume()
	{
		return *curr_++;
	}

	[[nodiscard]] constexpr bool is_eof() const
	{
		return curr_ == str_.end();
	}

	constexpr void consume_white_space()
	{
		while (!is_eof() && *curr_ != U'\n' && std::isspace(static_cast<int>(*curr_)))
		{
			++curr_;
		}
	}

	/// @return Returns the next non-white space character or std::nullopt if eof is reached. Note new line is not white space in this context.
	[[nodiscard]] constexpr std::optional<char32_t> consume_next_non_white_space()
	{
		consume_white_space();
		if (is_eof())
			return std::nullopt;
		return consume();
	}

	/// @param c Character to end on (will not be included in the return string)
	/// @return Returns the resulting string and a boolean indicating if the character was found before eof.
	[[nodiscard]] constexpr std::pair<std::string, bool> consume_until(char32_t c)
	{
		std::string ret;
		if (is_eof())
			return {ret, false};
		char32_t current = consume();
		while (current != c)
		{
			ret += static_cast<char>(current);
			if (is_eof())
				return {ret, false};
			current = consume();
		}

		return {ret, true};
	}

	[[nodiscard]] constexpr std::optional<GrammarUTF8::VarType> get_next_var()
	{
		char32_t current;

		auto next_non_white_space = consume_next_non_white_space();
		if (next_non_white_space.has_value())
			current = next_non_white_space.value();
		else return std::nullopt;

		if (current == U'\n')
		{
			next_non_white_space = consume_next_non_white_space();
			if (next_non_white_space.has_value())
				current = next_non_white_space.value();
			else return std::nullopt;
		}

		if (current != U'<' || is_eof())
			return std::nullopt;

		// Get the identifier
		auto identifier_pair = consume_until(U'>');
		if (identifier_pair.second)
			return identifier_pair.first;
		else
			return std::nullopt;
	}

	constexpr bool consume_next_implies()
	{
		char32_t current;
		auto next_non_white_space = consume_next_non_white_space();
		if (next_non_white_space.has_value())
			current = next_non_white_space.value();
		else return false;

		if (current != U'-')
			return false;
		current = consume();
		if (current != U'>')
			return false;

		return true;
	}

	constexpr std::optional<char32_t> get_next_terminal()
	{
		consume_white_space(); // Should really already be done, but just to be safe.

		if (is_eof() || peek(0) == '\n')
			return std::nullopt;

		return consume();
	}

	constexpr std::optional<GrammarUTF8::VarOrTerminal> get_next_var_or_terminal()
	{
		consume_white_space();
		if (peek(0) == U'<')
		{
			auto id = get_next_var();
			if (id.has_value())
				return GrammarUTF8::VarOrTerminal(GrammarUTF8::Var(id.value()));
			else return std::nullopt;
		}

		auto next_terminal = get_next_terminal();
		if (next_terminal.has_value())
			return GrammarUTF8::VarOrTerminal(GrammarUTF8::Terminal(next_terminal.value()));
		else
			return std::nullopt;
	}
};
}
