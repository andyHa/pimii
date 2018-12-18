cmake_minimum_required(VERSION 3.12)
project(pimii)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")

add_executable(pimii main.cpp
        src/vm/SystemDictionary.cpp
        src/vm/Interpreter.cpp
        src/vm/SymbolTable.cpp
        src/mem/MemoryManager.cpp
        src/vm/System.cpp
        src/compiler/Methods.cpp
        src/vm/Primitives.cpp
        src/compiler/Compiler.cpp
        src/compiler/AST.cpp
        src/compiler/Tokenizer.cpp src/common/Looping.h src/common/types.h src/compiler/SourceFileParser.cpp src/compiler/SourceFileParser.h src/mem/Segment.h src/mem/Allocator.h)

add_executable(pimii-tests src/tests/tests-main.cpp src/tests/ObjectPointerSpec.cpp src/mem/MemoryManager.cpp)