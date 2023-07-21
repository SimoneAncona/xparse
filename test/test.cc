#include "xparser.hh"
#include <iostream>
#include <fstream>

int main(int argc, char **argv)
{
    try
    {
        std::ifstream file;
        file.open("json/grammar1.json");
        Xpp::Parser parser(file);
        Xpp::AST ast = parser.generate_ast("def \"asdfasdf\"");
    }
    catch (const std::exception e)
    {
        std::cout << e.what() << std::endl;
    }
    return 0;
}