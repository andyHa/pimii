//
// Created by Andreas Haufler on 28.11.18.
//

#ifndef MEM_TOKENIZER_H
#define MEM_TOKENIZER_H

#include <vector>
#include <deque>
#include <iostream>
#include "../common/ObjectPointer.h"

namespace pimii {

    class BufferedReader {
        std::string_view input;
        size_t pos;
    public:
        explicit BufferedReader(std::string_view input) : input(input), pos(0) {};

        char current();

        char next();

        char offset(size_t offset);

        char consume();

    };

    enum TokenType {
        NAME,
        COLON_NAME,
        LITERAL_STRING,
        LITERAL_NUMBER,
        LITERAL_HEX,
        LITERAL_BINARY,
        LITERAL_FLOAT,
        LITERAL_SYMBOL,
        LITERAL_CHARACTER,
        ASSIGNMENT,
        OPERATOR,
        SEPARATOR,
        COLON,
        SEMICOLON,
        CARET,
        FULLSTOP,
        L_BRACKET,
        R_BRACKET,
        LA_BRACKET,
        RA_BRACKET,
        LC_BRACKET,
        RC_BRACKET,
        INVALID,
        EOI
    };

    struct Error {
        SmallInteger lineNumber;
        std::string message;

        Error(SmallInteger lineNumber, const std::string& message) : lineNumber(lineNumber), message(message) {
            std::cout << lineNumber << ": " << message << std::endl;
        }
    };

    struct Token {
        SmallInteger lineNumber;
        TokenType type;
        std::string value;

        bool isEOI() {
            return type == EOI;
        }

//        Token(SmallInteger lineNumber, TokenType type, std::string value) : lineNumber(lineNumber), type(type),
//                                                                      value(std::move(value)) {};

    };

    class Tokenizer {
        std::string_view input;
        std::deque<Token> bufferedTokens;
        BufferedReader reader;
        SmallInteger line;
        std::vector<Error>& errors;

        Token fetch();

        void skipWhitespaces();

        Token readName();

        Token readNumber();

        bool isOperator(char ch);

        Token readString();

        Token readCharacter();

    public:
        Tokenizer(std::string_view input, std::vector<Error>& errors) : input(input), reader(input), line(1),
                                                                        errors(errors) {}

        Token current();

        Token next();

        Token offset(size_t offset);

        Token consume();

        SmallInteger currentLine();

        //void reportAndConsumeUnexpectedToken
    };

}


#endif //MEM_TOKENIZER_H
