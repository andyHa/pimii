//
// Created by Andreas Haufler on 26.11.18.
//

#ifndef MEM_METHODS_H
#define MEM_METHODS_H

#include "MemoryManager.h"
#include "TypeSystem.h"

namespace pimii {

    class Methods {

        MemoryManager &mm;
        TypeSystem &types;

        bool tryInsert(ObjectPointer type, Offset index, Object *selectors,
                       Object *methods,
                       ObjectPointer selector, ObjectPointer method);

        void grow(ObjectPointer type, Object *selectors, Object *methods);

        ObjectPointer createHeaderOnlyMethod(SmallInteger headerValue);

    public:
        Methods(MemoryManager &mm, TypeSystem &types);

        void addMethod(ObjectPointer type, ObjectPointer selector,
                       ObjectPointer compiledMethod);

        ObjectPointer createMethod(Offset numberOfTemporaries,
                                   const std::vector<ObjectPointer> &literals,
                                   const std::vector<uint8_t> &byteCodes);

        ObjectPointer createPrimitiveMethod(Offset primitiveIndex);

        ObjectPointer createReturnFieldMethod(Offset fieldIndex);

        ObjectPointer createPopAndStoreFieldMethod(Offset fieldIndex);
    };

}

#endif //MEM_METHODS_H
