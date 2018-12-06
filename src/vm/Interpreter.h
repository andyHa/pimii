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
        ObjectPointer activeContext;
        ObjectPointer homeContext;
        ObjectPointer method;
        ObjectPointer opCodes;
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

        static inline const uint8_t OP_RETURN = 0;
        static inline const uint8_t OP_RETURN_RECEIVER_INDEX = 0;
        static inline const uint8_t OP_RETURN_TRUE_INDEX = 1;
        static inline const uint8_t OP_RETURN_FALSE_INDEX = 2;
        static inline const uint8_t OP_RETURN_NIL_INDEX = 3;
        static inline const uint8_t OP_RETURN_STACK_TOP_TO_SENDER_INDEX = 4;
        static inline const uint8_t OP_RETURN_STACK_TO_TO_CALLER_INDEX = 5;
        static inline const uint8_t OP_PUSH_LITERAL_CONSTANT = 2;
        static inline const uint8_t OP_PUSH_LITERAL_VARIABLE = 3;
        static inline const uint8_t OP_PUSH_TEMPORARY = 4;
        static inline const uint8_t OP_PUSH_RECEIVER_FIELD = 5;
        static inline const uint8_t OP_PUSH = 6;
        static inline const uint8_t OP_PUSH_RECEIVER_INDEX = 0;
        static inline const uint8_t OP_PUSH_TRUE_INDEX = 1;
        static inline const uint8_t OP_PUSH_FALSE_INDEX = 2;
        static inline const uint8_t OP_PUSH_NIL_INDEX = 3;
        static inline const uint8_t OP_PUSH_MINUS_ONE_INDEX = 4;
        static inline const uint8_t OP_PUSH_ZERO_INDEX = 5;
        static inline const uint8_t OP_PUSH_ONE_INDEX = 6;
        static inline const uint8_t OP_PUSH_TWO_INDEX = 7;
        static inline const uint8_t OP_POP_AND_STORE_RECEIVER_FIELD = 7;
        static inline const uint8_t OP_POP_AND_STORE_IN_TEMPORARY = 8;
        static inline const uint8_t OP_POP_AND_STORE_IN_LITERAL_VARIABLE = 9;
        static inline const uint8_t OP_SEND_LITERAL_SELECTOR_WITH_NO_ARGS = 10;
        static inline const uint8_t OP_SEND_LITERAL_SELECTOR_WITH_ONE_ARG = 11;
        static inline const uint8_t OP_SEND_LITERAL_SELECTOR_WITH_TWO_ARGS = 12;
        static inline const uint8_t OP_SEND_LITERAL_SELECTOR_WITH_N_ARGS = 13;
        static inline const uint8_t OP_SEND_SPECIAL_SELECTOR_WITH_NO_ARGS = 14;
        static inline const uint8_t OP_SEND_SPECIAL_SELECTOR_WITH_ONE_ARG = 15;
        static inline const uint8_t OP_SEND_SPECIAL_SELECTOR_WITH_TWO_ARGS = 16;
        static inline const uint8_t OP_SEND_SPECIAL_SELECTOR_WITH_N_ARGS = 17;
        static inline const uint8_t OP_JUMP_ON_TRUE = 18;
        static inline const uint8_t OP_JUMP_ON_FALSE = 19;
        static inline const uint8_t OP_JUMP_ALWAYS = 20;
        static inline const uint8_t OP_JUMP_BACK = 21;
        static inline const uint8_t OP_BLOCK_COPY = 22;

        static inline const uint8_t LAST_PREFERRED_PRIMITIVE_SELECTOR = 22;

        explicit Interpreter(System &system);

        System& getSystem();

        ObjectPointer getActiveContext();

        void newActiveContext(ObjectPointer context);

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

        bool isBlockContext(ObjectPointer context);

        Offset getInstructionPointer();

        Offset getStackPointer();

        Offset getStackBasePointer();
    private:
        void dispatchOpCode(uint8_t opCode);

        void storeContextRegisters();

        void fetchContextRegisters();

        ObjectPointer findMethod(ObjectPointer type, ObjectPointer selector);

        ObjectPointer findMethodInType(ObjectPointer type, ObjectPointer selector);

        CompiledMethodType getMethodType(ObjectPointer method, Offset &offset);

        bool executePrimitive(Offset index, Offset numberOfArguments);

        void handleJump(uint8_t code, uint8_t index);

        void performBlockCopy(uint8_t index);

    };

}


#endif //MEM_INTERPRETER_H
