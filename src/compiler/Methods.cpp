//
// Created by Andreas Haufler on 26.11.18.
//

#include "Methods.h"
#include "../vm/Interpreter.h"
#include "../common/Looping.h"

namespace pimii {

    Methods::Methods(MemoryManager& mm, System& sys) : mm(mm), sys(sys) {
    }

    void Methods::addMethod(ObjectPointer type, ObjectPointer selector,
                            ObjectPointer compiledMethod) {
        if (type[System::TYPE_FIELD_SELECTORS] == Nil::NIL) {
            type[System::TYPE_FIELD_SELECTORS] = ObjectPointer(
                    mm.makeObject(8, sys.typeArray()));
            type[System::TYPE_FIELD_METHODS] = ObjectPointer(
                    mm.makeObject(8, sys.typeArray()));
            type[System::TYPE_FIELD_TALLY] = 0;
        }


        ObjectPointer selectors = type[System::TYPE_FIELD_SELECTORS];
        ObjectPointer methods = type[System::TYPE_FIELD_METHODS];

        for (Looping loop = Looping(selectors.size(), selector.id()); loop.hasNext(); loop.next()) {
            if (selectors[loop()] == selector) {
                methods[loop()] = compiledMethod;
                return;
            }

            if (selectors[loop()] == Nil::NIL) {
                selectors[loop()] = selector;
                methods[loop()] = compiledMethod;
                SmallInteger newSize = type[System::TYPE_FIELD_TALLY].smallInt() + 1;
                if (newSize > selectors.size() * 0.75) {
                    grow(type, selectors, methods);
                } else {
                    type[System::TYPE_FIELD_TALLY] = newSize;
                }
                return;
            }
        }

        //TODO horrible!!

    }

    void Methods::grow(ObjectPointer type, ObjectPointer selectors, ObjectPointer methods) {
        type[System::TYPE_FIELD_SELECTORS] =
                mm.makeObject(selectors.size() + 8, sys.typeArray());
        type[System::TYPE_FIELD_METHODS] =
                mm.makeObject(selectors.size() + 8, sys.typeArray());
        type[System::TYPE_FIELD_TALLY] = 0;
        for (SmallInteger i = 0; i < selectors.size(); i++) {
            if (selectors[i] != Nil::NIL) {
                addMethod(type, selectors[i], methods[i]);
            }
        }
    }

    ObjectPointer Methods::createMethod(MethodHeader header,
                                        ObjectPointer type, ObjectPointer selector,
                                        const std::vector<ObjectPointer>& literals,
                                        const std::vector<uint8_t>& byteCodes) {
        auto method = mm.makeObject(System::COMPILED_METHOD_SIZE + (SmallInteger) literals.size(),
                                    sys.typeCompiledMethod());
        method[System::COMPILED_METHOD_FIELD_HEADER] = header.value();
        method[System::COMPILED_METHOD_FIELD_OWNER] = type;
        method[System::COMPILED_METHOD_FIELD_SELECTOR] = selector;

        SmallInteger literalIndex = System::COMPILED_METHOD_FIELD_LITERALS_START;
        for (auto literal : literals) {
            method[literalIndex++] = literal;
        }

        if (!byteCodes.empty()) {
            auto bytes = mm.makeBuffer((SmallInteger) byteCodes.size(), sys.typeByteArray());
            method[System::COMPILED_METHOD_FIELD_OPCODES] = bytes;
            bytes.loadFrom(byteCodes.data(), byteCodes.size());
        }

        return method;
    }


}

