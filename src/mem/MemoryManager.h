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
        static Word* basePointer;
        static Word rootAllocationIndex;
        static Word rootEndIndex;
        static Word allocationIndex;
        static Word endIndex;
        static Word size;
        /*
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


        static constexpr char STATE_ORIGINAL = 0;
        static constexpr char STATE_PARTIALLY_MOVED = 1;
        static constexpr char STATE_FULLY_MOVED = 2;
        static constexpr char STATE_ROOT = 3;

        void gc();

        ObjectPointer copyObject(ObjectPointer obj, std::deque<ObjectPointer>& gcWork);

        ObjectPointer copyBuffer(ObjectPointer buffer, std::deque<ObjectPointer>& gcWork);

        void translateObject(ObjectPointer obj, std::deque<ObjectPointer>& gcWork);

        ObjectPointer translateField(ObjectPointer field, std::deque<ObjectPointer>& gcWork);
*/
    public:

        static void initialize(std::string imageFileName);

   //     bool shouldIdleGC();

    //    void idleGC();

        bool shouldRunRecommendedGC() {
            return false;
        }

        void runRecommendedGC() {
        }

        ObjectPointer makeRootObject(SmallInteger numberOfFields, ObjectPointer type);

        inline ObjectPointer makeObject(SmallInteger numberOfFields, ObjectPointer type) {
            if (numberOfFields < 0) {
                throw std::runtime_error("Cannot allocate negative memory.");
            }

            if (allocationIndex + numberOfFields + 2 < endIndex) {
                Word result = allocationIndex;
                allocationIndex += numberOfFields + 2;
                return {result, type, numberOfFields};
            } else {
                throw std::runtime_error("Running out of memory!");
            }
        }

        ObjectPointer makeBuffer(SmallInteger numberOfBytes, ObjectPointer type);

        ObjectPointer makeString(std::string_view string, ObjectPointer type);

    };

}

#endif //MEM_MEMORYMANAGER_H
