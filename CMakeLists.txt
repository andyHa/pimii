cmake_minimum_required(VERSION 3.12)
project(pimii)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")

add_executable(pimii main.cpp
        src/vm/SystemDictionary.cpp
        src/vm/Interpreter.cpp
        src/vm/SymbolTable.cpp
        src/vm/MemoryManager.cpp
        src/vm/TypeSystem.cpp
        src/vm/System.cpp
        src/vm/Methods.cpp
        src/vm/Primitives.cpp
        src/compiler/Compiler.cpp
        src/compiler/AST.cpp
        src/compiler/Tokenizer.cpp)

add_executable(pimii-tests src/tests/tests-main.cpp src/tests/ObjectPointerSpec.cpp src/vm/MemoryManager.cpp)