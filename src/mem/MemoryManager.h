//
// Created by Andreas Haufler on 25.11.18.
//

#ifndef MEM_MEMORYMANAGER_H
#define MEM_MEMORYMANAGER_H

#include "../common/ObjectPointer.h"
#include "Allocator.h"

#include <list>
#include <vector>
#include <deque>
#include <chrono>

namespace pimii {

    class MemoryManager {
        Word memoryLimitPerPool;
        std::unique_ptr<Allocator> rootAllocator;
        std::list<ObjectPointer> rootObjects;
        std::unique_ptr<Allocator> activeObjects;
        std::unique_ptr<Allocator> buffers;

        SmallInteger baselineCounter;
        SmallInteger baselineAllocatedWords;
        SmallInteger lastGCDurationMicros;
        std::chrono::steady_clock::time_point lastGC;

        bool collectBuffers;
        std::deque<ObjectPointer> gcWork;

        static constexpr Word LARGE_OBJECT_SIZE = 100000;

        static constexpr char STATE_ORIGINAL = 0;
        static constexpr char STATE_PARTIALLY_MOVED = 1;
        static constexpr char STATE_FULLY_MOVED = 2;
        static constexpr char STATE_ROOT = 3;

        void gc(bool shoudCollectBuffers);

        ObjectPointer copyObject(ObjectPointer obj);

        ObjectPointer copyBuffer(ObjectPointer buffer);

        void translateObject(ObjectPointer obj);

        ObjectPointer translateField(ObjectPointer field);

    public:
        MemoryManager(Word memoryLimitPerPool) : memoryLimitPerPool(memoryLimitPerPool),
                                                 rootAllocator(std::make_unique<Allocator>(memoryLimitPerPool)),
                                                 activeObjects(std::make_unique<Allocator>(memoryLimitPerPool)),
                                                 buffers(std::make_unique<Allocator>(memoryLimitPerPool)),
                                                 baselineCounter(0), baselineAllocatedWords(0),
                                                 lastGC(std::chrono::steady_clock::now()) {};


        bool shouldIdleGC();

        void idleGC();

        bool shouldRunRecommendedGC() {
            return activeObjects->shouldGC() || buffers->shouldGC();
        }

        void runRecommendedGC() {
            gc(buffers->shouldGC());
        }

        ObjectPointer makeRootObject(SmallInteger numberOfFields, ObjectPointer type);

        inline ObjectPointer makeObject(SmallInteger numberOfFields, ObjectPointer type) {
            if (numberOfFields < 0) {
                throw std::range_error("Cannot allocate negative memory.");
            }

            auto* buffer = activeObjects->alloc(numberOfFields + 2);
            if (buffer == nullptr) {
                throw std::runtime_error("Overflow of heap space: objects");
            }

            return {buffer, type, numberOfFields};
        }

        ObjectPointer makeBuffer(SmallInteger numberOfBytes, ObjectPointer type);

        ObjectPointer makeString(std::string_view string, ObjectPointer type);

        SmallInteger allocatedObjects() {
            return activeObjects->objectCount();
        }

        SmallInteger allocatedRoots() {
            return rootAllocator->objectCount();
        }

        SmallInteger allocatedBuffers() {
            return buffers->objectCount();
        }

        SmallInteger totalObjects() {
            return activeObjects->objectCount() + rootAllocator->objectCount() + buffers->objectCount();
        }

        SmallInteger allocatedObjectWords() {
            return activeObjects->usedWords();
        }

        SmallInteger allocatedRootWords() {
            return rootAllocator->usedWords();
        }

        SmallInteger allocatedBufferWords() {
            return buffers->usedWords();
        }
    };

}

#endif //MEM_MEMORYMANAGER_H
