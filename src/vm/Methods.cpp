//
// Created by Andreas Haufler on 26.11.18.
//

#include "Methods.h"
#include "Interpreter.h"
#include "Nil.h"

namespace pimii {

    Methods::Methods(MemoryManager &mm, TypeSystem &types) : mm(mm), types(types) {
    }


    void Methods::addMethod(ObjectPointer type, ObjectPointer selector,
                            ObjectPointer compiledMethod) {
        if (type.getObject()->fields[TypeSystem::TYPE_FIELD_SELECTORS] == Nil::NIL) {
            type.getObject()->fields[TypeSystem::TYPE_FIELD_SELECTORS] = ObjectPointer(
                    mm.allocObject(8, types.arrayType));
            type.getObject()->fields[TypeSystem::TYPE_FIELD_METHODS] = ObjectPointer(
                    mm.allocObject(8, types.arrayType));
            type.getObject()->fields[TypeSystem::TYPE_FIELD_TALLY] = ObjectPointer(0);
        }


        Object *selectors = type.getObject()->fields[TypeSystem::TYPE_FIELD_SELECTORS].getObject();
        Object *methods = type.getObject()->fields[TypeSystem::TYPE_FIELD_METHODS].getObject();
        Offset index = selector.hash() % selectors->size;
        for (Offset i = index; i < selectors->size; i++) {
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

    bool Methods::tryInsert(ObjectPointer type, Offset index, Object *selectors,
                            Object *methods,
                            ObjectPointer selector, ObjectPointer method) {
        if (selectors->fields[index] == selector) {
            methods->fields[index] = method;
            return true;
        }

        if (selectors->fields[index] == Nil::NIL) {
            selectors->fields[index] = selector;
            methods->fields[index] = method;
            SmallInteger newSize = type.getObject()->fields[TypeSystem::TYPE_FIELD_TALLY].getInt() + 1;
            type.getObject()->fields[TypeSystem::TYPE_FIELD_TALLY] = ObjectPointer(newSize);
            if (newSize > selectors->size * 0.75) {
                grow(type, selectors, methods);
            }
            return true;
        }

        return false;
    }

    void Methods::grow(ObjectPointer type, Object *selectors, Object *methods) {
        type.getObject()->fields[TypeSystem::TYPE_FIELD_SELECTORS] = ObjectPointer(
                mm.allocObject(selectors->size + 8, types.arrayType));
        type.getObject()->fields[TypeSystem::TYPE_FIELD_METHODS] = ObjectPointer(
                mm.allocObject(selectors->size + 8, types.arrayType));

        for (Offset i = 0; i < selectors->size; i++) {
            if (selectors->fields[i] != Nil::NIL) {
                addMethod(type, selectors->fields[i], methods->fields[i]);
            }
        }
    }

    ObjectPointer Methods::createMethod(Offset numberOfTemporaries,
                                        const std::vector<ObjectPointer> &literals,
                                        const std::vector<uint8_t> &byteCodes) {
        auto *method = mm.allocObject(Interpreter::COMPILED_METHOD_SIZE + (Offset) literals.size(),
                                      types.compiledMethodType);
        Offset literalIndex = Interpreter::COMPILED_METHOD_FIELD_LITERALS_START;
        for (auto literal : literals) {
            method->fields[literalIndex++] = literal;
        }
        auto *bytes = mm.allocBytes((Offset) byteCodes.size(), types.byteArrayType);
        method->fields[Interpreter::COMPILED_METHOD_FIELD_HEADER] = ObjectPointer(
                (numberOfTemporaries << 2) | CompiledMethodType::BYTECODES);
        method->fields[Interpreter::COMPILED_METHOD_FIELD_OPCODES] = ObjectPointer(bytes);
        char *dest = bytes->bytes;
        for (auto b : byteCodes) {
            *dest = b;
            dest++;
        }
        return ObjectPointer(method);
    }

    ObjectPointer Methods::createPrimitiveMethod(Offset primitiveIndex) {
        return createHeaderOnlyMethod((primitiveIndex << 2) | CompiledMethodType::PRIMITIVE);
    }

    ObjectPointer Methods::createReturnFieldMethod(Offset fieldIndex) {
        return createHeaderOnlyMethod((fieldIndex << 2) | CompiledMethodType::RETURN_FIELD);
    }

    ObjectPointer Methods::createPopAndStoreFieldMethod(Offset fieldIndex) {
        return createHeaderOnlyMethod((fieldIndex << 2) | CompiledMethodType::POP_AND_STORE_FIELD);
    }

    ObjectPointer Methods::createHeaderOnlyMethod(SmallInteger headerValue) {
        auto *method = mm.allocObject(Interpreter::COMPILED_METHOD_SIZE, types.compiledMethodType);
        method->fields[Interpreter::COMPILED_METHOD_FIELD_HEADER] = ObjectPointer(headerValue);
        return ObjectPointer(method);
    }


}

