#include <iostream>
#include <stdlib.h>
#include "src/vm/Interpreter.h"
#include "src/vm/Primitives.h"
#include "src/vm/Methods.h"
#include "src/compiler/Tokenizer.h"
#include "src/compiler/Compiler.h"
#include "src/vm/Nil.h"


int main() {
//    pimii::Tokenizer tokenizer("((3 + 4) * -1) abs , §komisch 'Te\\'st' - #test 1_00_000_0");
//    pimii::Token token = tokenizer.consume();
//    while(!token.isEOI()) {
//        std::cout << token.lineNumber << " " << token.type << " " << token.value << std::endl;
//        token = tokenizer.consume();
//    }
//
    pimii::System sys;
    pimii::Compiler compiler("ifTrue: aBlock otherwise: anotherBlock | a b c | a := 3. ^a + 4 - -2");
    pimii::ObjectPointer method = compiler.compile(sys);
    pimii::Interpreter interpreter(sys);
//
//    std::vector<pimii::ObjectPointer> literals;
//    literals.emplace_back(3);
//    literals.emplace_back(4);
//
//    std::vector<uint8_t> ops;
//    ops.push_back(pimii::Interpreter::OP_PUSH_LITERAL_CONSTANT);
//    ops.push_back(pimii::Interpreter::OP_PUSH_LITERAL_CONSTANT | (1 << 5));
//    ops.push_back(pimii::Interpreter::OP_SEND_SPECIAL_SELECTOR_WITH_ONE_ARG | (pimii::Primitives::PRIMITIVE_ADD << 5));
//    ops.push_back(pimii::Interpreter::OP_RETURN_STACK_TOP_TO_SENDER);
//
//    pimii::Methods methods(sys.getMemoryManager(), sys.getTypeSystem());
//
//    pimii::ObjectPointer method = methods.createMethod(0, literals, ops);
//
//
    pimii::Object *context = sys.getMemoryManager().allocObject(pimii::Interpreter::CONTEXT_FIXED_SIZE + 8,
                                                                pimii::Nil::NIL);
    context->fields[pimii::Interpreter::CONTEXT_IP_FIELD] = pimii::ObjectPointer(0);
    context->fields[pimii::Interpreter::CONTEXT_SP_FIELD] = pimii::ObjectPointer(0);
    context->fields[pimii::Interpreter::CONTEXT_METHOD_FIELD] = method;

    interpreter.newActiveContext(context);
    interpreter.run();

    return 0;
}