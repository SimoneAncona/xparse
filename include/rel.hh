/**
 * @file rel.hh
 * @author Simone Ancona
 * @brief Rule Expression Language
 * @version 1.0
 * @date 2023-07-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <string>
#include <vector>
#include <stdexcept>
#include "ptools.hh"

#define CASE_INSENSITIVE_CLEAR 0
#define CASE_INSENSITIVE_STRICT 1
#define CASE_INSENSITIVE_SOFT 2

namespace Xpp
{
    enum ExpressionElementType
    {
        CONSTANT_TERMINAL,
        RULE_REFERENCE,
        ALTERNATIVE
    };

    enum QuantifierType
    {
        NONE,
        ZERO_OR_ONE,
        ZERO_OR_MORE,
        ONE_OR_MORE,
        EXACT_VALUE,
        EXACT_RANGE
    };

    struct Quantifier
    {
        QuantifierType type;
        size_t x_value;
        size_t y_value;
    };

    struct ExpressionReference
    {
        std::string reference_to;
        Quantifier quantifier;
    };
    
    struct ExpressionElement
    {
        ExpressionElementType type;
        std::string value;
        std::vector<ExpressionReference> references;
    };


    class RuleExpression
    {
    private:
        std::vector<ExpressionElement> elements;
        int case_insensitive_flag = CASE_INSENSITIVE_CLEAR;
        bool boundary_flag = false;
        bool ignore_spaces = false;
        size_t index = 0;

        void parse_flags(std::string);
        void parse_expression(std::string);
        void parse_constant_term(std::string);
        void parse_reference(std::string);
        Quantifier parse_quantifier(std::string, bool, char);

    public:
        RuleExpression() = default;
        ~RuleExpression() = default;
        RuleExpression(std::string);

        bool is_boundary_set() noexcept;
        bool is_ignore_spaces_set() noexcept;
        bool is_strict_case_insensitive_set() noexcept;
        bool is_soft_case_insensitive_set() noexcept;

        std::vector<ExpressionElement> &get_elements() noexcept;
        ExpressionElement &operator[](size_t index) noexcept;

        std::vector<ExpressionElement>::iterator begin();
        std::vector<ExpressionElement>::iterator end();

        size_t get_last_index() noexcept;
    };
};