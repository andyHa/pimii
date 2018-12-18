//
// Created by Andreas Haufler on 25.11.18.
//

#include <deque>
#include <iostream>

#include "MemoryManager.h"

namespace pimii {

    ObjectPointer MemoryManager::makeRootObject(SmallInteger numberOfFields, ObjectPointer type) {
        auto* buffer = rootAllocator->alloc(numberOfFields + 2);
        if (buffer == nullptr) {
            throw std::runtime_error("Overflow of heap space: root");
        }

        ObjectPointer result = ObjectPointer(buffer, type, numberOfFields);
        result.gcInfo(3);
        rootObjects.push_back(result);
        return result;
    }

    ObjectPointer MemoryManager::makeBuffer(SmallInteger numberOfBytes, ObjectPointer type) {
        SmallInteger numberOfWords = numberOfBytes / sizeof(Word);
        SmallInteger odd = sizeof(Word) - (numberOfBytes % sizeof(Word));
        if (odd != 0) {
            numberOfWords++;
        }

        auto* buffer = buffers->alloc(numberOfWords + 2);
        if (buffer == nullptr) {
            throw std::runtime_error("Overflow of heap space: buffers");
        }

        return ObjectPointer(buffer, type, numberOfWords, odd);
    }

    ObjectPointer MemoryManager::makeString(std::string_view string, ObjectPointer type) {
        //TODO size check
        auto byteLength = (SmallInteger) (string.size() + 1);
        ObjectPointer obj = makeBuffer(byteLength, type);

        obj.loadFrom(string.data(), byteLength);
        return obj;
    }

    void MemoryManager::gc(bool shouldCollectBuffers) {
        collectBuffers = shouldCollectBuffers;
        collectBuffers = true;
        std::cout << "Objects-Size: " << activeObjects->usedWords() << std::endl;
        std::cout << "Objects-Count: " << activeObjects->objectCount() << std::endl;
        std::cout << "Buffers-Size: " << buffers->usedWords() << std::endl;
        std::cout << "Buffers-Count: " << buffers->objectCount() << std::endl;
        std::cout << "Root-Size: " << rootAllocator->usedWords() << std::endl;
        std::cout << "Root-Count: " << rootAllocator->objectCount() << std::endl;
        std::unique_ptr<Allocator> garbageObjects = std::move(activeObjects);
        activeObjects = std::make_unique<Allocator>(memoryLimitPerPool);

        std::unique_ptr<Allocator> garbageBuffers = std::move(buffers);
        buffers = std::make_unique<Allocator>(memoryLimitPerPool);

        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        for (auto root : rootObjects) {
            translateObject(root);
        }

        int objectsMoved = 0;
        while (!gcWork.empty()) {
            ObjectPointer obj = gcWork.front();
            gcWork.pop_front();

            translateObject(obj);
            objectsMoved++;
        }

        activeObjects->resetGCRecommendation();
        buffers->resetGCRecommendation();

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        lastGCDurationMicros = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
        baselineCounter = totalObjects();
        baselineAllocatedWords =
                rootAllocator->allocatedWords() + activeObjects->allocatedWords() + buffers->allocatedWords();
        lastGC = std::chrono::steady_clock::now();
        std::cout << "Took: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()
                  << "us, Moved: " << objectsMoved
                  << std::endl;

        std::cout << "Objects-Size: " << activeObjects->usedWords() << std::endl;
        std::cout << "Objects-Count: " << activeObjects->objectCount() << std::endl;
        std::cout << "Buffers-Size: " << buffers->usedWords() << std::endl;
        std::cout << "Buffers-Count: " << buffers->objectCount() << std::endl;
        std::cout << "Root-Size: " << rootAllocator->usedWords() << std::endl;
        std::cout << "Root-Count: " << rootAllocator->objectCount() << std::endl;
    }


    ObjectPointer MemoryManager::copyObject(ObjectPointer obj) {
        obj.gcInfo(STATE_FULLY_MOVED);
        ObjectPointer copy = makeObject(obj.size(), obj.type());
        obj.transferFieldsTo(0, copy, 0, obj.size());
        copy.gcInfo(STATE_PARTIALLY_MOVED);
        obj.gcSuccessor(copy);
        gcWork.push_back(copy);

        return copy;
    }

    ObjectPointer MemoryManager::copyBuffer(ObjectPointer buffer) {
      //  if (buffer.hash() == 0) {
            std::cout << "hit" << std::endl;
   //     }
        buffer.gcInfo(STATE_FULLY_MOVED);

        if (!collectBuffers) {
            buffer.type(translateField(buffer.type()));
            return buffer;
        }

        ObjectPointer copy = makeBuffer(buffer.byteSize(), translateField(buffer.type()));
        buffer.transferBytesTo(0, copy, 0, buffer.byteSize());
        copy.gcInfo(STATE_ORIGINAL);

        buffer.gcSuccessor(copy);

        return copy;
    }

    void MemoryManager::translateObject(ObjectPointer obj) {
        if (obj.gcInfo() == STATE_PARTIALLY_MOVED) {
            obj.gcInfo(STATE_ORIGINAL);
        }

        obj.type(translateField(obj.type()));
        for (SmallInteger i = 0; i < obj.size(); i++) {
            obj[i] = translateField(obj[i]);
        }
    }

    ObjectPointer MemoryManager::translateField(ObjectPointer field) {
        if (field.isSmallInt() || field.isDecimal() || field == Nil::NIL) {
            return field;
        }

        char gcInfo = field.gcInfo();
        if (gcInfo == STATE_ORIGINAL) {
            if (field.isObject()) {
                return copyObject(field);
            } else if (field.isBuffer()) {
                return copyBuffer(field);
            }
        } else if (gcInfo == STATE_PARTIALLY_MOVED || gcInfo == STATE_FULLY_MOVED) {
            return field.gcSuccessor();
        } else if (gcInfo == STATE_ROOT) {
            return field;
        }

        throw std::bad_alloc();
    }

    bool MemoryManager::shouldIdleGC() {
        SmallInteger millisSinceLastGC = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - lastGC).count();
        if (millisSinceLastGC < 1000) {
            return false;
        }

        return totalObjects() - baselineCounter >= 1000;

    }

    void MemoryManager::idleGC() {
        gc(true);
    }
}
