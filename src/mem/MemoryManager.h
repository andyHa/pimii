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
#include <iostream>

namespace pimii {

    class MemoryManager {
        SmallInteger maxSegmentsPerPool;
        SmallInteger buffersLowWaterMark;
        SmallInteger buffersHighWaterMark;
        SmallInteger objectsLowWaterMark;
        SmallInteger objectsHighWaterMark;
        std::unique_ptr<Allocator> rootAllocator;
        std::list<ObjectPointer> rootObjects;
        std::unique_ptr<Allocator> activeObjects;
        std::unique_ptr<Allocator> buffers;

        SmallInteger gcDurationMicros;
        SmallInteger gcCounter;

        bool collectBuffers;
        std::deque<ObjectPointer> gcWork;

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
        MemoryManager(Word maxSegmentsPerPool) : maxSegmentsPerPool(maxSegmentsPerPool),
                                                 rootAllocator(std::make_unique<Allocator>(maxSegmentsPerPool)),
                                                 activeObjects(std::make_unique<Allocator>(maxSegmentsPerPool)),
                                                 buffers(std::make_unique<Allocator>(maxSegmentsPerPool)) {
            objectsHighWaterMark = maxSegmentsPerPool - 2;
            buffersHighWaterMark = objectsHighWaterMark;
        };


        bool shouldIdleGC();

        void idleGC();

        bool shouldRunRecommendedGC() {
            return activeObjects->numberOfSegments() >= objectsHighWaterMark ||
                   buffers->numberOfSegments() >= buffersHighWaterMark;
        }

        void runRecommendedGC() {
            gc(buffers->numberOfSegments() > buffersHighWaterMark);
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

        SmallInteger lowWatermarkForBuffers() {
            return buffersLowWaterMark;
        }

        SmallInteger highWatermarkForBuffers() {
            return buffersHighWaterMark;
        }

        SmallInteger lowWatermarkForObjects() {
            return objectsLowWaterMark;
        }

        SmallInteger highWatermarkForObjects() {
            return buffersHighWaterMark;
        }

        SmallInteger gcMicros() {
            return gcDurationMicros;
        }

        SmallInteger gcCount() {
            return gcCounter;
        }

        SmallInteger resetGCCounter() {
            gcDurationMicros = 0;
            gcCounter = 0;
        }
    };

}

#endif //MEM_MEMORYMANAGER_H
