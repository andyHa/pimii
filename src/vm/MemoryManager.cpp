//
// Created by Andreas Haufler on 25.11.18.
//

#include "MemoryManager.h"

namespace pimii {

    Object *MemoryManager::allocObject(Offset numberOfFields, ObjectPointer type) {
        auto result = (Object *) malloc(sizeof(Offset) + sizeof(ObjectPointer) * (numberOfFields + 1));
        memset(result, 0, sizeof(Offset) + sizeof(ObjectPointer) * (numberOfFields + 1));
        result->size = numberOfFields;
        result->type = type;
        return result;
    }

    WordBuffer *MemoryManager::allocWords(Offset numberOfWords, ObjectPointer type) {
        auto result = (WordBuffer *) malloc(sizeof(Offset) + sizeof(ObjectPointer) + sizeof(Word) * numberOfWords);
        result->size = numberOfWords;
        result->type = type;
        return result;
    }

    ByteBuffer *MemoryManager::allocBytes(Offset numberOfBytes, ObjectPointer type) {
        Offset numberOfWords = numberOfBytes / sizeof(Word);
        Offset odd = numberOfBytes % sizeof(Word);
        if (odd > 0) {
            numberOfWords++;
        }

        auto result = (ByteBuffer *) malloc(
                sizeof(Offset) + sizeof(ObjectPointer) + sizeof(Offset) + sizeof(Word) * numberOfWords);
        result->type = type;
        result->size = numberOfWords;
        result->odd = odd == 0 ? 0 : sizeof(Word) - odd;
        return result;
    }

}
