//
// Created by Andreas Haufler on 23.11.18.
//

#ifndef MEM_SYSTEMDICTIONARY_H
#define MEM_SYSTEMDICTIONARY_H

#include "../common/ObjectPointer.h"
#include "../mem/MemoryManager.h"

namespace pimii {

    class SystemDictionary {
        MemoryManager &mm;
        ObjectPointer associationType;
        ObjectPointer dictionary;

        ObjectPointer atPut(ObjectPointer key, ObjectPointer value, bool force);

        void grow(ObjectPointer table);

        void reInsert(ObjectPointer table, ObjectPointer association);

    public:
        static constexpr SmallInteger DICTIONARY_SIZE = 2;
        static constexpr SmallInteger DICTIONARY_FIELD_TALLY = 0;
        static constexpr SmallInteger DICTIONARY_FIELD_TABLE = 1;
        static constexpr SmallInteger ASSOCIATION_SIZE = 2;
        static constexpr SmallInteger ASSOCIATION_FIELD_KEY = 0;
        static constexpr SmallInteger ASSOCIATION_FIELD_VALUE = 1;

        explicit SystemDictionary(MemoryManager &mm);

        void installTypes(ObjectPointer systemDictionaryType, ObjectPointer arrayType, ObjectPointer associationType);

        ObjectPointer atPut(ObjectPointer key, ObjectPointer value);

        ObjectPointer at(ObjectPointer key);

        ObjectPointer getValue(ObjectPointer key);
    };
}


#endif //MEM_SYSTEMDICTIONARY_H
