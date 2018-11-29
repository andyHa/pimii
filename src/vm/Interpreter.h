//
// Created by Andreas Haufler on 23.11.18.
//

#ifndef MEM_INTERPRETER_H
#define MEM_INTERPRETER_H

#include "System.h"

namespace pimii {

    enum CompiledMethodType : Offset {
        BYTECODES = 0b00,
        PRIMITIVE = 0b01,
        RETURN_FIELD = 0b10,
        POP_AND_STORE_FIELD = 0b11
    };

    class Interpreter {
        System &system;
        Offset instructionPointer;
        Offset stackPointer;
        Object *activeContext;
        Object *homeContext;
        Object *method;
        ByteBuffer *opCodes;
        Offset maxIP;
        Offset temporaryCount;
        ObjectPointer receiver;
    public:

        static const Offset COMPILED_METHOD_SIZE;
        static const Offset COMPILED_METHOD_FIELD_HEADER;
        static const Offset COMPILED_METHOD_FIELD_OPCODES;
        static const Offset COMPILED_METHOD_FIELD_LITERALS_START;
        static const Offset COMPILED_METHOD_TYPE_FIELD_SPECIAL_SELECTORS;

        static const Offset CONTEXT_FIXED_SIZE;
        static const Offset CONTEXT_SENDER_FIELD;
        static const Offset CONTEXT_CALLER_FIELD;
        static const Offset CONTEXT_IP_FIELD;
        static const Offset CONTEXT_SP_FIELD;
        static const Offset CONTEXT_METHOD_FIELD;
        static const Offset CONTEXT_BLOCK_ARGUMENT_COUNT_FIELD;
        static const Offset CONTEXT_INITIAL_IP_FIELD;
        static const Offset CONTEXT_HOME_FIELD;
        static const Offset CONTEXT_RECEIVER_FIELD;

        static const uint8_t OP_RETURN_RECEIVER;
        static const uint8_t OP_RETURN_TRUE;
        static const uint8_t OP_RETURN_FALSE;
        static const uint8_t OP_RETURN_NIL;
        static const uint8_t OP_RETURN_STACK_TOP_TO_SENDER;
        static const uint8_t OP_RETURN_STACK_TO_TO_CALLER;
        static const uint8_t OP_PUSH_LITERAL_CONSTANT;
        static const uint8_t OP_PUSH_LITERAL_VARIABLE;
        static const uint8_t OP_PUSH_TEMPORARY;
        static const uint8_t OP_PUSH_RECEIVER_FIELD;
        static const uint8_t OP_PUSH_RECEIVER;
        static const uint8_t OP_PUSH_TRUE;
        static const uint8_t OP_PUSH_FALSE;
        static const uint8_t OP_PUSH_NIL;
        static const uint8_t OP_PUSH_MINUS_ONE;
        static const uint8_t OP_PUSH_ZERO;
        static const uint8_t OP_PUSH_ONE;
        static const uint8_t OP_PUSH_TWO;
        static const uint8_t OP_POP_AND_STORE_RECEIVER_FIELD;
        static const uint8_t OP_POP_AND_STORE_IN_TEMPORARY;
        static const uint8_t OP_SEND_LITERAL_SELECTOR_WITH_NO_ARGS;
        static const uint8_t OP_SEND_LITERAL_SELECTOR_WITH_ONE_ARG;
        static const uint8_t OP_SEND_LITERAL_SELECTOR_WITH_TWO_ARGS;
        static const uint8_t OP_SEND_LITERAL_SELECTOR_WITH_N_ARGS;
        static const uint8_t OP_SEND_SPECIAL_SELECTOR_WITH_NO_ARGS;
        static const uint8_t OP_SEND_SPECIAL_SELECTOR_WITH_ONE_ARG;
        static const uint8_t OP_SEND_SPECIAL_SELECTOR_WITH_TWO_ARGS;
        static const uint8_t OP_SEND_SPECIAL_SELECTOR_WITH_N_ARGS;

        static const uint8_t LAST_PREFERRED_PRIMITIVE_SELECTOR = 23;

        explicit Interpreter(System &system);

        System& getSystem();

        Object* getActiveContext();

        void newActiveContext(Object *context);

        void run();

        uint8_t fetchInstruction();

        void push(ObjectPointer value);

        ObjectPointer pop();

        ObjectPointer stackTop();

        ObjectPointer stackValue(Offset offset);

        void pop(Offset number);

        void unPop(Offset number);

        ObjectPointer sender();

        ObjectPointer caller();

        ObjectPointer getReceiver();

        ObjectPointer temporary(Offset index);

        void temporary(Offset index, ObjectPointer value);

        ObjectPointer literal(Offset index);

        void returnValueTo(ObjectPointer returnValue, ObjectPointer targetContext);

        void send(ObjectPointer selector, Offset numArguments);

        bool isBlockContext(Object *context);

        void transfer(Offset numberOfFields, Object* src, Offset srcIndex, Object* dst, Offset destIndex);

        Offset getInstructionPointer();

        Offset getStackPointer();

        Offset getStackBasePointer();
    private:
        void dispatchOpCode(uint8_t opCode);

        void storeContextRegisters();

        void fetchContextRegisters();

        ObjectPointer findMethod(ObjectPointer type, ObjectPointer selector);

        CompiledMethodType getMethodType(ObjectPointer method, Offset &offset);

        bool executePrimitive(Offset index, Offset numberOfArguments);

    };

}


#endif //MEM_INTERPRETER_H
