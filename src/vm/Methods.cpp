//
// Created by Andreas Haufler on 26.11.18.
//

#include "Methods.h"
#include "Interpreter.h"
#include "Looping.h"

namespace pimii {

    Methods::Methods(MemoryManager& mm, TypeSystem& types) : mm(mm), types(types) {
    }

    void Methods::addMethod(ObjectPointer type, ObjectPointer selector,
                            ObjectPointer compiledMethod) {
        if (type[TypeSystem::TYPE_FIELD_SELECTORS] == Nil::NIL) {
            type[TypeSystem::TYPE_FIELD_SELECTORS] = ObjectPointer(
                    mm.makeObject(8, types.arrayType));
            type[TypeSystem::TYPE_FIELD_METHODS] = ObjectPointer(
                    mm.makeObject(8, types.arrayType));
            type[TypeSystem::TYPE_FIELD_TALLY] = 0;
        }


        ObjectPointer selectors = type[TypeSystem::TYPE_FIELD_SELECTORS];
        ObjectPointer methods = type[TypeSystem::TYPE_FIELD_METHODS];

        for (Looping loop = Looping(selectors.size(), selector.hash()); loop.hasNext(); loop.next()) {
            if (selectors[loop()] == selector) {
                methods[loop()] = compiledMethod;
                return;
            }

            if (selectors[loop()] == Nil::NIL) {
                selectors[loop()] = selector;
                methods[loop()] = compiledMethod;
                SmallInteger newSize = type[TypeSystem::TYPE_FIELD_TALLY].smallInt() + 1;
                if (newSize > selectors.size() * 0.75) {
                    grow(type, selectors, methods);
                } else {
                    type[TypeSystem::TYPE_FIELD_TALLY] = newSize;
                }
                return;
            }
        }

        //TODO horrible!!

    }

    void Methods::grow(ObjectPointer type, ObjectPointer selectors, ObjectPointer methods) {
        type[TypeSystem::TYPE_FIELD_SELECTORS] =
                mm.makeObject(selectors.size() + 8, types.arrayType);
        type[TypeSystem::TYPE_FIELD_METHODS] =
                mm.makeObject(selectors.size() + 8, types.arrayType);
        type[TypeSystem::TYPE_FIELD_TALLY] = 0;
        for (SmallInteger i = 0; i < selectors.size(); i++) {
            if (selectors[i] != Nil::NIL) {
                addMethod(type, selectors[i], methods[i]);
            }
        }
    }

    ObjectPointer Methods::createMethod(MethodHeader header,
                                        const std::vector<ObjectPointer>& literals,
                                        const std::vector<uint8_t>& byteCodes) {
        auto method = mm.makeObject(Interpreter::COMPILED_METHOD_SIZE + (SmallInteger) literals.size(),
                                    types.compiledMethodType);
        method[Interpreter::COMPILED_METHOD_FIELD_HEADER] = header.value();

        SmallInteger literalIndex = Interpreter::COMPILED_METHOD_FIELD_LITERALS_START;
        for (auto literal : literals) {
            method[literalIndex++] = literal;
        }

        if (!byteCodes.empty()) {
            auto bytes = mm.makeBuffer((SmallInteger) byteCodes.size(), types.byteArrayType);
            method[Interpreter::COMPILED_METHOD_FIELD_OPCODES] = bytes;
            bytes.loadFrom(byteCodes.data(), byteCodes.size());
        }

        return method;
    }


}

