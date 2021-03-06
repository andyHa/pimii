//
// Created by Andreas Haufler on 27.11.18.
//

#ifndef MEM_COMPILER_H
#define MEM_COMPILER_H

#include <memory>
#include "AST.h"
#include "Tokenizer.h"

namespace pimii {

    class Compiler {
        Tokenizer& tokenizer;
        std::vector<Error>& errors;
        ObjectPointer type;

        ObjectPointer compile(System& system,EmitterContext& ctx);

        void parseSelector(EmitterContext& ctx);

        std::unique_ptr<Statement> statement();

        std::unique_ptr<Expression> expression();

        std::unique_ptr<Expression> atom();

        std::unique_ptr<Expression> parseName();

        std::unique_ptr<Expression> unaryCall(std::unique_ptr<Expression> receiver);

        std::unique_ptr<Expression> binaryCall(std::unique_ptr<Expression> receiver);

        std::unique_ptr<Expression> selectorCall(std::unique_ptr<Expression> receiver);

        std::unique_ptr<Expression> continuation();

    public:
        explicit Compiler(Tokenizer& tokenizer, std::vector<Error>& errors, ObjectPointer type) : tokenizer(tokenizer),
                                                                                                  errors(errors),
                                                                                                  type(type) {}

        ObjectPointer compileExpression(System& system);

        ObjectPointer compileMethod(System& system);

        void compileMethodAndAdd(System& system);

        void parseTemporaries(EmitterContext& ctx);

        std::unique_ptr<Expression> parseBlock();

        std::string selector;
    };

}

#endif //MEM_COMPILER_H
