#include <iostream>
#include "../include/json.hh"

void test1();
void test2();
void test3();

int main()
{
    try
    {
        test1();
    }
    catch (std::exception e)
    {
        std::cout << "TEST 1 " << e.what() << std::endl;
    }
}

void test1()
{
    std::string json_string = "\
    {\
        \"name\": \"Simone\",\
        \"surname\": \"Ancona\",\
        \"address\": \"Via \\\"ciao\\\" B 6/8\",\
        \"friends\": [\
            {\"name\": \"Luca\", \"age\": 16},\
            {\"name\": \"Frank\", \"age\": 20},\
            {\"name\": \"Andrea\", \"age\": 67}\
        ]\
    }\
    ";
    std::cout << "parsing: " << json_string << std::endl;
    Jpp::Json json;
    json.parse(json_string);

    std::cout << json.to_string() << std::endl;
}

void test2()
{

}

void test3()
{

}