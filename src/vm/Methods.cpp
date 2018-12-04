//
// Created by Andreas Haufler on 26.11.18.
//

#include "Methods.h"
#include "Interpreter.h"

namespace pimii {

    Methods::Methods(MemoryManager &mm, TypeSystem &types) : mm(mm), types(types) {
    }


    void Methods::addMethod(ObjectPointer type, ObjectPointer selector,
                            ObjectPointer compiledMethod) {
        if (type[TypeSystem::TYPE_FIELD_SELECTORS] == Nil::NIL) {
            type[TypeSystem::TYPE_FIELD_SELECTORS] = ObjectPointer(
                    mm.makeObject(8, types.arrayType));
            type[TypeSystem::TYPE_FIELD_METHODS] = ObjectPointer(
                    mm.makeObject(8, types.arrayType));
            type[TypeSystem::TYPE_FIELD_TALLY] = ObjectPointer(0);
        }


        ObjectPointer selectors = type[TypeSystem::TYPE_FIELD_SELECTORS];
        ObjectPointer methods = type[TypeSystem::TYPE_FIELD_METHODS];
        Offset index = selector.hash() % selectors.size();
        for (Offset i = index; i < selectors.size(); i++) {
            if (tryInsert(type, i, selectors, methods, selector, compiledMethod)) {
                return;
            }
        }
        for (Offset i = 0; i < index; i++) {
            if (tryInsert(type, i, selectors, methods, selector, compiledMethod)) {
                return;
            }
        }

        //TODO horrible!!

    }

    bool Methods::tryInsert(ObjectPointer type, Offset index, ObjectPointer selectors,
                            ObjectPointer methods,
                            ObjectPointer selector, ObjectPointer method) {
        if (selectors[index] == selector) {
            methods[index] = method;
            return true;
        }

        if (selectors[index] == Nil::NIL) {
            selectors[index] = selector;
            methods[index] = method;
            SmallInteger newSize = type[TypeSystem::TYPE_FIELD_TALLY].smallInt() + 1;
            type[TypeSystem::TYPE_FIELD_TALLY] = newSize;
            if (newSize > selectors.size() * 0.75) {
                grow(type, selectors, methods);
            }
            return true;
        }

        return false;
    }

    void Methods::grow(ObjectPointer type, ObjectPointer selectors, ObjectPointer methods) {
        type[TypeSystem::TYPE_FIELD_SELECTORS] =
                mm.makeObject(selectors.size() + 8, types.arrayType);
        type[TypeSystem::TYPE_FIELD_METHODS] =
                mm.makeObject(selectors.size() + 8, types.arrayType);

        for (Offset i = 0; i < selectors.size(); i++) {
            if (selectors[i] != Nil::NIL) {
                addMethod(type, selectors[i], methods[i]);
            }
        }
    }

    ObjectPointer Methods::createMethod(Offset numberOfTemporaries,
                                        const std::vector<ObjectPointer> &literals,
                                        const std::vector<uint8_t> &byteCodes) {
        auto method = mm.makeObject(Interpreter::COMPILED_METHOD_SIZE + (Offset) literals.size(),
                                     types.compiledMethodType);
        Offset literalIndex = Interpreter::COMPILED_METHOD_FIELD_LITERALS_START;
        for (auto literal : literals) {
            method[literalIndex++] = literal;
        }
        auto bytes = mm.makeBuffer((Offset) byteCodes.size(), types.byteArrayType);
        method[Interpreter::COMPILED_METHOD_FIELD_HEADER] = ObjectPointer(
                (numberOfTemporaries << 2) | CompiledMethodType::BYTECODES);
        method[Interpreter::COMPILED_METHOD_FIELD_OPCODES] = bytes;
        bytes.loadFrom(byteCodes.data(), byteCodes.size());

        return method;
    }

    ObjectPointer Methods::createHeaderOnlyMethod(SmallInteger headerValue) {
        auto method = mm.makeObject(Interpreter::COMPILED_METHOD_SIZE, types.compiledMethodType);
        method[Interpreter::COMPILED_METHOD_FIELD_HEADER] = headerValue;
        return ObjectPointer(method);
    }


}

