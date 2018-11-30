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
        std::string input;
        ObjectPointer type;
        std::vector<Error> errors;
        Tokenizer tokenizer;
        std::string selector;

        void parseSelector(EmitterContext &ctx);

        std::unique_ptr<Statement> statement();

        std::unique_ptr<Expression> expression(bool acceptColonSelectors);

        std::unique_ptr<Expression> atom();

        std::unique_ptr<Expression> parseName();

        std::unique_ptr<Expression> unaryCall(std::unique_ptr<Expression> receiver);

        std::unique_ptr<Expression> binaryCall(std::unique_ptr<Expression> receiver);

        std::unique_ptr<Expression> selectorCall(std::unique_ptr<Expression> receiver);

        std::unique_ptr<Expression> continuation();

    public:
        explicit Compiler(std::string source, ObjectPointer type) : input(std::move(source)), errors(), tokenizer(input, errors) {}

        ObjectPointer compile(System &system);

        const std::vector<Error> &getErrors() const;

        void parseTemporaries(EmitterContext &ctx);

        std::unique_ptr<Expression> parseBlock();
    };

}

#endif //MEM_COMPILER_H
