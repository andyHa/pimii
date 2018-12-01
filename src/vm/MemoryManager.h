//
// Created by Andreas Haufler on 25.11.18.
//

#ifndef MEM_MEMORYMANAGER_H
#define MEM_MEMORYMANAGER_H

#include "ObjectPointer.h"

namespace pimii {

    class Page {
        Word *basePointer;
        Word *allocationPointer;
        Word *endPointer;
        Word size;
    public:
        explicit Page(Word sizeOfPool) : size(sizeOfPool) {
            basePointer = (Word *) calloc(size, sizeof(Word));
            endPointer = basePointer + size;
            allocationPointer = basePointer;
        }

        ~Page() {
            free(basePointer);
        }

        std::shared_ptr<Page> previous;

        void initialize() {
            memset(allocationPointer, 0, endPointer - allocationPointer);
        }

        inline Word *alloc(size_t numberOfWords) {
            if (allocationPointer + numberOfWords < endPointer) {
                Word *result = allocationPointer;
                allocationPointer += numberOfWords;
                return result;
            } else {
                return nullptr;
            }
        }
    };

    class MemoryManager {
        std::shared_ptr<Page> currentPage;
    public:
        MemoryManager() : currentPage(new Page(16777216 / sizeof(Word))) {};

        inline Object *allocObject(Offset numberOfFields, ObjectPointer type) {
//            if (numberOfFields > 1048576) {
//                std::shared_ptr<Page> page = std::shared_ptr<Page>(new Page(3+numberOfFields));
//                auto result = (Object *) page->alloc(3 + numberOfFields);
//                page->previous = currentPage->previous;
//                currentPage->previous = page;
//                return result;
//            }
//            auto result = (Object *) currentPage->alloc(3 + numberOfFields);
//
//            if (result == nullptr) {
//                std::shared_ptr<Page> page = std::shared_ptr<Page>(new Page(16777216 / sizeof(Word)));
//                result = (Object *) page->alloc(3 + numberOfFields);
//                page->previous = currentPage;
//                currentPage = page;
//                return result;
//            }
//
//            result->size = numberOfFields;
//            result->type = type;


            auto data = malloc(sizeof(Word) + sizeof(ObjectPointer) * (numberOfFields + 1));
            memset(data, 0, sizeof(Word) + sizeof(ObjectPointer) * (numberOfFields + 1));
            auto result = (Object *) data;
            result->size = numberOfFields;
            result->type = type;
            return result;
        }

        ByteBuffer *allocBytes(Offset numberOfBytes, ObjectPointer type);
    };

}

#endif //MEM_MEMORYMANAGER_H
