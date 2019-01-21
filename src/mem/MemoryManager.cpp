//
// Created by Andreas Haufler on 25.11.18.
//

#include <deque>
#include <iostream>
#include <fstream>
#include <map>

#include "MemoryManager.h"

namespace pimii {

    Word* MemoryManager::basePointer;
    Word MemoryManager::rootAllocationIndex;
    Word MemoryManager::rootEndIndex;
    Word MemoryManager::allocationIndex;
    Word MemoryManager::endIndex;
    Word MemoryManager::size;
    Word* ObjectPointer::baseAddress;


    void MemoryManager::initialize(std::string imageFileName) {
        /*
        std::ifstream infile(imageFileName);
        bool keepReading = true;
        std::string line;
        std::map<std::string, Word> metas;
        while (keepReading && std::getline(infile, line)) {
            if (line == "image:") {
                keepReading = false;
            } else {
                size_t split_pos;
                if ((split_pos = line.find (':', 0)) != std::string::npos) {
                    metas[line.substr(0,split_pos)] = std::atol(line.substr(split_pos).c_str());
                }
            }
        }

        basePointer = static_cast<Word*>(calloc(metas["heap_size"], sizeof(Word)));
        memset(basePointer, 0, metas["heap_size"] * sizeof(Word));
        size = metas["heap_size"];
        rootAllocationIndex = 1;
        rootEndIndex = metas["root_end_index"];
        allocationIndex = rootEndIndex;
        endIndex = size;
*/
        basePointer = static_cast<Word*>(calloc(16000000, sizeof(Word)));
        memset(basePointer, 0, 16000000 * sizeof(Word));
        size = 16000000;
        rootAllocationIndex = 1;
        rootEndIndex = 2048;
        allocationIndex = rootEndIndex;
        endIndex = size;
        ObjectPointer::baseAddress = basePointer;
    }

    ObjectPointer MemoryManager::makeRootObject(SmallInteger numberOfFields, ObjectPointer type) {
        if (numberOfFields < 0) {
            throw std::runtime_error("Cannot allocate negative memory.");
        }

        if (rootAllocationIndex + numberOfFields + 2 < rootEndIndex) {
            Word result = rootAllocationIndex;
            rootAllocationIndex += numberOfFields + 2;
            if ((result << 2) == 324) {
                std::cout << result << std::endl;
            }
            return {result, type, numberOfFields};
        } else {
            throw std::runtime_error("Running out of root memory!");
        }
    }

    ObjectPointer MemoryManager::makeBuffer(SmallInteger numberOfBytes, ObjectPointer type) {
        SmallInteger numberOfWords = numberOfBytes / sizeof(Word);
        SmallInteger odd = sizeof(Word) - (numberOfBytes % sizeof(Word));
        if (odd != 0) {
            numberOfWords++;
        }

        if (allocationIndex + numberOfWords + 2 < endIndex) {
            Word result = allocationIndex;
            allocationIndex += numberOfWords + 2;

            ObjectPointer op = {result, type, numberOfWords, odd};
            assert(op.isBuffer());
            return op;
        } else {
            throw std::runtime_error("Running out of memory!");
        }
    }

    ObjectPointer MemoryManager::makeString(std::string_view string, ObjectPointer type) {
        auto byteLength = SmallIntegers::toSmallInteger(string.size() + 1);
        ObjectPointer obj = makeBuffer(byteLength, type);
        obj.loadFrom(string.data(), byteLength);
        return obj;
    }
//
//    void MemoryManager::gc() {
//        std::deque<ObjectPointer> gcWork;
//        std::unique_ptr<Allocator> garbageBuffers = std::move(buffers);
//        std::unique_ptr<Allocator> garbageObjects = std::move(activeObjects);
//
//        activeObjects = std::make_unique<Allocator>(maxSegmentsPerPool);
//        buffers = std::make_unique<Allocator>(maxSegmentsPerPool);
//
//        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
//        for (auto root : rootObjects) {
//            translateObject(root, gcWork);
//        }
//
//        int objectsMoved = 0;
//        while (!gcWork.empty()) {
//            ObjectPointer obj = gcWork.front();
//            gcWork.pop_front();
//
//            translateObject(obj, gcWork);
//            objectsMoved++;
//        }
//
//        objectsLowWaterMark =
//                std::min(objectsHighWaterMark,
//                         activeObjects->numberOfSegments() +
//                         (maxSegmentsPerPool - activeObjects->numberOfSegments()) / 3);
//        buffersLowWaterMark =
//                std::min(buffersHighWaterMark,
//                         activeObjects->numberOfSegments() + (maxSegmentsPerPool - buffers->allocatedWords()) / 3);
//
//        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
//
//        gcDurationMicros += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
//        gcCounter++;
//    }
//
//
//    ObjectPointer MemoryManager::copyObject(ObjectPointer obj, std::deque<ObjectPointer>& gcWork) {
//        obj.gcInfo(STATE_FULLY_MOVED);
//        ObjectPointer copy = makeObject(obj.size(), obj.type());
//        obj.transferFieldsTo(0, copy, 0, obj.size());
//        copy.gcInfo(STATE_PARTIALLY_MOVED);
//        obj.gcSuccessor(copy);
//        gcWork.push_back(copy);
//
//        return copy;
//    }
//
//    ObjectPointer MemoryManager::copyBuffer(ObjectPointer buffer, std::deque<ObjectPointer>& gcWork) {
//        buffer.gcInfo(STATE_FULLY_MOVED);
//        ObjectPointer copy = makeBuffer(buffer.byteSize(), translateField(buffer.type(), gcWork));
//        buffer.transferBytesTo(0, copy, 0, buffer.byteSize());
//        copy.gcInfo(STATE_ORIGINAL);
//
//        buffer.gcSuccessor(copy);
//
//        return copy;
//    }
//
//    void MemoryManager::translateObject(ObjectPointer obj, std::deque<ObjectPointer>& gcWork) {
//        if (obj.gcInfo() == STATE_PARTIALLY_MOVED) {
//            obj.gcInfo(STATE_ORIGINAL);
//        }
//
//        obj.type(translateField(obj.type(), gcWork));
//        for (SmallInteger i = 0; i < obj.size(); i++) {
//            obj[i] = translateField(obj[i], gcWork);
//        }
//    }
//
//    ObjectPointer MemoryManager::translateField(ObjectPointer field, std::deque<ObjectPointer>& gcWork) {
//        if (field.isSmallInt() || field.isDecimal() || field == Nil::NIL) {
//            return field;
//        }
//
//        char gcInfo = field.gcInfo();
//        if (gcInfo == STATE_ORIGINAL) {
//            if (field.isObject()) {
//                return copyObject(field, gcWork);
//            } else if (field.isBuffer()) {
//                return copyBuffer(field, gcWork);
//            }
//        } else if (gcInfo == STATE_PARTIALLY_MOVED || gcInfo == STATE_FULLY_MOVED) {
//            return field.gcSuccessor();
//        } else if (gcInfo == STATE_ROOT) {
//            return field;
//        }
//
//        throw std::bad_alloc();
//    }
//
//    bool MemoryManager::shouldIdleGC() {
//        return activeObjects->allocatedWords() > objectsLowWaterMark || buffers->allocatedWords() > buffersLowWaterMark;
//    }
//
//    void MemoryManager::idleGC() {
//       // gc();
//    }

}
