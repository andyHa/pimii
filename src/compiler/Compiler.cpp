//
// Created by Andreas Haufler on 27.11.18.
//

#include "Compiler.h"
#include "../vm/Interpreter.h"
#include "../vm/Methods.h"

namespace pimii {

    const std::vector<Error> &Compiler::getErrors() const {
        return errors;
    }

    ObjectPointer Compiler::compile(System &system) {
        EmitterContext context(system);

        parseSelector(context);
        parseTemporaries(context);

        while (!tokenizer.current().isEOI()) {
            std::unique_ptr<Statement> stmt = statement();
            stmt->emitByteCodes(context);
            if (tokenizer.current().type == FULLSTOP) {
                tokenizer.consume();
            } else {
                Offset lineOrError = tokenizer.currentLine();
                errors.emplace_back(Error(tokenizer.currentLine(), "Expected a '.' at the end of a statement."));
                while (tokenizer.current().lineNumber == lineOrError && !tokenizer.current().isEOI()) {
                    tokenizer.consume();
                }
            }
        }

        Methods methods(system.getMemoryManager(), system.getTypeSystem());
        return methods.createMethod(context.getMaxTemporaries(), context.getLiterals(), context.getOpCodes());
    }

    void Compiler::parseSelector(EmitterContext &ctx) {
        if (tokenizer.current().type == OPERATOR) {
            selector += tokenizer.consume().value;
            if (tokenizer.current().type == NAME) {
                ctx.pushTemporary(tokenizer.consume().value);
            } else {
                Offset lineOrError = tokenizer.currentLine();
                errors.emplace_back(Error(tokenizer.currentLine(), ("Unexpected Token '" + tokenizer.current().value +
                                                                    "'. Expected an argument name.")));
                while (tokenizer.current().lineNumber == lineOrError && !tokenizer.current().isEOI()) {
                    tokenizer.consume();
                }
            }

            return;
        }

        if (tokenizer.current().type == NAME) {
            selector += tokenizer.consume().value;
            return;
        }

        while (tokenizer.current().type == COLON_NAME) {
            selector += tokenizer.consume().value;

            if (tokenizer.current().type == NAME) {
                ctx.pushTemporary(tokenizer.consume().value);
            } else {
                Offset lineOrError = tokenizer.currentLine();
                errors.emplace_back(Error(tokenizer.currentLine(), ("Unexpected Token '" + tokenizer.current().value +
                                                                    "'. Expected an argument name.")));
                while (tokenizer.current().lineNumber == lineOrError && !tokenizer.current().isEOI()) {
                    tokenizer.consume();
                }
                return;
            }
        }
    }

    void Compiler::parseTemporaries(EmitterContext &ctx) {
        if (tokenizer.current().type != OPERATOR || tokenizer.current().value != "|") {
            return;
        }

        tokenizer.consume();
        while (tokenizer.current().type == NAME) {
            ctx.pushTemporary(tokenizer.consume().value);
        }

        if (tokenizer.current().type != OPERATOR || tokenizer.current().value != "|") {
            Offset lineOrError = tokenizer.currentLine();
            errors.emplace_back(Error(tokenizer.currentLine(), ("Unexpected Token '" + tokenizer.current().value +
                                                                "'. Expected '|'.")));
            while (tokenizer.current().lineNumber == lineOrError && !tokenizer.current().isEOI()) {
                tokenizer.consume();
            }
        } else {
            tokenizer.consume();
        }
    }

    std::unique_ptr<Statement> Compiler::statement() {
        if (tokenizer.current().type == CARET) {
            tokenizer.consume();
            if (tokenizer.current().type == NAME) {
                if (tokenizer.current().value == "self") {
                    tokenizer.consume();
                    return std::unique_ptr<Expression>(
                            new BuiltinConstant(Interpreter::OP_RETURN, Interpreter::OP_RETURN_RECEIVER_INDEX));
                }
                if (tokenizer.current().value == "true") {
                    tokenizer.consume();
                    return std::unique_ptr<Expression>(
                            new BuiltinConstant(Interpreter::OP_RETURN, Interpreter::OP_RETURN_TRUE_INDEX));
                }
                if (tokenizer.current().value == "false") {
                    tokenizer.consume();
                    return std::unique_ptr<Expression>(
                            new BuiltinConstant(Interpreter::OP_RETURN, Interpreter::OP_RETURN_FALSE_INDEX));
                }
                if (tokenizer.current().value == "nil") {
                    tokenizer.consume();
                    return std::unique_ptr<Expression>(
                            new BuiltinConstant(Interpreter::OP_RETURN, Interpreter::OP_RETURN_NIL_INDEX));
                }
            }

            return std::unique_ptr<Expression>(new Return(expression()));
        }

        if (tokenizer.current().type == NAME && tokenizer.next().type == ASSIGNMENT) {
            if (isupper(tokenizer.current().value[0])) {
                //TODO global assignment
            } else {
                std::unique_ptr<Assignment> assignment(new Assignment());
                assignment->name = tokenizer.consume().value;
                tokenizer.consume();
                assignment->expression = expression();
                return assignment;
            }
        }
        return expression();
    }

    std::unique_ptr<Expression> Compiler::expression() {
        std::unique_ptr<Expression> currentReceiver = atom();
        while (true) {
            if (tokenizer.current().type == NAME) {
                currentReceiver = unaryCall(std::move(currentReceiver));
            }

            if (tokenizer.current().type == OPERATOR) {
                currentReceiver = binaryCall(std::move(currentReceiver));
            }

            if (tokenizer.current().type == COLON_NAME) {
                currentReceiver = selectorCall(std::move(currentReceiver));
            }

            return currentReceiver;
        }

    }

    std::unique_ptr<Expression> Compiler::atom() {
        if (tokenizer.current().type == NAME) {
            return parseName();
        }

        if (tokenizer.current().type == L_BRACKET) {
            tokenizer.consume();
            std::unique_ptr<Expression> result = expression();
            if (tokenizer.current().type != R_BRACKET) {
                errors.emplace_back(Error(tokenizer.currentLine(), ("Unexpected Token '" + tokenizer.current().value +
                                                                    "'. Expected ')'.")));
            } else {
                tokenizer.consume();
            }

            return result;
        }

        if (tokenizer.current().type == LA_BRACKET) {
            return parseBlock();
        }

        if (tokenizer.current().type == LITERAL_STRING) {
            return std::unique_ptr<Expression>(new LiteralString(tokenizer.consume().value));
        }

        if (tokenizer.current().type == LITERAL_SYMBOL) {
            return std::unique_ptr<Expression>(new LiteralSymbol(tokenizer.consume().value));
        }

        if (tokenizer.current().type == LITERAL_NUMBER) {
            return std::unique_ptr<Expression>(new LiteralNumber(std::stoi(tokenizer.consume().value)));
        }

        if (tokenizer.current().type == LITERAL_HEX) {
            return std::unique_ptr<Expression>(new LiteralNumber(std::stoi(tokenizer.consume().value, nullptr, 16)));
        }

        if (tokenizer.current().type == LITERAL_BINARY) {
            return std::unique_ptr<Expression>(new LiteralNumber(std::stoi(tokenizer.consume().value, nullptr, 2)));
        }

        //TODO error
        return std::unique_ptr<Expression>();
    }

    std::unique_ptr<Expression> Compiler::parseName() {
        if (tokenizer.current().value == "self") {
            tokenizer.consume();
            return std::unique_ptr<Expression>(
                    new BuiltinConstant(Interpreter::OP_PUSH, Interpreter::OP_PUSH_RECEIVER_INDEX));
        }
        if (tokenizer.current().value == "true") {
            tokenizer.consume();
            return std::unique_ptr<Expression>(
                    new BuiltinConstant(Interpreter::OP_PUSH, Interpreter::OP_PUSH_TRUE_INDEX));
        }
        if (tokenizer.current().value == "false") {
            tokenizer.consume();
            return std::unique_ptr<Expression>(
                    new BuiltinConstant(Interpreter::OP_PUSH, Interpreter::OP_PUSH_FALSE_INDEX));
        }
        if (tokenizer.current().value == "nil") {
            tokenizer.consume();
            return std::unique_ptr<Expression>(
                    new BuiltinConstant(Interpreter::OP_PUSH, Interpreter::OP_PUSH_NIL_INDEX));
        }
        if (isupper(tokenizer.current().value[0])) {
            return std::unique_ptr<Expression>(new PushGlobal(tokenizer.consume().value));
        } else {
            return std::unique_ptr<Expression>(new PushLocal(tokenizer.consume().value));
        }
    }

    std::unique_ptr<Expression> Compiler::parseBlock() {
        tokenizer.consume();
        std::unique_ptr<Block> block = std::unique_ptr<Block>(new Block());
        while (tokenizer.current().type == COLON) {
            tokenizer.consume();
            if (tokenizer.current().type == NAME) {
                block->temporaries.emplace_back(tokenizer.consume().value);
            } else {
                //TODO
            }
        }

        if (tokenizer.current().type == OPERATOR && tokenizer.current().value == "|") {
            if (block->temporaries.empty()) {
                //TODO
            }
            tokenizer.consume();
        }

        while (tokenizer.current().type != EOI && tokenizer.current().type != RA_BRACKET) {
            block->statements.emplace_back(statement());
            if (tokenizer.current().type == FULLSTOP) {
                tokenizer.consume();
            } else if (tokenizer.current().type != RA_BRACKET) {
                errors.emplace_back(Error(tokenizer.currentLine(), "Expected a '.' at the end of a statement."));
            }
        }

        if (tokenizer.current().type != RA_BRACKET) {
            errors.emplace_back(Error(tokenizer.currentLine(), "Expected a ']' at the end of a block."));
        } else {
            tokenizer.consume();
        }

        return block;
    }

    std::unique_ptr<Expression> Compiler::unaryCall(std::unique_ptr<Expression> receiver) {
        return receiver;
    }

    std::unique_ptr<Expression> Compiler::binaryCall(std::unique_ptr<Expression> receiver) {
        auto call = new MethodCall();
        call->receiver = std::move(receiver);
        call->selector = tokenizer.consume().value;
        call->arguments.emplace_back(atom());

        return std::unique_ptr<Expression>(call);
    }

    std::unique_ptr<Expression> Compiler::selectorCall(std::unique_ptr<Expression> receiver) {
        auto call = new MethodCall();
        call->receiver = std::move(receiver);
        while(tokenizer.current().type == COLON_NAME) {
            call->selector += tokenizer.consume().value;
            call->arguments.emplace_back(expression());
        }
        return std::unique_ptr<Expression>(call);
    }

    std::unique_ptr<Expression> Compiler::continuation() {
        return std::unique_ptr<Expression>();
    }

}
