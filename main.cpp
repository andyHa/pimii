#include <iostream>
#include <fstream>
#include <thread>
#include <stdlib.h>
#include <ncurses.h>

#include "src/vm/Interpreter.h"
#include "src/vm/Primitives.h"
#include "src/compiler/Methods.h"
#include "src/compiler/Tokenizer.h"
#include "src/compiler/Compiler.h"
#include "src/compiler/SourceFileParser.h"


int main() {


    std::cout << pimii::SmallIntegers::minSmallInt() << std::endl;
    std::cout << pimii::SmallIntegers::maxSmallInt() << std::endl;
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
//            "[ [ true ] whileTrue: [ InputSemaphore wait. [ Terminal nextEvent] whileNotNil: [ :event | Terminal at: 0 @ 0 color: 0 put: event key asString. Terminal draw. ]. ] ] fork. [ true ] whileTrue: [ TimerSemaphore wait. ].",
            //  "Terminal at: 0 @ 0 color: 0 put: (#true asString). Terminal draw.",
            //   "Terminal at: 0 @ 0 color: 0 put: ((Rectangle origin: 16@1 dimensions: 10@10) intersects: (Rectangle origin: 5@5 dimensions: 10@10) ) asString.",
            "Terminal box: (Rectangle origin: 0 @ 0 dimensions: 10 @ 10) color: 1. Terminal draw.",
            errors);

    pimii::Compiler compiler(tokenizer, errors, sys.typeArray());
    pimii::ObjectPointer method = compiler.compileExpression(sys);
    //pimii::Compiler compiler("xx [ :a :b | a + b] value: 3 value: 4", pimii::Nil::NIL);
//    pimii::ObjectPointer method = compiler.compile(sys);
    pimii::Interpreter interpreter(sys);
    pimii::ObjectPointer context = sys.memoryManager().makeObject(pimii::System::CONTEXT_SIZE,
                                                                  pimii::Nil::NIL);
    context[pimii::System::CONTEXT_IP_FIELD] = pimii::ObjectPointer::forSmallInt(0);
    context[pimii::System::CONTEXT_SP_FIELD] = pimii::ObjectPointer::forSmallInt(0);
    context[pimii::System::CONTEXT_METHOD_FIELD] = method;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    //interpreter.newActiveContext(context);

    initscr();
    cbreak();
    noecho();
    timeout(0);
    start_color();

    // Enables keypad mode. This makes (at least for me) mouse events getting
    // reported as KEY_MOUSE, instead as of random letters.
  // keypad(stdscr, TRUE);

    // Don't mask any mouse events
 //  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

  //    printf("\033[?1003h\n"); // Makes the terminal report mouse movement events
//
//    std::thread([&sys]() {
//        while (true) {
//            sys.fireTimer();
//            std::this_thread::sleep_for(std::chrono::milliseconds(500));
//        }
//    }).detach();
//
//    std::thread([&interpreter]() {
//        while (true) {
//            interpreter.updateMetrics();
//            std::this_thread::sleep_for(std::chrono::seconds(1));
//        }
//    }).detach();

    /*  std::thread([&sys]() {
          while (true) {
              int c = wgetch(stdscr);
              sys.recordInputEvent(pimii::InputEvent::keyPressed(c));
          }
      }).detach();
  */
    try {
        interpreter.run(context);
    } catch (const std::exception& e) {
        endwin();
        std::cout << e.what() << std::endl;
        pimii::ObjectPointer ctx = interpreter.currentActiveContext();
        ctx[pimii::System::CONTEXT_IP_FIELD] = pimii::ObjectPointer::forSmallInt(interpreter.currentIP());
        int limit = 25;
        while (limit-- > 0 && ctx != pimii::Nil::NIL) {
            pimii::ObjectPointer method = ctx[pimii::System::CONTEXT_METHOD_FIELD];
            if (method.isSmallInt()) {
                method = ctx[pimii::System::CONTEXT_HOME_FIELD][pimii::System::CONTEXT_METHOD_FIELD];
            }
            pimii::ObjectPointer type = method[pimii::System::COMPILED_METHOD_FIELD_OWNER];
            pimii::ObjectPointer selector = method[pimii::System::COMPILED_METHOD_FIELD_SELECTOR];
            std::cout
                    << (type == pimii::Nil::NIL ? "?" : type[pimii::System::TYPE_FIELD_NAME].stringView()) << " "
                    << selector.stringView()
                    << " - IP: "
                    << ctx[pimii::System::CONTEXT_IP_FIELD].smallInt()
                    << std::endl;
            ctx = ctx[pimii::System::CONTEXT_SENDER_FIELD];
        }
    }
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