//
// Created by Andreas Haufler on 28.11.18.
//

#include "Tokenizer.h"

namespace pimii {

    char BufferedReader::current() {
        return offset(0);
    }

    char BufferedReader::next() {
        return offset(1);
    }

    char BufferedReader::offset(size_t offset) {
        if (pos + offset < input.size()) {
            return input[pos + offset];
        }

        return 0;
    }

    char BufferedReader::consume() {
        if (pos < input.size()) {
            return input[pos++];
        }

        return 0;
    }

    Token Tokenizer::fetch() {
        skipWhitespaces();

        if (reader.current() == 0) {
            return {line, EOI, ""};
        }

        if (isalpha(reader.current())) {
            return readName();

        }

        if (isdigit(reader.current()) || ((reader.current() == '-') && isdigit(reader.next()))) {
            return readNumber();
        }

        if (reader.current() == ':') {
            if (reader.next() == '=') {
                reader.consume();
                reader.consume();
                return {line, ASSIGNMENT, ":="};
            } else {
                reader.consume();
                return {line, COLON, ":"};
            }
        }

        if (reader.current() == '#' && isalpha(reader.next())) {
            reader.consume();
            std::string name;
            while (isalpha(reader.current())) {
                name += reader.consume();
            }
            return {line, LITERAL_SYMBOL, name};
        }

        if (reader.current() == '#' && reader.next() == '(') {
            //TODO array
        }

        if (reader.current() == '\'') {
            return readString();
        }

        if (reader.current() == ';') {
            reader.consume();
            return {line, SEMICOLON, ";"};
        }

        if (reader.current() == '^') {
            reader.consume();
            return {line, CARET, "^"};
        }

        if (reader.current() == '.') {
            reader.consume();
            return {line, FULLSTOP, "."};
        }

        if (reader.current() == '(') {
            reader.consume();
            return {line, L_BRACKET, "("};
        }

        if (reader.current() == ')') {
            reader.consume();
            return {line, R_BRACKET, ")"};
        }

        if (reader.current() == '[') {
            reader.consume();
            return {line, LA_BRACKET, "["};
        }

        if (reader.current() == ']') {
            reader.consume();
            return {line, RA_BRACKET, "]"};
        }

        if (reader.current() == '{') {
            reader.consume();
            return {line, LC_BRACKET, "{"};
        }

        if (reader.current() == '}') {
            reader.consume();
            return {line, RC_BRACKET, "}"};
        }

        if (isOperator(reader.current())) {
            std::string op;
            bool allDashes = reader.current() == '-';
            while (isOperator(reader.current())) {
                if (reader.current() != '-') {
                    allDashes = false;
                }
                op += reader.consume();
            }
            if (op.length() >= 5 && allDashes) {
                return {line, SEPARATOR, op};
            }

            return {line, OPERATOR, op};
        }

        std::string invalid;
        while (!isspace(reader.current()) && reader.current() != 0) {
            invalid += reader.consume();
        }
        return {line, INVALID, invalid};
    }

    Token Tokenizer::readString() {
        reader.consume();
        std::__1::string str;
        while (reader.current() != '\'' && reader.current() != 0 && reader.current() != '\n') {
            if (reader.current() == '\\') {
                reader.consume();
            }
            if (reader.current() != 0) {
                str += reader.consume();
            }
        }
        if (reader.current() != '\'') {
            errors.emplace_back(Error(line, "Unterminated string literal."));
        } else {
            reader.consume();
        }
        return {line, LITERAL_STRING, str};
    }

    void Tokenizer::skipWhitespaces() {
        while (isspace(reader.current())) {
            if (reader.consume() == '\n') {
                line++;
            }
        }
    }

    Token Tokenizer::readName() {
        std::string value;
        while (isalnum(reader.current()) || reader.current() == '_') {
            value += reader.consume();
        }

        if (reader.current() == ':') {
            value += reader.consume();
            return {line, COLON_NAME, value};
        }
        return {line, NAME, value};
    }

    Token Tokenizer::readNumber() {
        std::string value;
        if (reader.current() == '-') {
            value += reader.consume();
        } else if (reader.current() == '0') {
            reader.consume();
            if (reader.current() == 'x') {
                reader.consume();
                while (isxdigit(reader.current())) {
                    value += reader.consume();
                }
                if (value.empty()) {
                    return {line, INVALID, "0x"};
                }
                return {line, LITERAL_HEX, value};
            } else if (reader.current() == 'b') {
                reader.consume();
                while (reader.current() == '0' || reader.current() == '1') {
                    value += reader.consume();
                }
                if (value.empty()) {
                    return {line, INVALID, "0b"};
                }
                return {line, LITERAL_BINARY, value};
            } else {
                value += '0';
            }
        }

        bool lastCharacterWasDigit = false;
        while (isdigit(reader.current()) || (reader.current() == '_' && lastCharacterWasDigit)) {
            if (reader.current() != '_') {
                value += reader.consume();
                lastCharacterWasDigit = true;
            } else {
                reader.consume();
                lastCharacterWasDigit = false;
            }
        }

        if (!lastCharacterWasDigit) {
            errors.emplace_back(Error(line, "A numeric literal must not end with an '_'"));
        }

        if (reader.current() != '.' || !isdigit(reader.next())) {
            return {line, LITERAL_NUMBER, value};
        }
        value += reader.consume();
        while (isdigit(reader.current())) {
            value += reader.consume();
        }
        return {line, LITERAL_FLOAT, value};
    }


    Token Tokenizer::current() {
        return offset(0);
    }


    Token Tokenizer::next() {
        return offset(1);
    }


    Token Tokenizer::offset(size_t offset) {
        if (bufferedTokens.empty()) {
            bufferedTokens.emplace_back(fetch());
        }

        while (offset >= bufferedTokens.size() && !bufferedTokens.end()->isEOI()) {
            bufferedTokens.emplace_back(fetch());
        }

        return bufferedTokens[std::min(offset, bufferedTokens.size() - 1)];
    }

    Token Tokenizer::consume() {
        Token result = current();
        if (!result.isEOI()) {
            bufferedTokens.pop_front();
        }

        return result;
    }

    bool Tokenizer::isOperator(char ch) {
        return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '\\' || ch == '%' || ch == '&' || ch == '<' ||
               ch == '>' || ch == '=' || ch == '?' || ch == '|' || ch == '@' || ch == '!' || ch == ',';
    }

    SmallInteger Tokenizer::currentLine() {
        return current().lineNumber;
    }


}