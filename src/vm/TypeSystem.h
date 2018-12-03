//
// Created by Andreas Haufler on 25.11.18.
//

#ifndef MEM_TYPESYSTEM_H
#define MEM_TYPESYSTEM_H

#include "ObjectPointer.h"
#include "MemoryManager.h"
#include "SymbolTable.h"
#include "SystemDictionary.h"

namespace pimii {


    class TypeSystem {
        MemoryManager &mm;
        SymbolTable &symbols;
        SystemDictionary &systemDictionary;

        void completeType(ObjectPointer type, ObjectPointer superType, const std::string &name);

    public:
        static inline const Offset TYPE_FIELD_SUPERTYPE = 0;
        static inline const Offset TYPE_FIELD_NAME = 1;
        static inline const Offset TYPE_FIELD_NUMBER_OF_FIXED_FIELDS = 2;
        static inline const Offset TYPE_FIELD_FIELD_NAMES = 3;
        static inline const Offset TYPE_FIELD_TALLY = 4;
        static inline const Offset TYPE_FIELD_SELECTORS = 5;
        static inline const Offset TYPE_FIELD_METHODS = 6;
        static inline const Offset TYPE_SIZE = 7;

        TypeSystem(MemoryManager &mm, SymbolTable &symbols, SystemDictionary &systemDictionary);

        const ObjectPointer nilType;
        const ObjectPointer metaClassType;
        const ObjectPointer classType;
        const ObjectPointer objectType;
        const ObjectPointer smallIntType;
        const ObjectPointer symbolType;
        const ObjectPointer stringType;
        const ObjectPointer associationType;
        const ObjectPointer arrayType;
        const ObjectPointer compiledMethodType;
        const ObjectPointer byteArrayType;
        const ObjectPointer methodContextType;
        const ObjectPointer blockContextType;

        ObjectPointer makeType(ObjectPointer parent, const std::string &name, Offset typeFields);
    };

}


#endif //MEM_TYPESYSTEM_H
