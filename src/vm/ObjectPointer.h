//
// Created by Andreas Haufler on 23.11.18.
//

#ifndef MEM_OBJECTPOINTER_H
#define MEM_OBJECTPOINTER_H

#include <string>

#include "../common/types.h"

#define PIMII_ENABLE_CHECKS

namespace pimii {

    constexpr Word sizeMask() {
        Word mask = 0x00;
        for (int i = 0; i < usableSizeBytes(); i++) {
            mask = mask << 8;
            mask |= 0xFF;
        }

        return mask;
    }

    constexpr Word clearGCMask() {
        Word mask = 0x0F;
        for (int i = 0; i < usableSizeBytes(); i++) {
            mask = mask << 8;
            mask |= 0xFF;
        }

        return mask;
    }


    class ObjectPointer {

        enum ObjectPointerType : Word {
            OBJECT = 0,
            SMALL_INT = 1,
            BUFFER = 2,
            DECIMAL = 3
        };

        struct Object {
            Word size;
            Word type;
            Word fields[];
        };

        Word data;

        inline ObjectPointerType getObjectPointerType() const noexcept {
            return (ObjectPointerType) (data & TYPE_MASK);
        }

        inline Object *unmask() const {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != BUFFER && getObjectPointerType() != OBJECT) {
                throw std::bad_cast();
            }
#endif
            return (Object *) (data & ~TYPE_MASK);
        }

        inline Object *object() const {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != OBJECT) {
                throw std::bad_cast();
            }
#endif
            return unmask();
        }

        inline Object *buffer() const {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != BUFFER) {
                throw std::bad_cast();
            }
#endif
            return unmask();
        }

        constexpr static Word TYPE_MASK = 0b11;
        constexpr static Word ODD_MASK = 0x0F;
        constexpr static Word GC_MASK = 0xF0;

        Word highNibble() const {
            return unmask()->size >> (usableSizeBytes() * 8);
        }

        explicit ObjectPointer(Word data) noexcept : data(data) {};
    public:
        ObjectPointer() noexcept : data(0) {}

        ObjectPointer(const ObjectPointer &other) noexcept = default;

        static ObjectPointer forObject(Word data) noexcept {
            return ObjectPointer(data);
        }

        static ObjectPointer forSmallInt(SmallInteger intValue) noexcept {
            return ObjectPointer(((Word) (intValue) << 2) | ObjectPointerType::SMALL_INT);
        }

        static ObjectPointer forDecimal(Decimal floatValue) noexcept {
            return ObjectPointer(((Word) (floatValue) << 2) | ObjectPointerType::DECIMAL);
        }

        ObjectPointer(const void *object, ObjectPointer type, SmallInteger numberOfFields) noexcept : data(
                ((Word) object) | OBJECT) {
            unmask()->size = numberOfFields;
            unmask()->type = *reinterpret_cast<Word *>(&type);
        };

        ObjectPointer(const void *object, ObjectPointer type, SmallInteger wordSize, SmallInteger odd) noexcept : data(
                ((Word) object) | BUFFER) {

            unmask()->size = (Word) wordSize | (Word) ((odd & ODD_MASK) << (usableSizeBytes() * 8));
            unmask()->type = *reinterpret_cast<Word *>(&type);
        };

        ObjectPointer type() const {
            return *reinterpret_cast<ObjectPointer *>(&unmask()->type);
        }

        void type(ObjectPointer newType) {
            unmask()->type = *reinterpret_cast<Word *>(&newType);
        }

        ObjectPointer gcSuccessor() const {
            return type();
        }

        void gcSuccessor(ObjectPointer successor) {
            type(successor);
        }

        char gcInfo() const {
            return (char) ((unmask()->size >> (usableSizeBytes() * 8)) & GC_MASK) >> 4;
        }

        void gcInfo(char info) {
            unmask()->size &= clearGCMask();
            unmask()->size |= ((Word) ((info << 4) & GC_MASK)) << (usableSizeBytes() * 8);
        }

        bool isSmallInt() const noexcept {
            return getObjectPointerType() == SMALL_INT;
        }

        SmallInteger smallInt() const {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != SMALL_INT) {
                throw std::bad_cast();
            }
#endif
            return (SmallInteger) (data >> 2);
        }

        inline bool isDecimal() const noexcept {
            return getObjectPointerType() == DECIMAL;
        }

        inline Decimal decimal() const {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != DECIMAL) {
                throw std::bad_cast();
            }
#endif
            return (Decimal) (data >> 2);
        }

        inline bool isObject() const noexcept {
            return getObjectPointerType() == OBJECT;
        }

        inline ObjectPointer &operator[](SmallInteger index) const {
            return *reinterpret_cast<ObjectPointer *>(&object()->fields[index]);
        }

        inline bool isBuffer() const noexcept {
            return getObjectPointerType() == BUFFER;
        }

        inline char fetchByte(SmallInteger index) {
            return *(reinterpret_cast<char *>(&buffer()->fields[0]) + index);
        }

        void loadFrom(const void *src, SmallInteger byteLength) {
            std::memcpy(&(buffer())->fields[0], src, byteLength);
        }

        void storeTo(void *dest, SmallInteger byteLength) {
            std::memcpy(dest, &buffer()->fields[0], byteLength);
        }

        void transferBytesTo(SmallInteger start,ObjectPointer dest, SmallInteger destStart, SmallInteger byteLength) {
            std::memcpy(&dest.buffer()->fields[0] + destStart, &buffer()->fields[0] + start,
                        byteLength);
        }

        void transferFieldsTo(SmallInteger start, ObjectPointer dest, SmallInteger destStart, SmallInteger numberOfFields) {
            std::memcpy(&dest.object()->fields[destStart], &object()->fields[start], numberOfFields * sizeof(Word));
        }

        std::string_view stringView() const {
            return std::string_view(reinterpret_cast<char *>(&buffer()->fields[0]));
        }

        SmallInteger hashString() const noexcept {
            std::hash<std::string_view> fn;
            return (SmallInteger) fn(stringView());
        }

        int compare(const void *other, size_t length) const {
            SmallInteger thisSize = byteSize();
            if (thisSize != length) {
                return (int) (thisSize - length);
            }

            return memcmp(&buffer()->fields[0], other, thisSize);
        }

        int compareTo(ObjectPointer other) const {
            SmallInteger thisSize = byteSize();
            SmallInteger otherSize = other.byteSize();
            if (thisSize != otherSize) {
                return thisSize > otherSize ? 1 : -1;
            }

            return memcmp(&buffer()->fields[0], &other.buffer()->fields[0], thisSize);
        }

        SmallInteger size() {
            return (SmallInteger) (unmask()->size & sizeMask());
        }

        SmallInteger byteSize() const {
            Word wordSize = buffer()->size & sizeMask();
            Word odd = highNibble() & ODD_MASK;

            return (SmallInteger) ((wordSize * sizeof(Word)) - odd);
        }

        inline ObjectPointer &operator=(const ObjectPointer &rhs) noexcept = default;

        inline ObjectPointer &operator=(const SmallInteger &rhs) noexcept {
            data = ((Word) (rhs) << 2) | ObjectPointerType::SMALL_INT;
            return *this;
        }

        inline bool operator==(const ObjectPointer &rhs) const noexcept {
            return data == rhs.data;
        }

        inline bool operator!=(const ObjectPointer &rhs) const noexcept {
            return data != rhs.data;
        }

        inline SmallInteger hash() const noexcept {
            return (SmallInteger) data;
        }

    };

    class Nil {
    public:
        static const inline ObjectPointer NIL;
    };

}


#endif //MEM_OBJECTPOINTER_H
