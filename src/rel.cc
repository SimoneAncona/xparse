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
    else parse_expression(rule_expression, index);
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
        default:
            throw std::runtime_error("Unrecognized '" + std::string(1, next) + "' flag");
        }
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