//
// Created by Andreas Haufler on 26.11.18.
//

#ifndef MEM_METHODS_H
#define MEM_METHODS_H

#include "MemoryManager.h"
#include "TypeSystem.h"
#include <vector>


namespace pimii {


    enum CompiledMethodType : SmallInteger {
        MT_BYTECODES = 0b00,
        MT_PRIMITIVE = 0b01,
        MT_RETURN_FIELD = 0b10,
        MT_POP_AND_STORE_FIELD = 0b11
    };

    class MethodHeader {
        SmallInteger header;

        static constexpr SmallInteger LARGE_CONTEXT_FLAG_BIT = 0x40000;

    public:
        static SmallInteger forByteCodes(SmallInteger numTemporaries, bool largeContextFlag) {
            if (largeContextFlag) {
                return LARGE_CONTEXT_FLAG_BIT | ((numTemporaries & 0xFF) << 2) | MT_BYTECODES;
            } else {
                return ((numTemporaries & 0xFF) << 2) | MT_BYTECODES;
            }
        }

        static SmallInteger forPrimitive(SmallInteger primitiveIndex, SmallInteger numTemporaries, bool largeContextFlag) {
            if (largeContextFlag) {
                return LARGE_CONTEXT_FLAG_BIT | ((primitiveIndex & 0xFF) << 10) | ((numTemporaries & 0xFF) << 2) |
                       MT_PRIMITIVE;
            } else {
                return ((primitiveIndex & 0xFF) << 10) | ((numTemporaries & 0xFF) << 2) | MT_PRIMITIVE;
            }
        }

        MethodHeader(SmallInteger header) : header(header) {}

        MethodHeader(MethodHeader& header) = default;

        MethodHeader& operator=(MethodHeader& header) = default;

        CompiledMethodType methodType() {
            return static_cast<CompiledMethodType>(header & 0b11);
        }

        SmallInteger temporaries() {
            return static_cast<SmallInteger>((header >> 2) & 0xFF);
        }

        SmallInteger primitiveIndex() {
            return static_cast<SmallInteger>((header >> 10) & 0xFF);
        }

        SmallInteger fieldIndex() {
            return static_cast<SmallInteger>((header >> 2) & 0xFF);
        }

        bool largeContextFlag() {
            return (header & LARGE_CONTEXT_FLAG_BIT) == LARGE_CONTEXT_FLAG_BIT;
        }

        SmallInteger value() {
            return header;
        }
    };

    class Methods {

        MemoryManager& mm;
        TypeSystem& types;

        void grow(ObjectPointer type, ObjectPointer selectors, ObjectPointer methods);

    public:
        Methods(MemoryManager& mm, TypeSystem& types);

        void addMethod(ObjectPointer type, ObjectPointer selector,
                       ObjectPointer compiledMethod);

        ObjectPointer createMethod(MethodHeader header,
                                   const std::vector<ObjectPointer>& literals,
                                   const std::vector<uint8_t>& byteCodes);

    };

}

#endif //MEM_METHODS_H
