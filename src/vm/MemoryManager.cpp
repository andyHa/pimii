//
// Created by Andreas Haufler on 25.11.18.
//

#include <deque>
#include "MemoryManager.h"

namespace pimii {

    ObjectPointer MemoryManager::makeRootObject(Offset numberOfFields, ObjectPointer type) {
        auto *buffer = rootAllocator->alloc(numberOfFields + 2);
        if (buffer == nullptr) {
            //TODO heap overflow
            throw std::bad_alloc();
        }

        ObjectPointer result = ObjectPointer(buffer, type, numberOfFields);
        rootObjects.push_back(result);
        return result;
    }

    ObjectPointer MemoryManager::makeBuffer(Offset numberOfBytes, ObjectPointer type) {
        Offset numberOfWords = numberOfBytes / sizeof(Word);
        Offset odd = sizeof(Word) - (numberOfBytes % sizeof(Word));
        if (odd != 0) {
            numberOfWords++;
        }

        auto *buffer = buffers->alloc(numberOfWords + 2);
        if (buffer == nullptr) {
            //TODO heap overflow
            throw std::bad_alloc();
        }

        return ObjectPointer(buffer, type, numberOfWords, odd);
    }

    ObjectPointer MemoryManager::makeString(std::string_view string, ObjectPointer type) {
        //TODO size check
        auto byteLength = (Offset) (string.size() + 1);
        ObjectPointer obj = makeBuffer(byteLength, type);

        obj.loadFrom(string.data(), byteLength);
        return obj;
    }

    void MemoryManager::gc() {
        std::cout << "Objects: " << activeObjects->usedWords() << std::endl;
        std::cout << "Buffers: " << buffers->usedWords() << std::endl;
        std::unique_ptr<Allocator> garbageObjects = std::move(activeObjects);
        std::unique_ptr<Allocator> garbageBuffers = std::move(buffers);
        activeObjects = std::make_unique<Allocator>();
        buffers = std::make_unique<Allocator>();

        GarbageCollector collector(*this);
        collector.run();
        std::cout << "Objects: " << activeObjects->usedWords() << std::endl;
        std::cout << "Buffers: " << buffers->usedWords() << std::endl;
    }

    void GarbageCollector::run() {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        for (auto root : mm.roots()) {
            root.gcInfo(STATE_ROOT);
            translateObject(root);
        }

        int objectsMoved = 0;
        while (!work.empty()) {
            ObjectPointer obj = work.front();
            work.pop_front();
            translateObject(obj);
            objectsMoved++;
        }

        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Took: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "us, Moved: " << objectsMoved
                  << std::endl;
    }

    ObjectPointer GarbageCollector::copyObject(ObjectPointer obj) {
        obj.gcInfo(STATE_FULLY_MOVED);
        ObjectPointer copy = mm.makeObject(obj.size(), obj.type());
        obj.transferFieldsTo(0, copy, 0, obj.size());
        copy.gcInfo(STATE_PARTIALLY_MOVED);
        obj.gcSuccessor(copy);

        work.push_back(copy);

        return copy;
    }

    ObjectPointer GarbageCollector::copyBuffer(ObjectPointer buffer) {
        buffer.gcInfo(STATE_FULLY_MOVED);

        ObjectPointer copy = mm.makeBuffer(buffer.byteSize(), translateField(buffer.type()));
        buffer.transferBytesTo(copy, buffer.byteSize());
        copy.gcInfo(STATE_FULLY_MOVED);

        buffer.gcSuccessor(copy);

        return copy;
    }

    void GarbageCollector::translateObject(ObjectPointer obj) {
        if (obj.gcInfo() == STATE_PARTIALLY_MOVED) {
            obj.gcInfo(STATE_FULLY_MOVED);
        }

        obj.type(translateField(obj.type()));
        for (Offset i = 0; i < obj.size(); i++) {
            obj[i] = translateField(obj[i]);
        }
    }

    ObjectPointer GarbageCollector::translateField(ObjectPointer field) {
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
}
