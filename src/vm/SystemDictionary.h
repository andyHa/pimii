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
        Object *dictionary;

        Object *atPut(ObjectPointer key, ObjectPointer value, bool force);

        void grow(Object *table);

        void reInsert(Object *table, ObjectPointer association);

        Object *tryInsert(Offset index, Object *table, ObjectPointer key, ObjectPointer value, bool force);

    public:
        static const Offset DICTIONARY_SIZE;
        static const Offset DICTIONARY_FIELD_TALLY;
        static const Offset DICTIONARY_FIELD_TABLE;
        static const Offset ASSOCIATION_SIZE;
        static const Offset ASSOCIATION_FIELD_KEY;
        static const Offset ASSOCIATION_FIELD_VALUE;

        SystemDictionary(MemoryManager &mm);

        void installTypes(ObjectPointer systemDictionaryType, ObjectPointer arrayType, ObjectPointer associationType);


        Object *atPut(ObjectPointer key, ObjectPointer value);

        Object *at(ObjectPointer key);

        ObjectPointer getValue(ObjectPointer key);
    };
}


#endif //MEM_SYSTEMDICTIONARY_H
