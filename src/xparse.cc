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

    try
    {
        test2();
    }
    catch (std::exception e)
    {
        std::cout << "TEST 2 " << e.what() << std::endl;
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
    Jpp::Json json;
    json.parse(json_string);

    std::cout << json.to_string() << std::endl;

    std::cout << any_cast<std::string>(json["name"].get_value()) << std::endl;
    std::cout << any_cast<std::string>(json["friends"][0]["name"].get_value()) << std::endl;
}

void test2()
{
    std::string json_string = "\
    {\
        \"name\": \"Simone\",\
        \"surname\": \"Ancona\",\
        \"address\": \"Via \\\"ciao\\\" B 6/8\",\
        \"friends\": [\
            {\"name\": \"Luca\", \"age\": 16},\
            {\"name\": \"Frank\", \"age\": 20},\
            {\"name\": \"Andrea\", \"age\": 67},\
        ]\
    }\
    ";
    Jpp::Json json;
    json.parse(json_string);
}

void test3()
{
}