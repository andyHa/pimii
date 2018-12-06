//
// Created by Andreas Haufler on 23.11.18.
//

#ifndef MEM_SYSTEMDICTIONARY_H
#define MEM_SYSTEMDICTIONARY_H

#include "ObjectPointer.h"
#include "MemoryManager.h"

namespace pimii {

    class SystemDictionary {
        MemoryManager &mm;
        ObjectPointer associationType;
        ObjectPointer dictionary;

        ObjectPointer atPut(ObjectPointer key, ObjectPointer value, bool force);

        void grow(ObjectPointer table);

        void reInsert(ObjectPointer table, ObjectPointer association);

    public:
        static const Offset DICTIONARY_SIZE;
        static const Offset DICTIONARY_FIELD_TALLY;
        static const Offset DICTIONARY_FIELD_TABLE;
        static const Offset ASSOCIATION_SIZE;
        static const Offset ASSOCIATION_FIELD_KEY;
        static const Offset ASSOCIATION_FIELD_VALUE;

        SystemDictionary(MemoryManager &mm);

        void installTypes(ObjectPointer systemDictionaryType, ObjectPointer arrayType, ObjectPointer associationType);


        ObjectPointer atPut(ObjectPointer key, ObjectPointer value);

        ObjectPointer at(ObjectPointer key);

        ObjectPointer getValue(ObjectPointer key);
    };
}


#endif //MEM_SYSTEMDICTIONARY_H
