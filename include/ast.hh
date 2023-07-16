/**
 * @file ast.hh
 * @author Simone Ancona
 * @version 1.0
 * @date 2023-07-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <string>
#include <vector>
#include <stdexcept>

namespace Xpp
{
    class AST
    {
    private:
        std::vector<AST> children;
        std::string rule_name;
        bool atomic;
        std::string value;

    public:
        /**
         * @brief Construct a new AST object
         * 
         */
        AST();

        /**
         * @brief Construct a new AST object specifying the rule name and the children nodes
         * 
         */
        AST(std::string, std::vector<AST>);

        /**
         * @brief Construct a new AST object specifying the rule name and the atomic value
         * 
         */
        AST(std::string, std::string);

        /**
         * @brief Destroy the AST object
         * 
         */
        ~AST() = default;

        /**
         * @brief Check if is an ending node
         * 
         * @return true 
         * @return false 
         */
        bool is_atomic();

        /**
         * @brief Get the rule name
         * 
         * @return std::string 
         */
        std::string get_rule_name();

        /**
         * @brief Get the atomic value
         * 
         * @return std::string 
         */
        std::string get_value();

        /**
         * @brief Get the children object
         * 
         * @return std::vector<AST>& 
         */
        std::vector<AST> &get_children();

        /**
         * @brief Get a node
         * 
         * @return AST& 
         */
        AST &operator[](size_t);

        std::vector<AST>::iterator begin();

        std::vector<AST>::iterator end();
    };
};
