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

Xpp::RuleExpression::RuleExpression(std::string rule_expression)
{
    size_t index;
    if (rule_expression.starts_with('['))
    {
        index++;
        parse_flags(rule_expression, index);
    }
    while (index < rule_expression.length())
        parse_expression(rule_expression, index);
}

void Xpp::RuleExpression::parse_flags(std::string exp, size_t &index)
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

void Xpp::RuleExpression::parse_expression(std::string exp, size_t &index)
{
    if (exp.starts_with('['))
        throw std::runtime_error("Unexpected '[' token. Did you mean '\\['?");
    if (exp.starts_with('<'))
    {
        parse_reference(exp, index);
        return;
    }
    parse_constant_term(exp, index);
}

void Xpp::RuleExpression::parse_reference(std::string exp, size_t &index)
{
    char next = ParserTools::get_next(exp, index);
    std::vector<std::string> reference_names;
    size_t current_name = 0;
    bool is_alternative = false;
    Quantifier quantifier = {NONE, 0, 0};

    while (next != '>')
    {
        if (next == '\0')
            throw std::runtime_error("Unexpected the end of the expression. Did you forget '>'?");

        if (next == '|')
        {
            reference_names.push_back("");
            current_name++;
            is_alternative = true;
            continue;
        }

        if (next == '?' || next == '*' || next == '+' || next == '{')
        {
            if (is_alternative)
                throw std::runtime_error("Unexpected '" + std::string(1, next) + "' quantifier.");
            quantifier = parse_quantifier(exp, index);
            elements.push_back(ExpressionElement{RULE_REFERENCE, "", {ExpressionReference{reference_names[0], quantifier}}});
            return;
        }

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

        for (auto name : reference_names)
        {
            references.push_back(ExpressionReference{name, quantifier});
        }
        elements.push_back(ExpressionElement{ALTERNATIVE, "", references});
        return;
    }
    elements.push_back(ExpressionElement{RULE_REFERENCE, "", {ExpressionReference{reference_names[0], quantifier}}});
}

Xpp::Quantifier Xpp::RuleExpression::parse_quantifier(std::string exp, size_t &index)
{
    char type = exp[index];
    char ch;
    if ((ch = ParserTools::get_next(exp, index)) != '>')
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

void Xpp::RuleExpression::parse_constant_term(std::string exp, size_t &index)
{
    
}