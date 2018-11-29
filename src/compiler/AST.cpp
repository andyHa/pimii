//
// Created by Andreas Haufler on 28.11.18.
//

#include "AST.h"
#include "../vm/Interpreter.h"
#include "../vm/Strings.h"
#include "../vm/Primitives.h"

namespace pimii {

    void BuiltinConstant::emitByteCodes(EmitterContext &ctx) {
        ctx.pushSingle(opcode);
    }

    void Assignment::emitByteCodes(EmitterContext &ctx) {
        expression->emitByteCodes(ctx);
        //TODO receiver fields
        int offset = ctx.findTemporaryIndex(name);
        if (offset < 0) {
            //TODO error
            ctx.pushSingle(Interpreter::OP_PUSH_NIL);
        } else {
            ctx.pushWithIndex(Interpreter::OP_POP_AND_STORE_IN_TEMPORARY, offset);
        }
    }

    void PushGlobal::emitByteCodes(EmitterContext &ctx) {
        ObjectPointer symbol = ctx.getSystem().getSymbolTable().lookup(name);
        ObjectPointer association = ObjectPointer(ctx.getSystem().getSystemDictionary().at(symbol));
        Offset index = ctx.findOrAddLiteral(association);
        ctx.pushWithIndex(Interpreter::OP_PUSH_LITERAL_VARIABLE, index);
    }

    int EmitterContext::findTemporaryIndex(std::string &name) {
        for (auto i = (Offset) temporaries.size(); i-- > 0;) {
            if (name == temporaries[i]) {
                return i;
            }
        }

        return -1;
    }

    void EmitterContext::pushSingle(uint8_t opcode) {
        opcodes.push_back(opcode);
    }

    void EmitterContext::pushWithIndex(uint8_t opcode, int index) {
        if (index > 255 || index < 0) {
            //TODO error!
        }
        if (index < 0b111) {
            opcodes.push_back(opcode | (uint8_t) (index << 5));
        } else {
            opcodes.push_back(opcode | (uint8_t) 0b11100000);
            opcodes.push_back((uint8_t) index);
        }
    }

    Offset EmitterContext::addLiteral(ObjectPointer object) {
        auto index = (Offset) literals.size();
        literals.emplace_back(object);

        return index;
    }

    Offset EmitterContext::findOrAddLiteral(ObjectPointer object) {
        for (Offset index = 0; index < literals.size(); index++) {
            if (literals[index] == object) {
                return index;
            }
        }
        return addLiteral(object);
    }

    void EmitterContext::pushTemporaries(const std::vector<std::string> &temporariesToPush) {
        temporaries.insert(temporaries.begin(), temporariesToPush.begin(), temporariesToPush.end());
        maxTemporaries = std::max(maxTemporaries, (Offset) temporaries.size());
    }

    void EmitterContext::pushTemporary(const std::string &temporary) {
        temporaries.emplace_back(temporary);
        maxTemporaries = std::max(maxTemporaries, (Offset) temporaries.size());
    }

    void EmitterContext::popTemporaries(size_t numTemporaries) {
        temporaries.erase(temporaries.begin(), temporaries.end() - numTemporaries);
    }

    System &EmitterContext::getSystem() {
        return system;
    }

    Offset EmitterContext::getMaxTemporaries() {
        return maxTemporaries;
    }

    const std::vector<ObjectPointer> &EmitterContext::getLiterals() const {
        return literals;
    }

    const std::vector<uint8_t> &EmitterContext::getOpCodes() const {
        return opcodes;
    }

    void Block::emitByteCodes(EmitterContext &ctx) {
        ctx.pushTemporaries(temporaries);

        for (auto &statement : statements) {
            statement->emitByteCodes(ctx);
        }

        ctx.popTemporaries(temporaries.size());
    }

    void LiteralSymbol::emitByteCodes(EmitterContext &ctx) {
        ObjectPointer symbol = ctx.getSystem().getSymbolTable().lookup(name);
        Offset index = ctx.findOrAddLiteral(symbol);
        ctx.pushWithIndex(Interpreter::OP_PUSH_LITERAL_CONSTANT, index);
    }

    void LiteralString::emitByteCodes(EmitterContext &ctx) {
        ObjectPointer string = Strings::make(ctx.getSystem().getMemoryManager(),
                                             ctx.getSystem().getTypeSystem().stringType,
                                             name);
        Offset index = ctx.addLiteral(string);
        ctx.pushWithIndex(Interpreter::OP_PUSH_LITERAL_CONSTANT, index);
    }

    void LiteralNumber::emitByteCodes(EmitterContext &ctx) {
        if (number == 0) {
            ctx.pushSingle(Interpreter::OP_PUSH_ZERO);
            return;
        } else if (number == 1) {
            ctx.pushSingle(Interpreter::OP_PUSH_ONE);
            return;
        } else if (number == 2) {
            ctx.pushSingle(Interpreter::OP_PUSH_TWO);
            return;
        } else if (number == -1) {
            ctx.pushSingle(Interpreter::OP_PUSH_MINUS_ONE);
            return;
        }

        Offset index = ctx.findOrAddLiteral(ObjectPointer(number));
        ctx.pushWithIndex(Interpreter::OP_PUSH_LITERAL_CONSTANT, index);
    }

    void PushLocal::emitByteCodes(EmitterContext &ctx) {
        int offset = ctx.findTemporaryIndex(name);
        if (offset < 0) {
            //TODO error
            ctx.pushSingle(Interpreter::OP_PUSH_NIL);
        } else {
            ctx.pushWithIndex(Interpreter::OP_PUSH_TEMPORARY, offset);
        }
    }

    void MethodCall::emitByteCodes(EmitterContext &ctx) {
        receiver->emitByteCodes(ctx);
        for (auto &arg : arguments) {
            arg->emitByteCodes(ctx);
        }

        int specialSelectorIndex = ctx.getSystem().getSpecialSelectorIndex(selector);
        if (specialSelectorIndex >= 0) {
            if (arguments.empty()) {
                ctx.pushWithIndex(Interpreter::OP_SEND_SPECIAL_SELECTOR_WITH_NO_ARGS, specialSelectorIndex);
            } else if (arguments.size() == 1) {
                ctx.pushWithIndex(Interpreter::OP_SEND_SPECIAL_SELECTOR_WITH_ONE_ARG, specialSelectorIndex);
            } else if (arguments.size() == 2) {
                ctx.pushWithIndex(Interpreter::OP_SEND_SPECIAL_SELECTOR_WITH_TWO_ARGS, specialSelectorIndex);
            } else {
                ctx.pushWithIndex(Interpreter::OP_SEND_SPECIAL_SELECTOR_WITH_N_ARGS, (Offset) arguments.size());
            }

            return;
        }

        ObjectPointer symbol = ctx.getSystem().getSymbolTable().lookup(selector);
        Offset index = ctx.findOrAddLiteral(symbol);
        if (arguments.empty()) {
            ctx.pushWithIndex(Interpreter::OP_SEND_LITERAL_SELECTOR_WITH_NO_ARGS, index);
        } else if (arguments.size() == 1) {
            ctx.pushWithIndex(Interpreter::OP_SEND_LITERAL_SELECTOR_WITH_ONE_ARG, index);
        } else if (arguments.size() == 2) {
            ctx.pushWithIndex(Interpreter::OP_SEND_LITERAL_SELECTOR_WITH_TWO_ARGS, index);
        } else {
            ctx.pushWithIndex(Interpreter::OP_SEND_LITERAL_SELECTOR_WITH_N_ARGS, (Offset) arguments.size());
            ctx.pushSingle((uint8_t) index);
        }
    }

    void Return::emitByteCodes(EmitterContext &ctx) {
        expression->emitByteCodes(ctx);
        ctx.pushSingle(Interpreter::OP_RETURN_STACK_TOP_TO_SENDER);
    }
}
