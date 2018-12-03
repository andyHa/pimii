//
// Created by Andreas Haufler on 23.11.18.
//

#ifndef MEM_OBJECTPOINTER_H
#define MEM_OBJECTPOINTER_H

#include <cinttypes>
#include <typeinfo>
#include <cstring>
#include <string>
#include <iostream>


#define PIMII_ENABLE_CHECKS

namespace pimii {

    typedef unsigned long long Word;
    typedef int_fast32_t SmallInteger;
    typedef float Decimal;
    typedef uint_fast16_t Offset;

    constexpr SmallInteger minSmallInt() {
        return (SmallInteger) ((std::numeric_limits<Word>::min() >> 3) * -1);
    }

    constexpr SmallInteger maxSmallInt() {
        return (SmallInteger) ((std::numeric_limits<Word>::max() >> 3) - 1);
    }

    constexpr Offset maxOffset() {
        return (Offset) (std::numeric_limits<Word>::max() >> 8);
    }

    constexpr Offset usableSizeBytes() {
        return sizeof(Word) - 1;
    }

    constexpr Word sizeMask() {
        Word mask = 0x00;
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

        static inline const Word TYPE_MASK = 0b11;
        static inline const Word ODD_MASK = 0x0F;
        static inline const Word GC_MASK = 0xF0;

        Word highNibble() const {
            return unmask()->size >> (usableSizeBytes() * 8);
        }

    public:

        inline ObjectPointer() noexcept : data(0) {};

        explicit inline ObjectPointer(SmallInteger intValue) noexcept : data(
                ((Word) (intValue) << 2) | ObjectPointerType::SMALL_INT) {};

        explicit inline ObjectPointer(Decimal floatValue) noexcept : data(
                ((Word) (floatValue) << 2) | ObjectPointerType::DECIMAL) {};

        inline ObjectPointer(const void *object, ObjectPointer type, Offset numberOfFields) noexcept : data(
                ((Word) object) | OBJECT) {
            unmask()->size = numberOfFields;
            unmask()->type = *reinterpret_cast<Word *>(&type);
        };

        inline ObjectPointer(const void *object, ObjectPointer type, Offset wordSize, Offset odd) noexcept : data(
                ((Word) object) | BUFFER) {

            unmask()->size = (Word) wordSize | (Word) (odd & ODD_MASK) << (usableSizeBytes() * 8);
            unmask()->type = *reinterpret_cast<Word *>(&type);
        };

        inline ObjectPointer(const ObjectPointer &other) noexcept = default;

        inline ObjectPointer type() {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != BUFFER && getObjectPointerType() != OBJECT) {
                throw std::bad_cast();
            }
#endif
            return *reinterpret_cast<ObjectPointer *>(&unmask()->type);
        }

        inline void type(ObjectPointer newType) {
#ifdef PIMII_ENABLE_CHECKS
            if (getObjectPointerType() != BUFFER && getObjectPointerType() != OBJECT) {
                throw std::bad_cast();
            }
#endif
            unmask()->type = *reinterpret_cast<Word *>(&newType);
        }

        inline bool isSmallInt() const noexcept {
            return getObjectPointerType() == SMALL_INT;
        }

        inline SmallInteger smallInt() const {
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

        inline ObjectPointer &operator[](Offset index) const {
            return *reinterpret_cast<ObjectPointer *>(&object()->fields[index]);
        }

        inline bool isBuffer() const noexcept {
            return getObjectPointerType() == BUFFER;
        }

        inline char fetchByte(Offset index) {
            return *(reinterpret_cast<char *>(&buffer()->fields[0]) + index);
        }

        void loadFrom(const void *src, Offset byteLength) {
            std::memcpy(&(buffer())->fields[0], src, byteLength);
        }

        void storeTo(void *dest, Offset byteLength) {
            std::memcpy(dest, &buffer()->fields[0], byteLength);
        }

        void transferBytesTo(ObjectPointer dest, Offset byteLength) {
            std::memcpy(&dest.buffer()->fields[0], &buffer()->fields[0],
                        byteLength);
        }

        void transferFieldsTo(Offset start, ObjectPointer dest, Offset destStart, Offset numberOfFields) {
            std::memcpy(&dest.object()->fields[destStart], &object()->fields[start], numberOfFields * sizeof(Word));
        }

        std::string_view stringView() const {
            return std::string_view(reinterpret_cast<char *>(&buffer()->fields[0]));
        }

        Offset hashString() const noexcept {
            std::hash<std::string_view> fn;
            return (Offset) fn(stringView());
        }

        int compare(const void *other, size_t length) const {
            Offset thisSize = byteSize();
            if (thisSize != length) {
                return (int) (thisSize - length);
            }

            return memcmp(&buffer()->fields[0], other, thisSize);
        }

        int compareTo(ObjectPointer other) const {
            Offset thisSize = byteSize();
            Offset otherSize = other.byteSize();
            if (thisSize != otherSize) {
                return thisSize - otherSize;
            }

            return memcmp(&buffer()->fields[0], &other.buffer()->fields[0], thisSize);
        }

        Offset size() {
            return (Offset) (unmask()->size & sizeMask());
        }

        Offset byteSize() const {
            Word wordSize = buffer()->size & sizeMask();
            Word odd = highNibble() & ODD_MASK;

            return (Offset) ((wordSize * sizeof(Word)) - odd);
        }

        inline ObjectPointer &operator=(const ObjectPointer &rhs) noexcept = default;

        inline ObjectPointer &operator=(const SmallInteger &rhs) noexcept {
            data = ((Word) (rhs) << 2) | ObjectPointerType::SMALL_INT;
            return *this;
        }

        inline ObjectPointer &operator=(const Decimal &rhs) noexcept {
            data = ((Word) (rhs) << 2) | ObjectPointerType::DECIMAL;
            return *this;
        }

        inline bool operator==(const ObjectPointer &rhs) const noexcept {
            return data == rhs.data;
        }

        inline bool operator!=(const ObjectPointer &rhs) const noexcept {
            return data != rhs.data;
        }

        inline Offset hash() const noexcept {
            return (Offset) data;
        }

    };

    class Nil {
    public:
        static const inline ObjectPointer NIL = {};
    };

}


#endif //MEM_OBJECTPOINTER_H
