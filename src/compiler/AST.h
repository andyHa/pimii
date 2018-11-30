//
// Created by Andreas Haufler on 28.11.18.
//

#ifndef MEM_AST_H
#define MEM_AST_H


#include "../vm/System.h"

namespace pimii {

    class EmitterContext {
        System &system;
        std::vector<uint8_t> opcodes;
        std::vector<ObjectPointer> literals;
        std::vector<std::string> temporaries;
        Offset maxTemporaries;

    public:
        explicit EmitterContext(System &system) : system(system), maxTemporaries(0) {}

        System& getSystem();

        Offset  getMaxTemporaries();

        const std::vector<ObjectPointer>& getLiterals() const;

        const std::vector<std::string>& getTemporaries() const {
            return temporaries;
        };

        const std::vector<uint8_t>& getOpCodes() const;

        void pushTemporaries(const std::vector<std::string>& temporariesToPush);

        void pushTemporary(const std::string& temporary);

        void popTemporaries(size_t numTemporaries);



        int findTemporaryIndex(std::string &name);

        Offset findOrAddLiteral(ObjectPointer object);

        Offset addLiteral(ObjectPointer object);

        void pushSingle(uint8_t opcode);

        void pushCompound(uint8_t opcode, int index);

        void pushWithIndex(uint8_t opcode, int index);

        Offset nextOpCodePosition();

        Offset pushJumpPlaceholder();

        void pushJump(uint8_t opcode, Offset delta);

        void insertJump(Offset index, uint8_t opcode, Offset delta);


    };

    enum StatementType {
        STMT_OTHER,
        STMT_BLOCK
    };

    struct Statement {
        virtual void emitByteCodes(EmitterContext &ctx) = 0;
        virtual StatementType type() const { return STMT_OTHER; }
    };

    struct Expression : public Statement {

    };

    struct Assignment : public Statement {
        std::string name;
        std::unique_ptr<Expression> expression;

    public:
        void emitByteCodes(EmitterContext &ctx) override;
    };


    struct Return : public Expression {
        std::unique_ptr<Expression> expression;
    public:
        Return(std::unique_ptr<Expression> expression) : expression(std::move(expression)) {}
        void emitByteCodes(EmitterContext &ctx) override;
    };

    struct PushGlobal : public Expression {
        std::string name;
    public:
        PushGlobal(std::string name) : name(std::move(name)) {}
        void emitByteCodes(EmitterContext &ctx) override;
    };

    struct PushLocal : public Expression {
        std::string name;
    public:
        PushLocal(std::string name) : name(std::move(name)) {}
        void emitByteCodes(EmitterContext &ctx) override;
    };

    struct BuiltinConstant : public Expression {
        uint8_t opcode;
        uint8_t compound;
    public:
        BuiltinConstant(uint8_t opcode, uint8_t compound) : opcode(opcode), compound(compound) {}

        void emitByteCodes(EmitterContext &ctx) override;
    };

    struct LiteralSymbol : public Expression {
        std::string name;
    public:
        LiteralSymbol(std::string name) : name(std::move(name)) {}

        void emitByteCodes(EmitterContext &ctx) override;

    };

    struct LiteralString : public Expression {
        std::string name;
    public:
        LiteralString(std::string name) : name(std::move(name)) {}

        void emitByteCodes(EmitterContext &ctx) override;
    };

    struct LiteralNumber : public Expression {
        SmallInteger number;
    public:
        LiteralNumber(SmallInteger number) : number(number) {}

        void emitByteCodes(EmitterContext &ctx) override;
    };

    struct MethodCall : public Expression {
        std::unique_ptr<Expression> receiver;
        std::string selector;
        std::vector<std::unique_ptr<Expression>> arguments;
        bool callSuper;

        void emitByteCodes(EmitterContext &ctx) override;
    private:
        bool emitOptimizedControlFlow(EmitterContext &ctx);
    };


    //class ContinuationChain : public Expression {
    //    MethodCall initialCall;
    //    std::vector<
    //};

    struct Block : public Expression {
        std::vector<std::string> temporaries;
        std::vector<std::unique_ptr<Statement>> statements;

        void emitByteCodes(EmitterContext &ctx) override;
        StatementType  type() const override { return STMT_BLOCK; };

        void emitInner(EmitterContext &context);
    };


}

#endif //MEM_AST_H
