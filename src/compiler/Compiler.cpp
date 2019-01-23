//
// Created by Andreas Haufler on 27.11.18.
//

#include "Compiler.h"
#include "../vm/Interpreter.h"
#include "Methods.h"

namespace pimii {

    ObjectPointer Compiler::compileExpression(System& system) {
        EmitterContext context(system, type);
        return compile(system, context);
    }

    ObjectPointer Compiler::compileMethod(pimii::System& system) {
        EmitterContext context(system, type);
        parseSelector(context);
        return compile(system, context);
    }

    ObjectPointer Compiler::compile(System& system, EmitterContext& context) {
        parseTemporaries(context);

        SmallInteger primitiveIndex = -1;
        if (tokenizer.current().value == "<" && tokenizer.next().value == "Primitive:") {
            tokenizer.consume();
            tokenizer.consume();
            primitiveIndex = atoi(tokenizer.consume().value.c_str());
            if (tokenizer.current().value == ">") {
                tokenizer.consume();
            }
        }

        bool successiveStatement = false;
        while (!tokenizer.current().isEOI() && tokenizer.current().type != SEPARATOR) {
            if (successiveStatement) {
                context.pushSingle(Interpreter::OP_POP);
            }
            successiveStatement = true;
            std::unique_ptr<Statement> stmt = statement();
            stmt->emitByteCodes(context);
            if (tokenizer.current().type == FULLSTOP) {
                tokenizer.consume();
            } else {
//                SmallInteger lineOrError = tokenizer.currentLine();
                errors.emplace_back(Error(tokenizer.currentLine(), "Expected a '.' at the end of a statement."));
                //  while (tokenizer.current().lineNumber == lineOrError && !tokenizer.current().isEOI()) {
                //      tokenizer.consume();
                //  }
            }
        }

        context.pushCompound(Interpreter::OP_RETURN, Interpreter::OP_RETURN_STACK_TOP_TO_SENDER_INDEX);

        Methods methods(system.memoryManager(), system);
        return methods.createMethod(
                primitiveIndex >= 0 ? MethodHeader::forPrimitive((SmallInteger) primitiveIndex,
                                                                 context.getMaxTemporaries())
                                    : MethodHeader::forByteCodes(context.getMaxTemporaries()),
                type, system.symbolTable().lookup(selector),
                context.getLiterals(), context.getOpCodes());
    }

    void Compiler::compileMethodAndAdd(pimii::System& system) {
        ObjectPointer method = compileMethod(system);
        Methods methods(system.memoryManager(), system);
        methods.addMethod(type, system.symbolTable().lookup(selector), method);
    }

    void Compiler::parseSelector(EmitterContext& ctx) {
        if (tokenizer.current().type == OPERATOR) {
            selector += tokenizer.consume().value;
            if (tokenizer.current().type == NAME) {
                ctx.pushTemporary(tokenizer.consume().value);
            } else {
                SmallInteger lineOrError = tokenizer.currentLine();
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
                SmallInteger lineOrError = tokenizer.currentLine();
                errors.emplace_back(Error(tokenizer.currentLine(), ("Unexpected Token '" + tokenizer.current().value +
                                                                    "'. Expected an argument name.")));
                while (tokenizer.current().lineNumber == lineOrError && !tokenizer.current().isEOI()) {
                    tokenizer.consume();
                }
                return;
            }
        }
    }

    void Compiler::parseTemporaries(EmitterContext& ctx) {
        if (tokenizer.current().type != OPERATOR || tokenizer.current().value != "|") {
            return;
        }

        tokenizer.consume();
        while (tokenizer.current().type == NAME) {
            ctx.pushTemporary(tokenizer.consume().value);
        }

        if (tokenizer.current().type != OPERATOR || tokenizer.current().value != "|") {
            SmallInteger lineOrError = tokenizer.currentLine();
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
                if (tokenizer.current().value == "self" && tokenizer.next().type == FULLSTOP) {
                    tokenizer.consume();
                    return std::unique_ptr<Expression>(
                            new BuiltinConstant(Interpreter::OP_RETURN, Interpreter::OP_RETURN_RECEIVER_INDEX));
                }
                if (tokenizer.current().value == "true" && tokenizer.next().type == FULLSTOP) {
                    tokenizer.consume();
                    return std::unique_ptr<Expression>(
                            new BuiltinConstant(Interpreter::OP_RETURN, Interpreter::OP_RETURN_TRUE_INDEX));
                }
                if (tokenizer.current().value == "false" && tokenizer.next().type == FULLSTOP) {
                    tokenizer.consume();
                    return std::unique_ptr<Expression>(
                            new BuiltinConstant(Interpreter::OP_RETURN, Interpreter::OP_RETURN_FALSE_INDEX));
                }
                if (tokenizer.current().value == "nil" && tokenizer.next().type == FULLSTOP) {
                    tokenizer.consume();
                    return std::unique_ptr<Expression>(
                            new BuiltinConstant(Interpreter::OP_RETURN, Interpreter::OP_RETURN_NIL_INDEX));
                }
            }

            return std::unique_ptr<Expression>(new Return(expression()));
        }

        if (tokenizer.current().type == NAME && tokenizer.next().type == ASSIGNMENT) {
            std::unique_ptr<Assignment> assignment(new Assignment());
            assignment->name = tokenizer.consume().value;
            tokenizer.consume();
            assignment->expression = expression();
            return assignment;
        }

        return expression();
    }

    std::unique_ptr<Expression> Compiler::expression() {
        std::unique_ptr<Expression> currentReceiver = binaryCall(unaryCall(atom()));
        return selectorCall(std::move(currentReceiver));
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

        if (tokenizer.current().type == LITERAL_CHARACTER) {
            return std::unique_ptr<Expression>(new LiteralCharacter(tokenizer.consume().value));
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
        auto block = std::unique_ptr<Block>(new Block());
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
        while (tokenizer.current().type == NAME) {
            auto call = new MethodCall();
            call->receiver = std::move(receiver);
            call->selector = tokenizer.consume().value;
            receiver = std::unique_ptr<Expression>(call);
        }

        return receiver;
    }

    std::unique_ptr<Expression> Compiler::binaryCall(std::unique_ptr<Expression> receiver) {
        while (tokenizer.current().type == OPERATOR) {
            auto call = new MethodCall();
            call->receiver = std::move(receiver);
            call->selector = tokenizer.consume().value;
            call->arguments.emplace_back(unaryCall(atom()));
            receiver = std::unique_ptr<Expression>(call);
        }

        return receiver;
    }

    std::unique_ptr<Expression> Compiler::selectorCall(std::unique_ptr<Expression> receiver) {
        if (tokenizer.current().type != COLON_NAME) {
            return receiver;
        }

        auto call = new MethodCall();
        call->receiver = std::move(receiver);
        while (tokenizer.current().type == COLON_NAME) {
            call->selector += tokenizer.consume().value;
            call->arguments.emplace_back(binaryCall(unaryCall(atom())));
        }
        return std::unique_ptr<Expression>(call);
    }

    std::unique_ptr<Expression> Compiler::continuation() {
        return std::unique_ptr<Expression>();
    }


}
