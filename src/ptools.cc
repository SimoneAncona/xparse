/**
 * @file ptools.cc
 * @author Simone Ancona
 * @version 1.0
 * @date 2023-07-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "ptools.hh"

char ParserTools::get_next(std::string str, size_t &index) noexcept
{
    if (index >= str.length()) return 0;
    return str[index++];
}