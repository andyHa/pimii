cmake_minimum_required(VERSION 3.12)
project(mem)

set(CMAKE_CXX_STANDARD 17)

add_executable(mem main.cpp src/vm/ObjectPointer.h src/vm/SystemDictionary.cpp src/vm/SystemDictionary.h src/vm/Interpreter.cpp src/vm/Interpreter.h src/vm/SymbolTable.cpp src/vm/SymbolTable.h src/vm/MemoryManager.cpp src/vm/MemoryManager.h src/vm/TypeSystem.cpp src/vm/TypeSystem.h src/vm/System.cpp src/vm/System.h src/vm/Strings.cpp src/vm/Strings.h src/vm/Methods.cpp src/vm/Methods.h src/vm/Primitives.cpp src/vm/Primitives.h src/compiler/Compiler.cpp src/compiler/Compiler.h src/compiler/AST.h src/compiler/AST.cpp src/compiler/Tokenizer.cpp src/compiler/Tokenizer.h src/vm/Nil.h)