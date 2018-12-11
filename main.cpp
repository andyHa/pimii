#include <iostream>
#include <fstream>
#include <thread>
#include <stdlib.h>

#include "src/vm/Interpreter.h"
#include "src/vm/Primitives.h"
#include "src/vm/Methods.h"
#include "src/compiler/Tokenizer.h"
#include "src/compiler/Compiler.h"
#include "src/compiler/SourceFileParser.h"


int main() {


    std::cout << pimii::minSmallInt() << std::endl;
    std::cout << pimii::maxSmallInt() << std::endl;
    std::cout << sizeof(std::chrono::steady_clock::time_point) << std::endl;
//    pimii::Tokenizer tokenizer("((3 + 4) * -1) abs , §komisch 'Te\\'st' - #test 1_00_000_0");
//    pimii::Token token = tokenizer.consume();
//    while(!token.isEOI()) {
//        std::cout << token.lineNumber << " " << token.type << " " << token.value << std::endl;
//        token = tokenizer.consume();
//    }
//
    pimii::System sys;
    std::ifstream ifs("source.st");
    std::string content;
    content.assign(std::istreambuf_iterator<char>(ifs),
                   std::istreambuf_iterator<char>());

    pimii::SourceFileParser parser(sys, content);
    parser.compile();


    std::vector<pimii::Error> errors;
    pimii::Tokenizer tokenizer(
            "[ [ true ] whileTrue: [ System log: 'T'. TimerSemaphore wait. ] ] fork.",
            errors);
    pimii::Compiler compiler(tokenizer, errors, pimii::Nil::NIL);
    pimii::ObjectPointer method = compiler.compileExpression(sys);
    //pimii::Compiler compiler("xx [ :a :b | a + b] value: 3 value: 4", pimii::Nil::NIL);
//    pimii::ObjectPointer method = compiler.compile(sys);
    pimii::Interpreter interpreter(sys);
    pimii::ObjectPointer context = sys.getMemoryManager().makeObject(pimii::Interpreter::CONTEXT_FIXED_SIZE + 8,
                                                                     pimii::Nil::NIL);
    context[pimii::Interpreter::CONTEXT_IP_FIELD] = pimii::ObjectPointer::forSmallInt(0);
    context[pimii::Interpreter::CONTEXT_SP_FIELD] = pimii::ObjectPointer::forSmallInt(0);
    context[pimii::Interpreter::CONTEXT_METHOD_FIELD] = method;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    //interpreter.newActiveContext(context);

    std::thread([&sys]() {
        while (true) {
            sys.irq(pimii::IRQ_TIMER);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }).detach();

    interpreter.run(context);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Took: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "us"
              << std::endl;
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
//    pimii::ObjectPointer context = sys.getMemoryManager().makeObject(pimii::Interpreter::CONTEXT_FIXED_SIZE + 8,
//                                                                     pimii::Nil::NIL);
//    context[pimii::Interpreter::CONTEXT_IP_FIELD] = pimii::ObjectPointer::forSmallInt(0);
//    context[pimii::Interpreter::CONTEXT_SP_FIELD] = pimii::ObjectPointer::forSmallInt(0);
//    context[pimii::Interpreter::CONTEXT_METHOD_FIELD] = method;
//
//    interpreter.newActiveContext(context);
//    interpreter.run();
//
//    sys.getMemoryManager().gc();
//    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
//    context = sys.getMemoryManager().makeObject(pimii::Interpreter::CONTEXT_FIXED_SIZE + 8,
//                                                pimii::Nil::NIL);
//    context[pimii::Interpreter::CONTEXT_IP_FIELD] = pimii::ObjectPointer::forSmallInt(0);
//    context[pimii::Interpreter::CONTEXT_SP_FIELD] = pimii::ObjectPointer::forSmallInt(0);
//    context[pimii::Interpreter::CONTEXT_METHOD_FIELD] = method;
//
//    interpreter.newActiveContext(context);
//    interpreter.run();
//
//    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//    std::cout << "Took: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "us"
//              << std::endl;

    return 0;
}