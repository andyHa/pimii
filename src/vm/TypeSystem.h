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

        void completeType(ObjectPointer type, ObjectPointer superType, const std::string& name);

    public:
        static const Offset TYPE_FIELD_SUPERTYPE;
        static const Offset TYPE_FIELD_NAME;
        static const Offset TYPE_FIELD_NUMBER_OF_FIXED_FIELDS;
        static const Offset TYPE_FIELD_TALLY;
        static const Offset TYPE_FIELD_SELECTORS;
        static const Offset TYPE_FIELD_METHODS;
        static const Offset TYPE_SIZE;

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

        ObjectPointer makeType(ObjectPointer parent,const std::string& name, Offset typeFields);
    };

}


#endif //MEM_TYPESYSTEM_H
