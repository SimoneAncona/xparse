# Xparser

## Introduction

Xparser is a versatile parsing library that empowers developers with robust parsing capabilities.

## Installation

You can easily install Xparser by downloading the source codes and integrating them into your project.

## Getting Started

### Define a grammar

In order to use Xparser, you need to define your grammar using a simple JSON file.

> A grammar allows Xparser to transform a sequence of characters into a syntax tree.

All JSON Xparser grammar files must have the following structure:

```json
{
    "name": "nameOfYourGrammar",
    "terminals": [
        {
            "name": "nameOfTerminalRule",
            "regex": "ECMAScript regular expression"
        }
    ],
    "rules": [
        {
            "name": "ruleName",
            "expressions": [
                "[b]def<identifier>():"
            ]
        }
    ]
}
```

You can also specify the JSON schema so that you don't run into errors:

```json
{
    "$schema": "https://raw.githubusercontent.com/SimoneAncona/xparser/main/schemas/schema.json",
}
```
For more information <a href="#grammars">click here</a>

### Using Grammars

As mentioned above, we use grammars to generate an abstract syntax tree or AST, you can do it in your C++ project:

```cpp
#include "xparser.hh"
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <iostream>

std::string read_json_file(std::string filename);

int main(int argc, char** argv)
{
    Xpp::Parser parser(read_json_file("myGrammar.json"));       // import the grammar file
    Xpp::AST ast = parser.generate_ast("parse this string");    // parse a string and generate the AST

    std::cout << ast.to_json().to_string() << std::endl;  // see the JSON string representation of the AST
    return 0;
}

std::string read_json_file(std::string filename)
{
    ifstream file;
    stringstream buff;
    file.open(filename);
    if (file.fail())
        throw std::runtime_error("Cannot open the file: " + filename);
    buff << file.rdbuf();
    return buff.str();
}
```

<a name="grammars"></a>
## Grammars

### Terminal Values

A terminal is always a final node in the AST, a terminal value can be a literal number, a literal string or an identifier. There are 3 types of terminal values:
- Predefined: terminals that are built-in such as `integer` or `identifier`
- User-defined: terminals that are defined in the `terminals` property of the grammar JSON file.
- Constant: terminals that are defined in rule expressions, we will see later what this means.

A terminal is defined by a name and a regular expression, except for those constants.

#### Predefined Terminal Values

There are 10 built-in terminals:
- `integer`: that is equivalent to `[-|+]?\d+` regular expression.
- `identifier`: that is equivalent to `[_a-zA-Z][_a-zA-Z0-9]*`.
- `real`: that is equivalent to `[+|-]?\d+(\.\d+)?`.
- `alpha`: that is equivalent to `[a-zA-Z]`.
- `alnum`: equivalent to `[a-zA-Z0-9]`.
- `digit`: equivalent to `[0-9]`.
- `hexDigit`: equivalent to `[0-9a-fA-F]`.
- `octalDigit`: equivalent to `[0-7]`.
- `space`: equivalent to `[^\S\r\n]`.
- `newLine`: equivalent to `\r?\n`.

#### User-defined Terminal

User-defined terminals are defined in the JSON grammar file under the `terminals` property. A terminal is defined by specifying the name and the ECMAScript regular expression.

>NOTE: regular expressions are strings, in order to represent the expression `/[^\S\r\n]/` you must write `"[^\\S\\r\\n]"`.

A user-defined terminal could be like the following.
```json
{
    "terminals": [
        {
            "name": "binaryNumber",
            "regex": "[0|1]+"
        }
    ]
}
```
> NOTE: The order in which they are placed in the array indicates the hierarchy, the topmost terminals will be parsed first.

### Rules

A rule define the syntax of the language and specify how elements of the language are combined. Rules are defined under the `rules` property in the JSON grammar.  
Each rule has a name and a set of expressions which specify the syntax.
```json
{
    "rules": [
        {
            "name": "variableDeclaration",
            "expressions": [
                "[B]var<identifier><newLine>"
            ]
        }
    ]
}
```
> NOTE: The order in which rules are placed in the array indicates a reverse hierarchy, those below are parsed first.

### Rule Expression Language

The rule expression language allows you to specify the syntax of a rule, there are 3 elements in the rule expression language:
- Constant terminals: are used to define strings or sequences of characters that must match exactly in order to form a valid expression or sentence.
- References: references to other rules or terminals, references are delimited by `<>`.
- Flags: flags are always specified at the beginning and are delimited by `[]`.

#### Constant Terminals

As mentioned above, constant terminals tells the parser to match exactly the character sequence. For example:
```json
"[b]if(<condition>)"
```
In this expression, `if` is a constant terminal and tells the parser to match exactly the string "if".

#### References

A reference is a reference to another rule or terminal, that tells the parser to match the string that follow the referenced rule.  
A rule can have a reference to itself provided that in the expression array there is at least one expression with only terminal references or constant terminals.  
Using the previous example:
```json
"[b]if(<condition>)"
```
`<condition>` is a reference to a rule called condition.

#### Quantifiers

A reference can be quantified. There are 5 quantifiers:
- `?`: zero or 1.
- `*`: zero or more.
- `+`: 1 or more.
- `{x}`: exactly x of.
- `{x:y}`: a range from x to y (included).

Quantifiers are placed at the end of the reference like this:
```json
"4letters:<alpha{4}>"
```
The example above specify to match a string that starts with "4letters:" and then followed by exactly 4 alphabetic characters.

#### Alternate

References can be alternated, alternate matches are represented using the `|` character. Each alternative represents a different way to match a part of the expression. For example:
```json
"4letters_or_5num:<alpha{4}|digit{5}>"
```
In this example we match all strings that starts with "4letters_or_5num:" followed by 4 alphabetic characters or 5 decimal digits.

#### Flags
Flags are specified at the beginning of the expression and can change how the expression is evaluated.  
There are 4 flags:
- `b` for **b**oundary: that flag allows you to not specify spaces between different terminals and terminals, rule references and other rules or terminals and rule references.
- `B` for **b**oundary: same as `b` with the difference that `B` guarantees that there is at least 1 space of gap.
> NOTE: you cannot specify both `b` and `B` flags. The `b` flag doesn't work between constant terminals and references whose regular expression also includes the constant terminal.
- `i` for case-**i**nsesitive: all constant terminals are case insensitive.
- `I` for case-**i**nsesitive: all characters of a constant terminal are lower case or upper case, not a mix.
> NOTE: you cannot specify both `i` and `I` flags.

Example:
```json
"[Ib]foreach<space*>(<identifier><space+>in<space+><identifier>)"
```

That expression can match:
- `FOREACH (el in els)`.
- `foreach(  el  IN  els)`.

That expression doesn't match with:
- `Foreach(el in els)`.
- `foreach(elinels)`.

#### Spaces in Rule Expression Language

If not specified, spaces can be evaluated as constant terminal or ignored. Let's see the difference:
```
"hello world<letter{4}> <number{6}>"
      ┃                ┃
      ┃                ┃
      ┗━━━━━━━━━━━━━━━━┻━━ These spaces are constant terminals.

"[b]hello world<letter{4}> <number{6}>"
         ┃                ┃
         ┃                ┗ This space will be ignored.
         ┗ This is space is a constant terminal.

"[b]hello <identifier> world"
         ┃            ┃
         ┃            ┃
         ┗━━━━━━━━━━━━┻━━ These spaces will be ignored
         You must add <space> even if the b flag is set
         This is because 'hello' and 'world' can be seen 
         as identifiers and 'b' does not guarantee that 
         there are no spaces.

"[B]hello <identifier> world"
         ┃            ┃
         ┃            ┃
         ┗━━━━━━━━━━━━┻━━ These spaces will be ignored
         However the 'B' flag ensures that there is at 
         least one space between constant terminals and
         references.
```
