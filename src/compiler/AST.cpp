//
// Created by Andreas Haufler on 28.11.18.
//

#include <iostream>
#include "AST.h"
#include "../vm/Interpreter.h"
#include "../vm/Primitives.h"

namespace pimii {

    void BuiltinConstant::emitByteCodes(EmitterContext& ctx) {
        ctx.pushCompound(opcode, compound);
    }

    void Assignment::emitByteCodes(EmitterContext& ctx) {
        expression->emitByteCodes(ctx);

        SmallInteger offset = ctx.findTemporaryIndex(name);
        if (offset >= 0) {
            ctx.pushSingle(Interpreter::OP_DUPLICAE_STACK_TOP);
            ctx.pushWithIndex(Interpreter::OP_POP_AND_STORE_IN_TEMPORARY, offset);
            return;
        }

        offset = ctx.findFieldIndex(name);
        if (offset >= 0) {
            ctx.pushSingle(Interpreter::OP_DUPLICAE_STACK_TOP);
            ctx.pushWithIndex(Interpreter::OP_POP_AND_STORE_RECEIVER_FIELD, offset);
            return;
        }

        if (isupper(name[0])) {
            auto symbol = ctx.getSystem().symbolTable().lookup(name);
            auto association = ObjectPointer(ctx.getSystem().systemDictionary().at(symbol));
            SmallInteger index = ctx.findOrAddLiteral(association);
            ctx.pushSingle(Interpreter::OP_DUPLICAE_STACK_TOP);
            ctx.pushWithIndex(Interpreter::OP_POP_AND_STORE_IN_LITERAL_VARIABLE, index);
        }

        //TODO error
    }

    void PushGlobal::emitByteCodes(EmitterContext& ctx) {
        auto symbol = ctx.getSystem().symbolTable().lookup(name);
        auto association = ObjectPointer(ctx.getSystem().systemDictionary().at(symbol));

        SmallInteger index = ctx.findOrAddLiteral(association);
        ctx.pushWithIndex(Interpreter::OP_PUSH_LITERAL_VARIABLE, index);
    }

    SmallInteger EmitterContext::findTemporaryIndex(const std::string& name) {
        for (auto i = (SmallInteger) temporaries.size(); i-- > 0;) {
            if (name == temporaries[i]) {
                return i;
            }
        }

        return -1;
    }

    SmallInteger EmitterContext::findFieldIndex(const std::string& name) {
        for (auto i = (SmallInteger) fields.size(); i-- > 0;) {
            if (name == fields[i]) {
                return i;
            }
        }

        return -1;
    }

    void EmitterContext::pushSingle(uint8_t opcode) {
        opcodes.push_back(opcode);
    }

    void EmitterContext::pushCompound(uint8_t opcode, int index) {
        opcodes.push_back(opcode | (uint8_t) (index << 5));
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

    SmallInteger EmitterContext::addLiteral(ObjectPointer object) {
        auto index = (SmallInteger) literals.size();
        literals.emplace_back(object);

        return index;
    }

    SmallInteger EmitterContext::findOrAddLiteral(ObjectPointer object) {
        for (SmallInteger index = 0; index < literals.size(); index++) {
            if (literals[index] == object) {
                return index;
            }
        }
        return addLiteral(object);
    }

    void EmitterContext::pushTemporaries(const std::vector<std::string>& temporariesToPush) {
        temporaries.insert(temporaries.end(), temporariesToPush.begin(), temporariesToPush.end());
        maxTemporaries = std::max(maxTemporaries, (SmallInteger) temporaries.size());
    }

    void EmitterContext::pushTemporary(const std::string& temporary) {
        temporaries.emplace_back(temporary);
        maxTemporaries = std::max(maxTemporaries, (SmallInteger) temporaries.size());
    }

    void EmitterContext::popTemporaries(size_t numTemporaries) {
        if (numTemporaries == 0) {
            return;
        }
        temporaries.erase(temporaries.end() - numTemporaries, temporaries.end());
    }

    System& EmitterContext::getSystem() {
        return system;
    }

    SmallInteger EmitterContext::getMaxTemporaries() {
        return maxTemporaries;
    }

    const std::vector<ObjectPointer>& EmitterContext::getLiterals() const {
        return literals;
    }

    const std::vector<uint8_t>& EmitterContext::getOpCodes() const {
        return opcodes;
    }

    SmallInteger EmitterContext::nextOpCodePosition() {
        return opcodes.size();
    }

    void EmitterContext::pushJump(uint8_t opcode, SmallInteger delta) {
        opcodes.emplace_back(opcode | (uint8_t) ((delta / 256) << 5));
        opcodes.emplace_back((uint8_t) (delta % 256));
    }

    void EmitterContext::insertJump(SmallInteger index, uint8_t opcode, SmallInteger delta) {
        opcodes[index] = opcode | (uint8_t) ((delta / 256) << 5);
        opcodes[index + 1] = (uint8_t) (delta % 256);
    }

    SmallInteger EmitterContext::pushJumpPlaceholder() {
        SmallInteger result = nextOpCodePosition();
        pushSingle(0);
        pushSingle(0);
        return result;
    }

    void Block::emitByteCodes(EmitterContext& ctx) {
        ctx.pushCompound(Interpreter::OP_BLOCK_COPY, temporaries.size());
        SmallInteger jmpAddr = ctx.pushJumpPlaceholder();
        emitInner(ctx);
        ctx.pushCompound(Interpreter::OP_RETURN, Interpreter::OP_RETURN_STACK_TO_TO_CALLER_INDEX);
        ctx.insertJump(jmpAddr, Interpreter::OP_JUMP_ALWAYS, ctx.nextOpCodePosition() - jmpAddr - 2);
    }

    void Block::emitInner(EmitterContext& context) {
        context.pushTemporaries(temporaries);

        for (auto i = 0; i < temporaries.size(); i++) {
            context.pushWithIndex(Interpreter::OP_POP_AND_STORE_IN_TEMPORARY,
                                  (SmallInteger) context.getTemporaries().size() - i - 1);
        }

        bool successive = false;
        for (auto& statement : statements) {
            if (successive) {
                context.pushSingle(Interpreter::OP_POP);
            }
            successive = true;
            statement->emitByteCodes(context);
        }

        context.popTemporaries(temporaries.size());
    }

    void LiteralSymbol::emitByteCodes(EmitterContext& ctx) {
        ObjectPointer symbol = ctx.getSystem().symbolTable().lookup(name);
        SmallInteger index = ctx.findOrAddLiteral(symbol);
        ctx.pushWithIndex(Interpreter::OP_PUSH_LITERAL_CONSTANT, index);
    }

    void LiteralString::emitByteCodes(EmitterContext& ctx) {
        ObjectPointer string = ctx.getSystem().memoryManager().makeString(name, ctx.getSystem().typeString());
        SmallInteger index = ctx.addLiteral(string);
        ctx.pushWithIndex(Interpreter::OP_PUSH_LITERAL_CONSTANT, index);
    }

    void LiteralCharacter::emitByteCodes(EmitterContext& ctx) {
        int ch;
        if (name.length() > 1) {
            ch = (int)stoi(name);
        } else {
            ch =  name[0];
        }
        ObjectPointer character = ctx.getSystem().typeCharacter()[System::TYPE_SIZE][ch];
        SmallInteger index = ctx.addLiteral(character);
        ctx.pushWithIndex(Interpreter::OP_PUSH_LITERAL_CONSTANT, index);
    }

    void LiteralNumber::emitByteCodes(EmitterContext& ctx) {
        if (number == 0) {
            ctx.pushCompound(Interpreter::OP_PUSH, Interpreter::OP_PUSH_ZERO_INDEX);
            return;
        } else if (number == 1) {
            ctx.pushCompound(Interpreter::OP_PUSH, Interpreter::OP_PUSH_ONE_INDEX);
            return;
        } else if (number == 2) {
            ctx.pushCompound(Interpreter::OP_PUSH, Interpreter::OP_PUSH_TWO_INDEX);
            return;
        } else if (number == -1) {
            ctx.pushCompound(Interpreter::OP_PUSH, Interpreter::OP_PUSH_MINUS_ONE_INDEX);
            return;
        }

        SmallInteger index = ctx.findOrAddLiteral(ObjectPointer::forSmallInt(number));
        ctx.pushWithIndex(Interpreter::OP_PUSH_LITERAL_CONSTANT, index);
    }

    void PushLocal::emitByteCodes(EmitterContext& ctx) {
        int offset = ctx.findTemporaryIndex(name);
        if (offset >= 0) {
            ctx.pushWithIndex(Interpreter::OP_PUSH_TEMPORARY, offset);
            return;
        }
        offset = ctx.findFieldIndex(name);
        if (offset >= 0) {
            ctx.pushWithIndex(Interpreter::OP_PUSH_RECEIVER_FIELD, offset);
            return;
        }
        //TODO error
        ctx.pushCompound(Interpreter::OP_PUSH, Interpreter::OP_PUSH_NIL_INDEX);

    }

    void MethodCall::emitByteCodes(EmitterContext& ctx) {
        if (emitOptimizedControlFlow(ctx)) {
            return;
        }

        receiver->emitByteCodes(ctx);
        for (auto& arg : arguments) {
            arg->emitByteCodes(ctx);
        }

        SmallInteger specialSelectorIndex = ctx.getSystem().specialSelectorIndex(selector);
        if (specialSelectorIndex >= 0) {
            if (arguments.empty()) {
                ctx.pushWithIndex(Interpreter::OP_SEND_SPECIAL_SELECTOR_WITH_NO_ARGS, specialSelectorIndex);
            } else if (arguments.size() == 1) {
                ctx.pushWithIndex(Interpreter::OP_SEND_SPECIAL_SELECTOR_WITH_ONE_ARG, specialSelectorIndex);
            } else if (arguments.size() == 2) {
                ctx.pushWithIndex(Interpreter::OP_SEND_SPECIAL_SELECTOR_WITH_TWO_ARGS, specialSelectorIndex);
            } else {
                ctx.pushWithIndex(Interpreter::OP_SEND_SPECIAL_SELECTOR_WITH_N_ARGS, (SmallInteger) arguments.size());
                ctx.pushSingle((uint8_t) specialSelectorIndex);
            }

            return;
        }

        ObjectPointer symbol = ctx.getSystem().symbolTable().lookup(selector);
        SmallInteger index = ctx.findOrAddLiteral(symbol);
        if (arguments.empty()) {
            ctx.pushWithIndex(Interpreter::OP_SEND_LITERAL_SELECTOR_WITH_NO_ARGS, index);
        } else if (arguments.size() == 1) {
            ctx.pushWithIndex(Interpreter::OP_SEND_LITERAL_SELECTOR_WITH_ONE_ARG, index);
        } else if (arguments.size() == 2) {
            ctx.pushWithIndex(Interpreter::OP_SEND_LITERAL_SELECTOR_WITH_TWO_ARGS, index);
        } else {
            ctx.pushWithIndex(Interpreter::OP_SEND_LITERAL_SELECTOR_WITH_N_ARGS, (SmallInteger) arguments.size());
            ctx.pushSingle((uint8_t) index);
        }
    }

    bool MethodCall::emitOptimizedControlFlow(EmitterContext& ctx) {
        if (selector == "whileTrue:" && arguments.size() == 1 && receiver->type() == STMT_BLOCK &&
            arguments[0]->type() == STMT_BLOCK) {
            SmallInteger loopAddress = ctx.nextOpCodePosition();
            reinterpret_cast<Block*>(receiver.get())->emitInner(ctx);
            SmallInteger jumpOnFalseLocation = ctx.pushJumpPlaceholder();
            reinterpret_cast<Block*>(arguments[0].get())->emitInner(ctx);
            SmallInteger delta = ctx.nextOpCodePosition() - loopAddress;
            ctx.pushJump(Interpreter::OP_JUMP_BACK, delta + 2);
            ctx.insertJump(jumpOnFalseLocation, Interpreter::OP_JUMP_ON_FALSE,
                           ctx.nextOpCodePosition() - jumpOnFalseLocation - 2);
            return true;
        }
        return false;
    }

    void Return::emitByteCodes(EmitterContext& ctx) {
        expression->emitByteCodes(ctx);
        ctx.pushCompound(Interpreter::OP_RETURN, Interpreter::OP_RETURN_STACK_TOP_TO_SENDER_INDEX);
    }
}
