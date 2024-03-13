#pragma once

#include <optional>
#include <uchar.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace cfg
{
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
		explicit VarOrTerminal(Var var)
			: is_var_(true), var_(var)
		{
		}

		explicit VarOrTerminal(Terminal terminal)
			: is_var_(false), terminal_(terminal)
		{
		}

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
	};

	class DerivationString
	{
	public:

	private:
		std::vector<VarOrTerminal> str;
	};

	class ProductionRule
	{
	public:

	private:
		std::vector<DerivationString> derivations_;
	};

	class Alphabet
	{
	public:
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
	};

public:
	///// Constructors
	// The empty grammar.
	Grammar();

	///// Methods
	/// Mutators

	constexpr void add_rule(VarType ruleName, ProductionRule rule)
	{
		rules_[ruleName] = rule;
	}
	
	/// Getters
	
private:
	std::unordered_map<VarType, ProductionRule> rules_{};
};

typedef Grammar<char, '\f'> GrammarASCII;
typedef Grammar<char32_t, 0x000003B5> GrammarUTF8;
}
