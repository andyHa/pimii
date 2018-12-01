//
// Created by Andreas Haufler on 25.11.18.
//

#include "MemoryManager.h"

namespace pimii {


    ByteBuffer *MemoryManager::allocBytes(Offset numberOfBytes, ObjectPointer type) {
        Offset numberOfWords = numberOfBytes / sizeof(Word);
        Offset odd = numberOfBytes % sizeof(Word);
        if (odd > 0) {
            numberOfWords++;
        }

        auto data = malloc(
                sizeof(Word) + sizeof(ObjectPointer) + sizeof(Word) + sizeof(Word) * numberOfWords);
        memset(data, 0, sizeof(Word) + sizeof(ObjectPointer) + sizeof(Word) + sizeof(Word) * numberOfWords);
        auto result = (ByteBuffer *) data;
        result->type = type;
        result->size = numberOfWords;
        result->odd = odd == 0 ? 0 : sizeof(Word) - odd;

        return result;
    }

}
