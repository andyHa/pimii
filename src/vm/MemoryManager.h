//
// Created by Andreas Haufler on 25.11.18.
//

#ifndef MEM_MEMORYMANAGER_H
#define MEM_MEMORYMANAGER_H

#include "ObjectPointer.h"
#include <list>
#include <vector>
#include <deque>

namespace pimii {

    class Segment {
        Word *basePointer;
        Word *allocationPointer;
        Word *endPointer;
        Word size;
        std::shared_ptr<Segment> previous;
    public:
        explicit Segment(Word sizeOfPool, std::shared_ptr<Segment> previous) : size(sizeOfPool),
                                                                               previous(std::move(previous)) {
            basePointer = (Word *) calloc(size, sizeof(Word));
            endPointer = basePointer + size;
            allocationPointer = basePointer;
            memset(allocationPointer, 0, (endPointer - allocationPointer) * sizeof(Word));
        }

        ~Segment() {
            free(basePointer);
        }

        Word sizeInWord() {
            return size;
        }

        Word *alloc(size_t numberOfWords) {
            if (allocationPointer + numberOfWords < endPointer) {
                Word *result = allocationPointer;
                allocationPointer += numberOfWords;
                return result;
            } else {
                return nullptr;
            }
        }

        const std::shared_ptr<Segment> &previousSegment() {
            return previous;
        }

    };

    class Allocator {
        std::shared_ptr<Segment> currentSegment;
        Word allocated;
        Word used;
        Word counter;

        static const inline Word SEGMENT_SIZE = 400000;

        Word *allocateInNewSegment(size_t numberOfWords) {
            currentSegment = std::make_shared<Segment>(SEGMENT_SIZE, currentSegment);
            allocated += SEGMENT_SIZE;
            used += numberOfWords;
            counter++;

            return currentSegment->alloc(numberOfWords);
        }

    public:
        Allocator() : currentSegment(nullptr), allocated(0), used(0), counter(0) {}

        Word objectCount() const {
            return counter;
        }

        Word allocatedWords() const {
            return allocated;
        }

        Word usedWords() const {
            return used;
        }

        Word *alloc(size_t numberOfWords) {
            if (currentSegment != nullptr) {
                Word *result = currentSegment->alloc(numberOfWords);
                if (result != nullptr) {
                    used += numberOfWords;
                    counter++;
                    return result;
                }
            }

            return allocateInNewSegment(numberOfWords);
        }

        const std::shared_ptr<Segment> &lastSegment() {
            return currentSegment;
        }
    };

    class MemoryManager;

    class GarbageCollector {
        std::deque<ObjectPointer> work;
        MemoryManager &mm;

        bool collectBuffers;

        static const inline char STATE_ORIGINAL = 0;
        static const inline char STATE_PARTIALLY_MOVED = 1;
        static const inline char STATE_FULLY_MOVED = 2;
        static const inline char STATE_ROOT = 3;


        ObjectPointer copyObject(ObjectPointer obj);

        ObjectPointer copyBuffer(ObjectPointer buffer);

        void translateObject(ObjectPointer obj);

        ObjectPointer translateField(ObjectPointer field);

    public:
        GarbageCollector(MemoryManager &mm, bool collectBuffers) : mm(mm), collectBuffers(collectBuffers) {}

        void run();
    };

    class MemoryManager {
        std::unique_ptr<Allocator> rootAllocator;
        std::list<ObjectPointer> rootObjects;
        std::unique_ptr<Allocator> activeObjects;
        std::unique_ptr<Allocator> buffers;
        static const inline Word LARGE_OBJECT_SIZE = 100000;

    public:
        std::vector<ObjectPointer> seen;
        MemoryManager() : rootAllocator(std::make_unique<Allocator>()),
                          activeObjects(std::make_unique<Allocator>()),
                          buffers(std::make_unique<Allocator>()) {};

        const std::list<ObjectPointer> &roots() {
            return rootObjects;
        }

        void gc();

        ObjectPointer makeRootObject(Offset numberOfFields, ObjectPointer type);

        inline ObjectPointer makeObject(Offset numberOfFields, ObjectPointer type) {
            auto *buffer = activeObjects->alloc(numberOfFields + 2);
            if (buffer == nullptr) {
                //TODO heap overflow
                throw std::bad_alloc();
            }

            ObjectPointer result = ObjectPointer(buffer, type, numberOfFields);
            seen.push_back(result);
            return result;
        }

        ObjectPointer makeBuffer(Offset numberOfBytes, ObjectPointer type);

        ObjectPointer makeString(std::string_view string, ObjectPointer type);
    };

}

#endif //MEM_MEMORYMANAGER_H
