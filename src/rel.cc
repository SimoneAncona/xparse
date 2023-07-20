/**
 * @file rel.cc
 * @author Simone Ancona
 * @version 1.0
 * @date 2023-07-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "rel.hh"

inline bool Xpp::RuleExpression::is_boundary_set() noexcept
{
    return this->boundary_flag;
}

inline bool Xpp::RuleExpression::is_ignore_spaces_set() noexcept
{
    return this->ignore_spaces;
}

inline bool Xpp::RuleExpression::is_soft_case_insensitive_set() noexcept
{
    return this->case_insensitive_flag == CASE_INSENSITIVE_SOFT;
}

inline bool Xpp::RuleExpression::is_strict_case_insensitive_set() noexcept
{
    return this->case_insensitive_flag == CASE_INSENSITIVE_STRICT;
}

inline std::vector<Xpp::ExpressionElement> &Xpp::RuleExpression::get_elements() noexcept
{
    return this->elements;
}

inline std::vector<Xpp::ExpressionElement>::iterator Xpp::RuleExpression::begin()
{
    return this->elements.begin();
}

inline std::vector<Xpp::ExpressionElement>::iterator Xpp::RuleExpression::end()
{
    return this->elements.end();
}

inline Xpp::ExpressionElement &Xpp::RuleExpression::operator[](size_t index) noexcept
{
    return this->elements[index];
}

Xpp::RuleExpression::RuleExpression(std::string rule_expression)
{
    if (rule_expression.starts_with('['))
    {
        index++;
        parse_flags(rule_expression);
    }
    while (index < rule_expression.length())
        parse_expression(rule_expression);
}

void Xpp::RuleExpression::parse_flags(std::string exp)
{
    char next = ParserTools::get_next(exp, index);
    while (next != ']')
    {
        switch (next)
        {
        case '\0':
            throw std::runtime_error("Unexpected the end of the expression, ']' was expected");
        case 'i':
            if (case_insensitive_flag == CASE_INSENSITIVE_STRICT)
                throw std::runtime_error("Cannot set 'i' flag after 'I' was already set");
            if (case_insensitive_flag == CASE_INSENSITIVE_SOFT)
                throw std::runtime_error("'i' flag is already set");
            case_insensitive_flag = CASE_INSENSITIVE_SOFT;
            break;
        case 'I':
            if (case_insensitive_flag == CASE_INSENSITIVE_SOFT)
                throw std::runtime_error("Cannot set 'I' flag after 'i' was already set");
            if (case_insensitive_flag == CASE_INSENSITIVE_STRICT)
                throw std::runtime_error("'I' flag is already set");
            case_insensitive_flag = CASE_INSENSITIVE_STRICT;
            break;
        case 'b':
            if (boundary_flag)
                throw std::runtime_error("'b' flag is already set");
            boundary_flag = true;
            break;
        case 's':
            if (ignore_spaces)
                throw std::runtime_error("'s' flag is already set");
            ignore_spaces = true;
            break;
        case '<':
            throw std::runtime_error("Unrecognized '<' token. Did you forget ']'?");
        default:
            throw std::runtime_error("Unrecognized '" + std::string(1, next) + "' flag");
        }
        next = ParserTools::get_next(exp, index);
    }
}

void Xpp::RuleExpression::parse_expression(std::string exp)
{
    if (exp[index] == '[')
        throw std::runtime_error("Unexpected '[' token. Did you mean '\\['?");
    if (exp[index] == '<')
    {
        index++;
        parse_reference(exp);
        return;
    }
    parse_constant_term(exp);
}

void Xpp::RuleExpression::parse_reference(std::string exp)
{
    char next = ParserTools::get_next(exp, index);
    std::vector<std::string> reference_names = {""};
    size_t current_name = 0;
    bool is_alternative = false;
    std::vector<Quantifier> quantifiers = {Quantifier{NONE, 0, 0}};

    while (true)
    {
        if (next == '\0')
            throw std::runtime_error("Unexpected the end of the expression. Did you forget '>'?");

        if (next == '|')
        {
            reference_names.push_back("");
            quantifiers.push_back(Quantifier{NONE, 0, 0});
            current_name++;
            is_alternative = true;
            continue;
        }

        if (next == '?' || next == '*' || next == '+' || next == '{')
        {
            quantifiers[current_name] = parse_quantifier(exp, is_alternative, next);
            if (quantifiers[current_name].type != EXACT_VALUE)
            {
                elements.push_back(ExpressionElement{RULE_REFERENCE, "", {ExpressionReference{reference_names[current_name], quantifiers[current_name]}}});
                return;
            }
        }

        if (next == '>')
            break;

        if (!isalnum(next))
            throw std::runtime_error("Unexpected '" + std::string(1, next) + "' token");

        reference_names[current_name] += next;
        next = ParserTools::get_next(exp, index);
    }

    if (is_alternative)
    {
        std::vector<ExpressionReference> references;
        if (reference_names[current_name] == "")
            throw std::runtime_error("Unexpected the end of the reference after '|'");

        for (size_t i = 0; i < reference_names.size(); i++)
        {
            references.push_back(ExpressionReference{reference_names[i], quantifiers[i]});
        }
        elements.push_back(ExpressionElement{ALTERNATIVE, "", references});
        return;
    }
    elements.push_back(ExpressionElement{RULE_REFERENCE, "", {ExpressionReference{reference_names[0], quantifiers[0]}}});
}

Xpp::Quantifier Xpp::RuleExpression::parse_quantifier(std::string exp, bool is_alternative, char type)
{
    if (is_alternative && type != '{')
        throw std::runtime_error("Cannot use ?, *, + and range quantifier with an alternative references");
    char ch;
    if ((ch = ParserTools::get_next(exp, index)) != '>' && type != '{')
        throw std::runtime_error("Unexpected '" + std::string(1, ch) + "' token, '>' was expected");
    if (type == '?')
        return Quantifier{ZERO_OR_ONE, 0, 0};
    if (type == '*')
        return Quantifier{ZERO_OR_MORE, 0, 0};
    if (type == '+')
        return Quantifier{ONE_OR_MORE, 0, 0};

    std::vector<std::string> values = {"", ""};
    size_t current_value = 0;
    
    while (ch != '}')
    {
        if (ch == ':')
        {
            if (current_value != 0)
                throw std::runtime_error("Unexpected ':' token");
            current_value = 1;
        }

        if (!isdigit(ch))
            throw std::runtime_error("Unexpected token '" + std::string(1, ch) + "' in {} quantifier");
        
        values[current_value] += ch;
        ch = ParserTools::get_next(exp, index);
    }

    if (current_value == 1)
    {
        if (is_alternative)
            throw std::runtime_error("Cannot use range quantifier with alternative references");
        if (values[0] == "")
            throw std::runtime_error("Expected value before ':' in '{:" + values[1] + "}'");
        if (values[current_value] == "")
            throw std::runtime_error("Expected value after ':' in '{" + values[0] + ":}'");
        return Quantifier{EXACT_RANGE, std::stoull(values[0]), std::stoull(values[1])};
    }
    if (values[0] == "")
        throw std::runtime_error("Expected a value after '{'");
    return Quantifier{EXACT_VALUE, std::stoull(values[0]), 0};

}

void Xpp::RuleExpression::parse_constant_term(std::string exp)
{
    std::string value;
    char ch;
    bool escape = false;

    while (true)
    {
        ch = ParserTools::get_next(exp, index);
        if (ch == '\0')
            break;
        if (escape)
        {
            escape = false;
            switch (ch)
            {
            case 't':
                value += '\t';
                break;
            case 'b':
                throw std::runtime_error("Cannot match \\b character");
            case 'n':
                value += '\n';
                break;
            case 'r':
                value += '\r';
                break;
            case 'v':
                value += '\v';
                break;
            case '0':
                value += '\0';
                break;
            default:
                value += ch;
                break;
            }
            continue;
        }

        if (ch == '\\')
        {
            escape = true;
            continue;
        }

        if (ch == '<')
        {
            --index;
            break;
        }
        if (ch == '>')
            throw std::runtime_error("Unexpected '>' token");
        value += ch;
    }

    elements.push_back(ExpressionElement{CONSTANT_TERMINAL, value, {}});
}

inline size_t Xpp::RuleExpression::get_last_index() noexcept
{
    return index;
}