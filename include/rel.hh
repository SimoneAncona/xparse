/**
 * @file rel.hh
 * @author Simone Ancona
 * @brief Rule Expression Language
 * @version 0.1
 * @date 2023-07-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <string>
#include <vector>

#define CASE_INSENSITIVE_CLEAR 0
#define CASE_INSENSITIVE_STRICT 1
#define CASE_INSENSITIVE_SOFT 2

namespace Xpp
{
    enum ExpressionElementType
    {
        CONSTANT_TERMINAL,
        RULE_REFERENCE,
        TERMINAL_REFERENCE,
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
        int case_insensitive_flag;
        bool boundary_flag;
        bool ignore_spaces;

    public:
        RuleExpression() = default;
        ~RuleExpression() = default;

        bool is_boundary_set();
        bool is_ignore_spaces_set();
        bool is_strict_case_insensitive_set();
        bool is_soft_case_insensitive_set();

        std::vector<ExpressionElement> &get_elements();
        ExpressionElement &operator[](size_t index);

        std::vector<ExpressionElement>::iterator begin();
        std::vector<ExpressionElement>::iterator end();
    };
};