//
// Created by Andreas Haufler on 25.11.18.
//

#ifndef MEM_MEMORYMANAGER_H
#define MEM_MEMORYMANAGER_H

#include "ObjectPointer.h"

namespace pimii {

    class MemoryManager {
    public:
        Object *allocObject(Offset numberOfFields, ObjectPointer type);

        WordBuffer *allocWords(Offset numberOfWords, ObjectPointer type);

        ByteBuffer *allocBytes(Offset numberOfBytes, ObjectPointer type);
    };

}

#endif //MEM_MEMORYMANAGER_H
