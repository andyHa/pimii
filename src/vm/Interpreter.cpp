//
// Created by Andreas Haufler on 23.11.18.
//

#include <array>
#include <iostream>
#include "Interpreter.h"
#include "Primitives.h"
#include "Nil.h"

namespace pimii {

    const Offset Interpreter::CONTEXT_FIXED_SIZE = 6;
    const Offset Interpreter::CONTEXT_SENDER_FIELD = 0;
    const Offset Interpreter::CONTEXT_CALLER_FIELD = 0;
    const Offset Interpreter::CONTEXT_IP_FIELD = 1;
    const Offset Interpreter::CONTEXT_SP_FIELD = 2;
    const Offset Interpreter::CONTEXT_METHOD_FIELD = 3;
    const Offset Interpreter::CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD = 3;
    const Offset Interpreter::CONTEXT_INITIAL_IP_FIELD = 4;
    const Offset Interpreter::CONTEXT_HOME_FIELD = 5;
    const Offset Interpreter::CONTEXT_RECEIVER_FIELD = 5;

    const Offset Interpreter::COMPILED_METHOD_SIZE = 2;
    const Offset Interpreter::COMPILED_METHOD_FIELD_HEADER = 0;
    const Offset Interpreter::COMPILED_METHOD_FIELD_OPCODES = 1;
    const Offset Interpreter::COMPILED_METHOD_FIELD_LITERALS_START = 2;
    const Offset Interpreter::COMPILED_METHOD_TYPE_FIELD_SPECIAL_SELECTORS = 6;


    Interpreter::Interpreter(System &system) : system(system) {
        // Install by emulating: "CompiledMethod class specialSelectors: <array>"
        system.getTypeSystem().compiledMethodType.getObject()->fields[Interpreter::COMPILED_METHOD_TYPE_FIELD_SPECIAL_SELECTORS] = system.getSpecialSelectors();
    }

    void Interpreter::run() {
        while (activeContext != nullptr) {
            dispatchOpCode(fetchInstruction());
        }
    }


    void Interpreter::dispatchOpCode(uint8_t opCode) {
        uint8_t code = opCode & (uint8_t) 0b11111;
        uint8_t index = opCode >> 5;

        switch (code) {
            case OP_RETURN:
                switch (index) {
                    case OP_RETURN_RECEIVER_INDEX:
                        returnValueTo(receiver, sender());
                        return;
                    case OP_RETURN_TRUE_INDEX:
                        returnValueTo(system.trueValue, sender());
                        return;
                    case OP_RETURN_FALSE_INDEX:
                        returnValueTo(system.falseValue, sender());
                        return;
                    case OP_RETURN_NIL_INDEX:
                        returnValueTo(Nil::NIL, sender());
                        return;
                    case OP_RETURN_STACK_TOP_TO_SENDER_INDEX:
                        returnValueTo(pop(), sender());
                        return;
                    case OP_RETURN_STACK_TO_TO_CALLER_INDEX:
                        returnValueTo(pop(), caller());
                        return;
                }
                //TODO
                return;
            case OP_PUSH:
                switch (index) {
                    case OP_PUSH_RECEIVER_INDEX:
                        push(receiver);
                        return;
                    case OP_PUSH_TRUE_INDEX:
                        push(system.trueValue);
                        return;
                    case OP_PUSH_FALSE_INDEX:
                        push(system.falseValue);
                        return;
                    case OP_PUSH_NIL_INDEX:
                        push(Nil::NIL);
                        return;
                    case OP_PUSH_MINUS_ONE_INDEX:
                        push(ObjectPointer(-1));
                        return;
                    case OP_PUSH_ZERO_INDEX:
                        push(ObjectPointer(0));
                        return;
                    case OP_PUSH_ONE_INDEX:
                        push(ObjectPointer(1));
                        return;
                    case OP_PUSH_TWO_INDEX:
                        push(ObjectPointer(2));
                        return;
                }
                //TODO
                return;
            case OP_JUMP_BACK:
            case OP_JUMP_ALWAYS:
            case OP_JUMP_ON_TRUE:
            case OP_JUMP_ON_FALSE:
                handleJump(opCode, index);
                return;
        }

        if (index == 0b111) {
            index = fetchInstruction();
        }

        switch (code) {
            case OP_PUSH_LITERAL_CONSTANT:
                push(literal(index));
                return;
            case OP_PUSH_LITERAL_VARIABLE:
                push(literal(index).getObject()->fields[SystemDictionary::ASSOCIATION_FIELD_VALUE]);
                return;
            case OP_PUSH_TEMPORARY:
                push(temporary(index));
                return;
            case OP_PUSH_RECEIVER_FIELD:
                push(receiver.getObject()->fields[index]);
                return;
            case OP_POP_AND_STORE_RECEIVER_FIELD:
                receiver.getObject()->fields[index] = pop();
                return;
            case OP_POP_AND_STORE_IN_TEMPORARY:
                temporary(index, pop());
                return;
            case OP_POP_AND_STORE_IN_LITERAL_VARIABLE:
                literal(index).getObject()->fields[SystemDictionary::ASSOCIATION_FIELD_VALUE] = pop();
                return;
            case OP_SEND_LITERAL_SELECTOR_WITH_NO_ARGS:
                send(literal(index), 0);
                return;
            case OP_SEND_LITERAL_SELECTOR_WITH_ONE_ARG:
                send(literal(index), 1);
                return;
            case OP_SEND_LITERAL_SELECTOR_WITH_TWO_ARGS:
                send(literal(index), 2);
                return;
            case OP_SEND_LITERAL_SELECTOR_WITH_N_ARGS:
                send(literal(fetchInstruction()), index);
                return;
            case OP_SEND_SPECIAL_SELECTOR_WITH_NO_ARGS:
                if (index > LAST_PREFERRED_PRIMITIVE_SELECTOR || !executePrimitive(index, 0)) {
                    send(system.getSpecialSelector(index), 0);
                }
                return;
            case OP_SEND_SPECIAL_SELECTOR_WITH_ONE_ARG:
                if (index > LAST_PREFERRED_PRIMITIVE_SELECTOR || !executePrimitive(index, 1)) {
                    send(system.getSpecialSelector(index), 1);
                }
                return;
            case OP_SEND_SPECIAL_SELECTOR_WITH_TWO_ARGS:
                if (index > LAST_PREFERRED_PRIMITIVE_SELECTOR || !executePrimitive(index, 2)) {
                    send(system.getSpecialSelector(index), 2);
                }
                return;
            case OP_SEND_SPECIAL_SELECTOR_WITH_N_ARGS:
                Offset primitiveIndex = fetchInstruction();
                if (primitiveIndex > LAST_PREFERRED_PRIMITIVE_SELECTOR || !executePrimitive(primitiveIndex, index)) {
                    send(system.getSpecialSelector(index), index);
                }
                return;
        }
    }

    void Interpreter::newActiveContext(Object *context) {
        if (activeContext != nullptr) {
            storeContextRegisters();
        }

        activeContext = context;

        if (activeContext != nullptr) {
            fetchContextRegisters();
        }
    }

    void Interpreter::storeContextRegisters() {
        activeContext->fields[CONTEXT_IP_FIELD] = ObjectPointer(instructionPointer);
        activeContext->fields[CONTEXT_SP_FIELD] = ObjectPointer(stackPointer);
    }

    void Interpreter::fetchContextRegisters() {
        if (isBlockContext(activeContext)) {
            homeContext = activeContext->fields[CONTEXT_HOME_FIELD].getObject();
            temporaryCount = 0;
        } else {
            homeContext = activeContext;
            getMethodType(homeContext->fields[CONTEXT_METHOD_FIELD], temporaryCount);
        }

        receiver = homeContext->fields[CONTEXT_RECEIVER_FIELD];
        method = homeContext->fields[CONTEXT_METHOD_FIELD].getObject();
        opCodes = method->fields[COMPILED_METHOD_FIELD_OPCODES].getBytes();
        maxIP = opCodes->size * sizeof(Word) - opCodes->odd;
        instructionPointer = (Offset) activeContext->fields[CONTEXT_IP_FIELD].getInt();
        stackPointer = (Offset) activeContext->fields[CONTEXT_SP_FIELD].getInt();;
    }

    uint8_t Interpreter::fetchInstruction() {
        if (instructionPointer >= maxIP) {
            return OP_RETURN;
        }
        return (uint8_t) opCodes->bytes[instructionPointer++];
    }

    void Interpreter::push(ObjectPointer value) {
        //TODO stack limits!
        activeContext->fields[getStackBasePointer() + (stackPointer++)] = value;
    }

    ObjectPointer Interpreter::pop() {
        if (stackPointer == 0) {
            return Nil::NIL; //TODO error?
        }
        return activeContext->fields[getStackBasePointer() + (--stackPointer)];
    }

    ObjectPointer Interpreter::stackTop() {
        if (stackPointer == 0) {
            return Nil::NIL; //TODO error?
        }
        return activeContext->fields[getStackBasePointer() + (stackPointer - 1)];
    }

    ObjectPointer Interpreter::stackValue(Offset offset) {
        Offset effectiveStackPointer = stackPointer - offset;
        if (effectiveStackPointer <= 0) {
            return Nil::NIL; //TODO error?
        }
        return activeContext->fields[getStackBasePointer() + (effectiveStackPointer - 1)];
    }

    void Interpreter::pop(Offset number) {
        if (stackPointer >= number) {
            stackPointer -= number;
        } else {
            //TODO error?
            stackPointer = 0;
        }
    }

    void Interpreter::unPop(Offset number) {
        //TODO limit1!
        stackPointer += number;
    }

    ObjectPointer Interpreter::sender() {
        return homeContext->fields[CONTEXT_SENDER_FIELD];
    }

    ObjectPointer Interpreter::caller() {
        return activeContext->fields[CONTEXT_SENDER_FIELD];
    }

    ObjectPointer Interpreter::temporary(Offset index) {
        //TODO limits
        return homeContext->fields[CONTEXT_FIXED_SIZE + index];
    }

    void Interpreter::temporary(Offset index, ObjectPointer value) {
        homeContext->fields[CONTEXT_FIXED_SIZE + index] = value;
    }

    ObjectPointer Interpreter::literal(Offset index) {
        return method->fields[COMPILED_METHOD_FIELD_LITERALS_START + index];
    }

    void Interpreter::returnValueTo(ObjectPointer returnValue, ObjectPointer targetContext) {
        newActiveContext(targetContext.getObject());
        std::cout << returnValue.getInt() << std::endl;
        if (activeContext != nullptr) {
            push(returnValue);
        }
    }

    ObjectPointer Interpreter::findMethod(ObjectPointer type, ObjectPointer selector) {
        lookup:
        if (type == Nil::NIL ||
            type.getObject()->fields[TypeSystem::TYPE_FIELD_SELECTORS] == Nil::NIL) {
            return Nil::NIL;
        }

        Object *typeObject = type.getObject();

        Object *selectors = typeObject->fields[TypeSystem::TYPE_FIELD_SELECTORS].getObject();
        Offset index = selector.hash() % selectors->size;
        for (Offset i = index; i < selectors->size; i++) {
            if (selectors->fields[i] == selector) {
                return typeObject->fields[TypeSystem::TYPE_FIELD_METHODS].getObject()->fields[i];
            } else if (selectors->fields[i] == Nil::NIL) {
                type = typeObject->fields[TypeSystem::TYPE_FIELD_SUPERTYPE];
                goto lookup;
            }
        }
        for (size_t i = 0; i < index; i++) {
            if (selectors->fields[i] == selector) {
                return typeObject->fields[TypeSystem::TYPE_FIELD_METHODS].getObject()->fields[i];
            } else if (selectors->fields[i] == Nil::NIL) {
                type = typeObject->fields[TypeSystem::TYPE_FIELD_SUPERTYPE];
                goto lookup;
            }
        }

        type = typeObject->fields[TypeSystem::TYPE_FIELD_SUPERTYPE];
        goto lookup;
    }

    CompiledMethodType Interpreter::getMethodType(ObjectPointer method, Offset &offset) {
        auto header = (Offset) method.getObject()->fields[COMPILED_METHOD_FIELD_HEADER].getInt();
        offset = header >> 2;
        return (CompiledMethodType) (header & 0b11);
    }


    void Interpreter::send(ObjectPointer selector, Offset numArguments) {
        ObjectPointer newReceiver = stackValue(numArguments);
        ObjectPointer type = system.getType(newReceiver);
        ObjectPointer newMethod = findMethod(type, selector);

        Offset index = 0;
        CompiledMethodType methodType = getMethodType(newMethod, index);
        if (methodType == CompiledMethodType::PRIMITIVE) {
            if (executePrimitive(index, numArguments)) {
                return;
            }
        } else if (methodType == CompiledMethodType::RETURN_FIELD) {
            //TODO range check
            pop(); //assert numArguments == 0
            push(newReceiver.getObject()->fields[index]);
            return;
        } else if (methodType == CompiledMethodType::POP_AND_STORE_FIELD) {
            //TODO range check
            //pop(); //assert numArguments == 1
            newReceiver.getObject()->fields[index] = pop();
            //push(newReceiver);
            return;
        }

        // TODO check if method is non-nil and has bytecodes

//        //TODO
//        Context *newContext = (Context *) memory.allocHeap(NIL, Context::FIXED_SIZE + 8);
//        newContext->atPut(Context::SENDER_FIELD, activeContext);
//        newContext->atPut(Context::IP_FIELD, 0);
//        newContext->atPut(Context::SP_FIELD, compiledMethod->getTemporaryCount());
//        newContext->atPut(Context::METHOD_FIELD, newMethod);
//
//        for (size_t i = 0; i < numArguments + 1; i++) {
//            newContext->atPut(Context::RECEIVER_FIELD + i, stackTop(numArguments - i));
//        }
//
//        pop(numArguments + 1);
//
//        newActiveContext(newContext);
    }

    ObjectPointer Interpreter::getReceiver() {
        return receiver;
    }

    bool Interpreter::executePrimitive(Offset index, Offset numberOfArguments) {
        return Primitives::executePrimitive(index, *this, numberOfArguments);
    }

    bool Interpreter::isBlockContext(Object *context) {
        return context->fields[CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD].getObjectPointerType() ==
               ObjectPointerType::SMALL_INT;
    }

    System &Interpreter::getSystem() {
        return system;
    }

    Offset Interpreter::getInstructionPointer() {
        return instructionPointer;
    }

    Object *Interpreter::getActiveContext() {
        return activeContext;
    }

    void Interpreter::transfer(Offset numberOfFields, Object *src, Offset srcIndex, Object *dst, Offset destIndex) {
        for (Offset idx = 0; idx < numberOfFields; idx++) {
            dst->fields[destIndex + idx] = src->fields[srcIndex + idx];
        }
    }

    Offset Interpreter::getStackPointer() {
        return stackPointer;
    }

    Offset Interpreter::getStackBasePointer() {
        return CONTEXT_FIXED_SIZE + temporaryCount;
    }

    void Interpreter::handleJump(uint8_t code, uint8_t index) {
        int delta = index * 255 + fetchInstruction();

        switch (code) {
            case OP_JUMP_BACK:
                if (delta > instructionPointer) {
                    instructionPointer = 0;
                } else {
                    instructionPointer = instructionPointer - delta;
                }
                return;
            case OP_JUMP_ALWAYS:
                instructionPointer = instructionPointer + delta;
                return;
            case OP_JUMP_ON_TRUE:
                if (pop() == system.trueValue) {
                    instructionPointer = instructionPointer + delta;
                }
                return;
            case OP_JUMP_ON_FALSE:
                if (pop() == system.falseValue) {
                    instructionPointer = instructionPointer + delta;
                }
                return;
        }
    }


}