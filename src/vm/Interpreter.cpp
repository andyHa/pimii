//
// Created by Andreas Haufler on 23.11.18.
//

#include <array>
#include <iostream>
#include "Interpreter.h"
#include "Primitives.h"
#include "Looping.h"
#include "Methods.h"

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


    Interpreter::Interpreter(System& system) : system(system), contextSwitchExpected(false) {
        // Install by emulating: "CompiledMethod class specialSelectors: <array>"
        system.getTypeSystem().compiledMethodType[Interpreter::COMPILED_METHOD_TYPE_FIELD_SPECIAL_SELECTORS] = system.getSpecialSelectors();

    }

    void Interpreter::run(ObjectPointer rootContext) {
        rootProcess = system.getMemoryManager().makeObject(System::PROCESS_SIZE,
                                                           system.getTypeSystem().processType);
        rootProcess[System::PROCESS_FIELD_CONTEXT] = rootContext;

        pushBack(rootProcess, system.processor, System::PROCESSOR_FIELD_FIRST_WAITING_PROCESS,
                 System::PROCESSOR_FIELD_LAST_WAITING_PROCESS);
        contextSwitchExpected = true;

        while (true) { //TODO for rootProcess done
            if (!contextSwitchExpected) {
                if (system.hasIRQ()) {
                    if (system.isIRQ(IRQ_TIMER)) {
                        ObjectPointer irqTable = system.processor[System::PROCESSOR_FIELD_IRQ_TABLE];
                        if (irqTable != Nil::NIL) {
                            ObjectPointer semaphore = irqTable[0];
                            if (semaphore != Nil::NIL) {
                                signalSemaphore(semaphore);
                            }
                        }
                    }
                    system.clearIRQ();
                }
            }

            if (contextSwitchExpected) {
                ObjectPointer activeProcess = system.processor[System::PROCESSOR_FIELD_ACTIVE_PROCESS];

                ObjectPointer nextProcess = popFront(system.processor, System::PROCESSOR_FIELD_FIRST_WAITING_PROCESS,
                                                     System::PROCESSOR_FIELD_LAST_WAITING_PROCESS);
                while (nextProcess == Nil::NIL) {
                    std::unique_lock<std::mutex> l(system.work_lock);
                    system.work_available.wait(l);
                    if (system.hasIRQ()) {
                        if (system.isIRQ(IRQ_TIMER)) {
                            ObjectPointer irqTable = system.processor[System::PROCESSOR_FIELD_IRQ_TABLE];
                            if (irqTable != Nil::NIL) {
                                ObjectPointer semaphore = irqTable[0];
                                if (semaphore != Nil::NIL) {
                                    signalSemaphore(semaphore);
                                }
                            }
                        }
                        system.clearIRQ();
                    }
                    nextProcess = popFront(system.processor, System::PROCESSOR_FIELD_FIRST_WAITING_PROCESS,
                                           System::PROCESSOR_FIELD_LAST_WAITING_PROCESS);
                }
                if (nextProcess == Nil::NIL) {
                    return; //TODO idle
                }

                contextSwitchExpected = false;
                std::cout << "NEXT PROCESS " << nextProcess.hash() << std::endl;

                if (activeProcess != Nil::NIL) {
                    activeProcess[System::PROCESS_FIELD_CONTEXT] = activeContext;
                }
                system.processor[System::PROCESSOR_FIELD_ACTIVE_PROCESS] = nextProcess;

                newActiveContext(nextProcess[System::PROCESS_FIELD_CONTEXT]);
            }

            uint8_t opcode = fetchInstruction();
            dispatchOpCode(opcode);
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
                        push(ObjectPointer::forSmallInt(-1));
                        return;
                    case OP_PUSH_ZERO_INDEX:
                        push(ObjectPointer::forSmallInt(0));
                        return;
                    case OP_PUSH_ONE_INDEX:
                        push(ObjectPointer::forSmallInt(1));
                        return;
                    case OP_PUSH_TWO_INDEX:
                        push(ObjectPointer::forSmallInt(2));
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
            case OP_BLOCK_COPY:
                performBlockCopy(index);
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
                push(literal(index)[SystemDictionary::ASSOCIATION_FIELD_VALUE]);
                return;
            case OP_PUSH_TEMPORARY:
                push(temporary(index));
                return;
            case OP_PUSH_RECEIVER_FIELD:
                push(receiver[index]);
                return;
            case OP_POP_AND_STORE_RECEIVER_FIELD:
                receiver[index] = pop();
                return;
            case OP_POP_AND_STORE_IN_TEMPORARY:
                temporary(index, pop());
                return;
            case OP_POP_AND_STORE_IN_LITERAL_VARIABLE:
                literal(index)[SystemDictionary::ASSOCIATION_FIELD_VALUE] = pop();
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

    void Interpreter::newActiveContext(ObjectPointer context) {
        if (activeContext != Nil::NIL) {
            storeContextRegisters();
        }

        activeContext = context;

        if (activeContext != Nil::NIL) {
            fetchContextRegisters();
        }
    }

    void Interpreter::storeContextRegisters() {
        activeContext[CONTEXT_IP_FIELD] = instructionPointer;
        activeContext[CONTEXT_SP_FIELD] = stackPointer;
    }

    void Interpreter::fetchContextRegisters() {
        if (isBlockContext(activeContext)) {
            homeContext = activeContext[CONTEXT_HOME_FIELD];
            temporaryCount = 0;
        } else {
            homeContext = activeContext;
            temporaryCount = MethodHeader(
                    homeContext[CONTEXT_METHOD_FIELD][COMPILED_METHOD_FIELD_HEADER].smallInt()).temporaries();
        }

        receiver = homeContext[CONTEXT_RECEIVER_FIELD];
        method = homeContext[CONTEXT_METHOD_FIELD];
        opCodes = method[COMPILED_METHOD_FIELD_OPCODES];
        maxIP = opCodes.byteSize();
        instructionPointer = (Offset) activeContext[CONTEXT_IP_FIELD].smallInt();
        stackPointer = (Offset) activeContext[CONTEXT_SP_FIELD].smallInt();
    }

    uint8_t Interpreter::fetchInstruction() {
        if (instructionPointer >= maxIP) {
            return OP_RETURN;
        }
        return (uint8_t) opCodes.fetchByte(instructionPointer++);
    }

    void Interpreter::push(ObjectPointer value) {
        //TODO stack limits!
        activeContext[getStackBasePointer() + (stackPointer++)] = value;
    }

    ObjectPointer Interpreter::pop() {
        if (stackPointer == 0) {
            return Nil::NIL; //TODO error?
        }
        return activeContext[getStackBasePointer() + (--stackPointer)];
    }

    ObjectPointer Interpreter::stackTop() {
        if (stackPointer == 0) {
            return Nil::NIL; //TODO error?
        }
        return activeContext[getStackBasePointer() + (stackPointer - 1)];
    }

    ObjectPointer Interpreter::stackValue(Offset offset) {
        Offset effectiveStackPointer = stackPointer - offset;
        if (effectiveStackPointer <= 0) {
            return Nil::NIL; //TODO error?
        }
        return activeContext[getStackBasePointer() + (effectiveStackPointer - 1)];
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
        return homeContext[CONTEXT_SENDER_FIELD];
    }

    ObjectPointer Interpreter::caller() {
        return activeContext[CONTEXT_SENDER_FIELD];
    }

    ObjectPointer Interpreter::temporary(Offset index) {
        //TODO limits
        std::cout << "Temporary " << index << " of context " << homeContext.hash()
                  << " is " << homeContext[CONTEXT_FIXED_SIZE + index].hash() << " Type: "
                  << homeContext[CONTEXT_FIXED_SIZE + index].type().hash() << std::endl;
        return homeContext[CONTEXT_FIXED_SIZE + index];
    }

    void Interpreter::temporary(Offset index, ObjectPointer value) {
        homeContext[CONTEXT_FIXED_SIZE + index] = value;
    }

    ObjectPointer Interpreter::literal(Offset index) {
        return method[COMPILED_METHOD_FIELD_LITERALS_START + index];
    }

    void Interpreter::returnValueTo(ObjectPointer returnValue, ObjectPointer targetContext) {
        newActiveContext(targetContext);
        if (activeContext != Nil::NIL) {
            push(returnValue);
        } else {
            contextSwitchExpected = true;
        }
    }

    ObjectPointer Interpreter::findMethod(ObjectPointer type, ObjectPointer selector) {
        while (type != Nil::NIL) {

            std::cout << "Finding " << selector.stringView() << " in " << type[TypeSystem::TYPE_FIELD_NAME].stringView()
                      << std::endl;

            if (type[TypeSystem::TYPE_FIELD_SELECTORS] != Nil::NIL) {
                ObjectPointer method = findMethodInType(type, selector);
                if (method != Nil::NIL) {
                    return method;
                }
            }
            type = type[TypeSystem::TYPE_FIELD_SUPERTYPE];
        }


        return Nil::NIL;
    }

    ObjectPointer Interpreter::findMethodInType(ObjectPointer type, ObjectPointer selector) {
        ObjectPointer selectors = type[TypeSystem::TYPE_FIELD_SELECTORS];
        for (Looping loop = Looping(selectors.size(), selector.hash()); loop.hasNext(); loop.next()) {
            if (selectors[loop()] == selector) {
                return type[TypeSystem::TYPE_FIELD_METHODS][loop()];
            } else if (selectors[loop()] == Nil::NIL) {
                return Nil::NIL;
            }
        }

        return Nil::NIL;
    }

    void Interpreter::send(ObjectPointer selector, Offset numArguments) {
        ObjectPointer newReceiver = stackValue(numArguments);
        ObjectPointer type = system.getType(newReceiver);
        std::cout << "SENDING " << newReceiver.hash() << " " << selector.stringView() << std::endl;
        ObjectPointer newMethod = findMethod(type, selector);

        MethodHeader header(newMethod[COMPILED_METHOD_FIELD_HEADER].smallInt());

        if (header.methodType() == CompiledMethodType::MT_PRIMITIVE) {
            if (executePrimitive(header.primitiveIndex(), numArguments)) {
                return;
            }
        } else if (header.methodType() == CompiledMethodType::MT_RETURN_FIELD) {
            //TODO range check
            pop(); //assert numArguments == 0
            push(newReceiver[header.fieldIndex()]);
            return;
        } else if (header.methodType() == CompiledMethodType::MT_POP_AND_STORE_FIELD) {
            //TODO range check
            //pop(); //assert numArguments == 1
            newReceiver[header.fieldIndex()] = pop();
            //push(newReceiver);
            return;
        }

        std::cout << "Sending " << selector.stringView() << std::endl;

        // TODO check if method is non-nil and has bytecodes
        ObjectPointer newContext = system.getMemoryManager().makeObject(
                CONTEXT_FIXED_SIZE + (header.largeContextFlag() ? 16 : 8), system.getTypeSystem().methodContextType);
        newContext[CONTEXT_SENDER_FIELD] = activeContext;
        newContext[CONTEXT_IP_FIELD] = 0;
        newContext[CONTEXT_SP_FIELD] = header.temporaries();
        newContext[CONTEXT_METHOD_FIELD] = newMethod;
        newContext[CONTEXT_RECEIVER_FIELD] = newReceiver;

        //TODO ensure proper stack limits
        //TODO ensure numArguments <= numTeporaries
        if (numArguments > 0) {
            activeContext.transferFieldsTo(getStackBasePointer() + (stackPointer - 1 - numArguments), newContext,
                                           CONTEXT_FIXED_SIZE, numArguments);
        }

        pop(numArguments + 1);

        newActiveContext(newContext);
    }

    ObjectPointer Interpreter::getReceiver() {
        return receiver;
    }

    bool Interpreter::executePrimitive(Offset index, Offset numberOfArguments) {
        return Primitives::executePrimitive(index, *this, numberOfArguments);
    }

    bool Interpreter::isBlockContext(ObjectPointer context) {
        return context[CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD].isSmallInt();
    }

    System& Interpreter::getSystem() {
        return system;
    }

    Offset Interpreter::getInstructionPointer() {
        return instructionPointer;
    }

    ObjectPointer Interpreter::getActiveContext() {
        return activeContext;
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
                pop();
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
                if (stackTop() == system.trueValue) {
                    pop();
                    instructionPointer = instructionPointer + delta;
                }
                return;
            case OP_JUMP_ON_FALSE:
                if (stackTop() == system.falseValue) {
                    pop();
                    instructionPointer = instructionPointer + delta;
                }
                return;
        }
    }

    void Interpreter::performBlockCopy(uint8_t blockArgumentCount) {
        ObjectPointer newContext = system.getMemoryManager().makeObject(
                activeContext.size(), system.getTypeSystem().blockContextType);

        newContext[Interpreter::CONTEXT_INITIAL_IP_FIELD] =
                instructionPointer + 2;
        newContext[Interpreter::CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD] = blockArgumentCount;
        newContext[Interpreter::CONTEXT_HOME_FIELD] = homeContext;
        push(ObjectPointer(newContext));
    }

    void Interpreter::signalSemaphore(ObjectPointer semaphore) {
        ObjectPointer firstWaitingProcess = popFront(semaphore, System::SEMAPHORE_FIELD_FIRST_WAITING_PROCESS,
                                                     System::SEMAPHORE_FIELD_LAST_WAITING_PROCESS);

        if (firstWaitingProcess == Nil::NIL) {
            semaphore[System::SEMAPHORE_FIELD_EXCESS_SIGNALS] =
                    semaphore[System::SEMAPHORE_FIELD_EXCESS_SIGNALS].smallInt() + 1;
            return;
        }

        pushFront(firstWaitingProcess, system.processor, System::PROCESSOR_FIELD_FIRST_WAITING_PROCESS,
                  System::PROCESSOR_FIELD_LAST_WAITING_PROCESS);
        contextSwitchExpected = true;
    }

    ObjectPointer Interpreter::popFront(ObjectPointer list, Offset first, Offset last) {
        ObjectPointer next = list[first];
        if (next == Nil::NIL) {
            std::cout << "POPF NIL " << first << " " << list.hash() << std::endl;
            return Nil::NIL;
        }

        list[first] = next[System::LINK_NEXT];
        if (list[first] == Nil::NIL) {
            list[last] = Nil::NIL;
        }

        std::cout << "POPF " << next[System::LINK_VALUE].hash() << " " << first << " " << list.hash() << std::endl;
        return next[System::LINK_VALUE];
    }

    void Interpreter::pushFront(ObjectPointer value, ObjectPointer list, Offset first, Offset last) {
        std::cout << "PUSHF " << value.hash() << " " << first << " " << list.hash() << std::endl;
        ObjectPointer link = system.getMemoryManager().makeObject(System::LINK_SIZE, system.getTypeSystem().linkType);
        link[System::LINK_VALUE] = value;
        ObjectPointer head = list[first];
        if (head == Nil::NIL) {
            list[last] = link;
        } else {
            link[System::LINK_NEXT] = head;
        }
        list[first] = link;
    }

    void Interpreter::pushBack(ObjectPointer value, ObjectPointer list, Offset first, Offset last) {
        std::cout << "PUSHB " << value.hash() << " " << first << " " << list.hash() << std::endl;
        ObjectPointer link = system.getMemoryManager().makeObject(System::LINK_SIZE, system.getTypeSystem().linkType);
        link[System::LINK_VALUE] = value;
        ObjectPointer tail = list[last];
        if (tail == Nil::NIL) {
            list[first] = link;
        } else {
            tail[System::LINK_NEXT] = link;
        }
        list[last] = link;
    }


}