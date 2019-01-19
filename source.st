Class: Boolean
Superclass: Object

Class: True
Superclass: Boolean

Class: False
Superclass: Boolean

Class: System
Superclass: Object

Class: Behaviour
Superclass: Object
Instance Fields: superclass name fixedFields fields tally selectors methods

Methods: Object
------------------------
asString
    ^self class asString.
------------------------
isNil
    ^false.
------------------------
notNil
    ^true.
------------------------
ifNil: aBlock
    self isNil ifTrue: [ ^aBlock value. ].
    ^self.
------------------------
id
    <Primitive:37>
------------------------
hash
    self id.
------------------------
hashBytes
    <Primitive:22>
------------------------
compareBytesTo: other
    <Primitive:21>
------------------------
== other
    <Primitive:0>
------------------------
= other
    ^self == other.
------------------------
<> other
    ^(self = other) not.
------------------------
at: index
    <Primitive:43>
------------------------
at: index put: value
    <Primitive:44>
------------------------
transferTo: dest index: index destIndex: destIndex length: length
    <Primitive:45>
------------------------
objectAt: index
    <Primitive:34>
------------------------
objectAt: index put: value
    <Primitive:35>
------------------------
objectTransferTo: dest index: index destIndex: destIndex length: length
    <Primitive:36>
------------------------
byteAt: index
    <Primitive:18>
------------------------
byteAt: index put: value
    <Primitive:19>
------------------------
class
    <Primitive:23>
------------------------
size
    <Primitive:38>
------------------------
objectSize
    <Primitive:39>
------------------------
is: aClass
    | currentClass |
    currentClass := self class.
    [ currentClass notNil ] whileTrue: [
        (currentClass = aClass) ifTrue: [ ^true ].
        currentClass := currentClass superclass.
    ].
    ^false.
------------------------
init
    ^self.
------------------------

Methods: Behaviour
------------------------
basicNew
    <Primitive:15>
------------------------
basicNew: size
    <Primitive:16>
------------------------
new
    ^self basicNew init.
------------------------
new: size
    ^(self basicNew: size) init.
------------------------
basicAlloc: size
    <Primitive:17>
------------------------
alloc: size
    ^self basicAlloc: size.
------------------------
name
    ^name.
------------------------
superclass
    ^superclass.
------------------------
asString
    ^name plainString.
------------------------

Methods: Symbol
------------------------
plainString
    | string |
    string := String alloc: self size.
    self transferBytesTo: string index: 1 destIndex: 1 length: self size.
    ^string.
------------------------
asString
    ^'#' , self plainString.
------------------------

Class Methods: Symbol
------------------------
forString: aString
    | symbol |
    symbol := self alloc: aString size.
    aString transferBytesTo: symbol index: 1 destIndex: 1 length: aString size.
    ^symbol.
------------------------

Class: String
Superclass: Object
Class Fields: empty

Class Methods: String
------------------------
empty
    ^empty.
------------------------

Methods: String
------------------------
asString
    ^self.
------------------------
asSymbol
    ^Symbols at: self.
------------------------
length
    ^self size - 1.
------------------------
, other
    | string |
    other := other asString.
    string := String alloc: self length + other length + 1.
    self transferBytesTo: string index: 1 destIndex: 1 length: self length.
    other transferBytesTo: string index: 1 destIndex: self length + 1 length: other length.

    ^string.
------------------------
at: index
    ^Character asChar: (self byteAt: index).
------------------------
at: index put: character
    ^self byteAt: index put: character value.
------------------------
from: start to: end
    | substring |
    end <= start ifTrue: [ ^String empty. ].
    start := 1 max: start.
    end := self length min: end.
    substring := String alloc: end - start + 2.
    self transferBytesTo: substring index: start destIndex: 1 length: end - start + 1.
    ^substring.
------------------------
isEmpty
    ^self length = 0.
------------------------
left: length
    (length >= self length) ifTrue: [ ^self ].
    (length <= self length negated) ifTrue: [ ^String empty ].
    (length > 0) ifTrue: [ ^self from: 1 to: length ].
    (length < 0) ifTrue: [ ^self from: length abs + 1 to: self length ].
    ^String empty.
------------------------
right: length
    (length >= self length) ifTrue: [ ^self ].
    (length <= self length negated) ifTrue: [ ^String empty ].
    (length > 0) ifTrue: [ ^self from: self length - length to: self length ].
    (length < 0) ifTrue: [ ^self from: 1 to: self length + length ].
    ^String empty.
------------------------
startsWith: aString
    (aString isEmpty) ifTrue: [ ^true ].
    (aString length > self length) ifTrue: [ ^false ].
    1 to: aString length do: [ :index |
        ((aString at: index) = (self at: index)) ifFalse: [ ^false ].
    ].

    ^true.
------------------------
endsWith: aString
    | checkIndex |
    (aString isEmpty) ifTrue: [ ^true ].
    (aString length > self length) ifTrue: [ ^false ].
    checkIndex := self length.
    aString length to: 1 do: [ :index |
        ((aString at: index) = (self at: checkIndex)) ifFalse: [ ^false ].
        checkIndex := checkIndex - 1.
    ].

    ^true.
------------------------
indexOf: aString
    ^0.
------------------------
contains: aString
    ^false.
------------------------
toUpperCase
    | result |
    result := String alloc: self size.
    1 to: self length do: [ :index | result at: index put: ((self at: index) toUpperCase) ].
    ^result.
------------------------
toLowerCase
    | result |
    result := String alloc: self size.
    1 to: self length do: [ :index | result at: index put: ((self at: index) toLowerCase) ].
    ^result.
------------------------
leftPadWith: aCharacter to: size
    (self length >= size) ifTrue: [ ^self ].

    ^self , (aCharacter repeat: size - self length).
------------------------
rightPadWith: aCharacter to: size
    (self length >= size) ifTrue: [ ^self ].

    ^(aCharacter repeat: size - self length) , self.
------------------------
centerPadWith: aCharacter to: size
    | half |
    (self length >= size) ifTrue: [ ^self ].
    half := (size - self length) // 2.

    ^(aCharacter repeat: half) , self , (aCharacter repeat: size - self length - half).
------------------------
limitTo: size
    (self length <= size) ifTrue: [ ^self ].

    ^self from: 1 to: size.
------------------------
< other
    ^(self compareBytesTo: other) = -1.
------------------------
<= other
    ^(self compareBytesTo: other) <= 0.
------------------------
= other
    ^(self compareBytesTo: other) = 0.
------------------------
>= other
    ^(self compareBytesTo: other) >= 0.
------------------------
> other
    ^(self compareBytesTo: other) = 1.
------------------------
hash
    ^self hashBytes.
------------------------

Class: Character
Superclass: Object
Instance Fields: value
Class Fields: table

Class Methods: Character
------------------------
asChar: aByte
    (0 to: 255 contains: aByte) ifFalse: [ self error: 'Invalid character: ', aByte ].
    ^table at: aByte + 1.
------------------------
digitValue: aValue
    (0 to: 9 contains: aValue) ifTrue: [ ^self asChar: aValue + 48 ].
    (10 to: 35 contains: aValue) ifTrue: [ ^self asChar: aValue + 55 ].
    self error: 'Invalid digit: ', aByte.
------------------------


Methods: Character
------------------------
value
    ^value.
------------------------
isDigit
    ^48 to: 57 contains: value.
------------------------
isUpper
    ^65 to: 90 contains: value.
------------------------
toLowerCase
    self isUpper ifTrue: [ ^Character asChar: value + 32 ].
    ^self.
------------------------
toUpperCase
    self isLower ifTrue: [ ^Character asChar: value - 32 ].
    ^self.
------------------------
isLower
    ^97 to: 122 contains: value.
------------------------
isAlpha
    ^self isUpper or: [ self isLower].
------------------------
isAlphaNumeric
    ^self isDigit or: [ self isAlphaNumeric].
------------------------
asString
    | result |
    result := String alloc: 2.
    result at: 1 put: self.
    ^result.
------------------------
repeat: amount
    | result |
    result := String alloc: amount + 1.
    1 to: amount do: [ :index | result at: index put: self ].
    ^result.
------------------------

Methods: Nil
------------------------
isNil
    ^true.
------------------------
notNil
    ^false.
------------------------
asString
    ^'nil'.
------------------------

Class Methods: True
------------------------
value
    ^true.
------------------------

Methods: True
------------------------
not
    ^false.
------------------------
ifTrue: aBlock
    ^aBlock value.
------------------------
ifTrue: aBlock ifFalse: otherBlock
    ^aBlock value.
------------------------
ifFalse: aBlock
    ^nil.
------------------------
& other
    ^other.
------------------------
and: otherBlock
    ^otherBlock value.
------------------------
or: otherBlock
    ^true.
------------------------
| other
    ^true.
------------------------
asString
    ^'true'.
------------------------

Methods: False
------------------------
not
    ^true.
------------------------
ifTrue: aBlock
    ^nil.
------------------------
ifTrue: aBlock ifFalse: otherBlock
    ^otherBlock value.
------------------------
ifFalse: aBlock
    ^aBlock value.
------------------------
& other
    ^false.
------------------------
and: otherBlock
    ^false.
------------------------
or: otherBlock
    ^otherBlock value.
------------------------
| other
    ^other.
------------------------
asString
    ^'false'.
------------------------

Methods: SmallInteger
------------------------
negated
    ^self * -1.
------------------------
min: other
    self < other ifTrue: [ ^self ] ifFalse: [ ^other ].
------------------------
max: other
    self > other ifTrue: [ ^self ] ifFalse: [ ^other ].
------------------------
abs
    (self < 0) ifTrue: [ ^self negated ].
    ^self.
------------------------
asString: radix
    | result value |

    result := StringBuilder new.
    value := self abs.

    [ value >= radix ] whileTrue:
        [   result prepend: (Character digitValue: value % radix).
            value := value // radix. ].

    result prepend: (Character digitValue: value).
    (self < 0) ifTrue: [ result prepend: '-' ].
    ^result asString.
------------------------
asString
    ^self asString: 10.
------------------------
to: other do: aBlock
    (other >= self) ifTrue:  [ self to: other     by: 1 do: aBlock ]
                    ifFalse: [ self downTo: other by: 1 do: aBlock ].
------------------------
to: other by: delta do: aBlock
    | index |
    index := self.
    [ index <= other ] whileTrue: [ aBlock value: index. index := index + delta ].
------------------------
downTo: other by: delta do: aBlock
    | index |
    index := self.
    [ index >= other ] whileTrue: [ aBlock value: index. index := index - delta ].
------------------------
to: other contains: aValue
    ^(aValue >= self) & (aValue <= other).
------------------------

Methods: BlockContext
------------------------
fork: name
    <Primitive:25>
------------------------
whileNotNil: aBlock
    | obj |
    [ obj := self value. obj notNil. ] whileTrue: [ aBlock value: obj ].
------------------------

Class: ProcessScheduler
Superclass: Object
Instance Fields: activeProcess timerSemaphore inputSemaphore firstWaitingProcess lastWaitingProcess

Class: Semaphore
Superclass: Object
Instance Fields: excessSignals firstWaitingProcess lastWaitingProcess

Methods: Semaphore
------------------------
wait
    <Primitive:41>
------------------------
signal
    <Primitive:42>
------------------------

Class: Terminal
Superclass: Object
Class Fields: root

Class Methods: Terminal
------------------------
nextEvent
    <Primitive:46>
------------------------
output: aString
    <Primitive:47>
------------------------
print: anObject
    self output: anObject asString
------------------------
cr
    self print: $10 asString.
------------------------
println: anObject
    self print: anObject.
    self cr.
------------------------

Class: InputEvent
Superclass: Object
Instance Fields: type key button col row

Methods: InputEvent
------------------------
key
    ^key.
------------------------

Class: Point
Superclass: Object
Instance Fields: x y

Class Methods: Point
------------------------
x: newX y: newY
    | point |
    point := self new.
    point x: newX.
    point y: newY.
    ^point.
------------------------

Methods: Point
------------------------
x
    ^x.
------------------------
y
    ^y.
------------------------
x: newX
    x := newX.
    ^self.
------------------------
y: newY
    y := newY.
    ^self.
------------------------
+ aPoint
    ^Point x: x + aPoint x y: y + aPoint y.
------------------------
* aScalar
    ^Point x: x * aScalar y: y * aScalar.
------------------------
asString
    | builder |
    builder := StringBuilder new.
    builder append: x.
    builder append: '@'.
    builder append: y.
    ^builder asString.
------------------------

Class: Rectangle
Superclass: Object
Instance Fields: origin dimensions

Class Methods: Rectangle
------------------------
origin: aPoint dimensions: anotherPoint
    | rect |
    rect := self new.
    rect origin: aPoint.
    rect dimensions: anotherPoint.
    ^rect.
------------------------

Methods: Rectangle
------------------------
origin
    ^origin.
------------------------
dimensions
    ^dimensions.
------------------------
origin: aPoint
    origin := aPoint.
    ^self.
------------------------
dimensions: aPoint
    dimensions := aPoint.
    ^self.
------------------------
contains: aPoint
    aPoint x < origin x ifTrue: [ ^false. ].
    aPoint y < origin y ifTrue: [ ^false. ].

    aPoint x > (origin x + dimensions x) ifTrue: [ ^false. ].
    aPoint y > (origin y + dimensions y) ifTrue: [ ^false. ].

    ^true.
------------------------
intersects: aRect
    aRect origin x > (origin x + dimensions x) ifTrue: [ ^false. ].
    aRect origin y > (origin y + dimensions y) ifTrue: [ ^false. ].

    (aRect origin x + aRect dimensions x) < origin x ifTrue: [ ^false. ].
    (aRect origin y + aRect dimensions y) < origin y ifTrue: [ ^false. ].

    ^true.
------------------------

Methods: SmallInteger
------------------------
@ other
    ^Point x: self y: other.
------------------------

Class: Component
Superclass: Object
Instance Fields: rect

Methods: Component
------------------------
redraw: requiredRect
    ^nil.
------------------------
at: localPoint inColor: aColorIndex put: aString
    | pos |
    localPoint y >= rect dimensions y ifTrue: [ ^self. ].
    localPoint x + aString size >= rect dimensions x ifTrue: [

    ].

    pos := localPoint + rect origin.

    pos y < 0 ifTrue: [ ^self. ].

    pos x < 0 ifTrue: [
        pos x negate >= aString size ifTrue: [ ^self. ].
        aString := aString left: pos x.
        pos x: 0.
    ].


    ^self.
------------------------

Class: RenderContext
Superclass: Object
Instance Fields: parent offset dimensions currentColor

Class Methods: RenderContext
------------------------
withParent: aParent offset: localOffset dimensions: dimensions
    | ctx |
    ctx := self new.
    parent := aParent.
    aParent notNil ifTrue: [
        currentColor := aParent color.
        offset = aParent offset + localOffset.

    ].
------------------------

Methods: RenderContext
------------------------

at: aPoint startWith: leftString repeat: filler endWith: endString
    ^nil.
------------------------



Class: Link
Superclass: Object
Instance Fields: value next

Methods: Link
------------------------
value
    ^value.
------------------------
next
    ^next.
------------------------
value: aValue
    value := aValue.
    ^self.
------------------------
next: nextLink
    next := nextLink.
    ^self.
------------------------

Class: LinkedList
Superclass: Object
Instance Fields: head tail

Methods: LinkedList
------------------------
append: aValue
    | nextLink |
    nextLink := Link new value: aValue.
    tail isNil ifTrue: [ head := nextLink. tail := nextLink. ]
               otherwise: [ tail next: nextLink. tail := nextLink. ].
    ^self.
------------------------
each: aBlock
    | node |
    node := head.
    [ node notNil ] whileTrue: [ aBlock value: node value. node := node next. ].
    ^self.
------------------------

Class: Array
Superclass: Object

Methods: Array
------------------------
do: aBlock
    1 to: self size do: [ :i | aBlock value: (self at: i) ].
------------------------
filter: aFilterBlock do: aBlock
    self do: [ :element | (aFilterBlock value: element) ifTrue: [ aBlock value: element ] ].
------------------------
withHash: aHash loop: aBlock
    | index |

    index := (aHash % self size) + 1.
    index to: self size do: aBlock.
    1 to: (index - 1) do: aBlock.
------------------------

Class: SymbolTable
Superclass: Object
Instance Fields: tally table

Methods: SymbolTable
------------------------
init
    tally := 0.
    table := Array new: 32.
------------------------
do: aBlock
    table filter: [ :symbol | symbol notNil ] do: [ :symbol | aBlock value: symbol ].
------------------------
at: aString
    | symbol |
    table withHash: (aString hash) loop: [ :i |
         symbol := table at: i.
         symbol isNil ifTrue: [
            symbol := Symbol forString: aString.
            table at: i put: symbol.
            self incrementTally.
            ^symbol.
         ].

         ((symbol compareBytesTo: aString) = 0) ifTrue: [ ^symbol ].
    ].

    self error: 'Symbol table overflow'.
------------------------
incrementTally
    tally := tally + 1.
    (tally > (table size * 3 // 4)) ifTrue: [ self grow ].
------------------------
grow
    | oldTable |
    oldTable := table.
    table := Array new: table size + 256.
    oldTable filter: [ :symbol | symbol notNil ] do: [ :symbol | self insert: symbol ].
------------------------
insert: aSymbol
   | association |
   table withHash: aSymbol hashBytes loop: [ :i |
        (table at: i isNil) ifTrue: [ table at: i put: aSymbol. ^i ].
   ].

   self error: 'Symbol table overflow'.
------------------------

Class: Dictionary
Superclass: Object
Instance Fields: tally table

Methods: Dictionary
------------------------
init
    tally := 0.
    table := Array new: 32.
------------------------
do: aBlock
    table filter: [ :association | association notNil ] do: [ :association | aBlock value: association ].
------------------------
associationAt: aKey
    | association |
    table withHash: (aKey hash) loop: [ :i |
         association := table at: i.
         association isNil ifTrue: [
            association := Association forKey: aKey.
            table at: i put: association.
            self incrementTally.
            ^association.
         ].

         (association key = aKey) ifTrue: [ ^association. ].
    ].

    self error: 'Dictionary overflow'.
------------------------
incrementTally
    tally := tally + 1.
    (tally > (table size * 3 // 4)) ifTrue: [ self grow ].
------------------------
grow
    | oldTable |
    oldTable := table.
    table := Array new: table size + 32.
    oldTable filter: [ :a | a notNil ] do: [ :a | self at: a key put: a value ].
------------------------
at: aKey
   | association |
   table withHash: aKey hash loop: [ :i |
        association := table at: i.
        (association isNil) ifTrue: [ ^nil ].
        (association key = aKey) ifTrue: [ ^association value ].
   ].
------------------------
at: aKey put: aValue
    ^(self associationAt: aKey) value: aValue.
------------------------
remove: aKey
    | association |
    table withHash: aKey hash loop: [ :i |
        association := table at: i.
        (association isNil) ifTrue: [ ^nil ].
        (association key = aKey) ifTrue: [
            table at: i put: nil.
            tally := tally - 1.
            ^association value ].
    ].
    ^nil.
------------------------

Class: IdentityDictionary
Superclass: Dictionary

Methods: IdentityDictionary
------------------------
at: aKey
    ^(self associationAt: aKey) value.
------------------------
remove: aKey
    | association previousValue |
    association := self associationAt: aKey.
    previousValue := association value.
    association value: nil.
    ^previousValue.
------------------------

Class: Association
Superclass: Object
Instance Fields: key value

Class Methods: Association
------------------------
forKey: aKey
    | association |
    association := self new.
    association key: aKey.
    ^association.
------------------------

Methods: Association
------------------------
key
    ^key.
------------------------
value
    ^value.
------------------------
key: aKey
    key := aKey.
------------------------
value: aValue
    value := aValue.
------------------------
asString
    ^(key asString , ': ', value asString).
------------------------

Class: System
Superclass: Object

Class Methods: System
------------------------
allClasses
    SmallTalk do: [ :a | (a value is: Class) ifTrue: [ Terminal at: 0 @ 0 color: 0 put: a key plainString ] ].
------------------------

Class: ByteArray
Superclass: Object

Class: StringBuilder
Superclass: Object
Instance Fields: buffer index

Class Methods: StringBuilder
------------------------
with: anObject
    ^self new append: anObject.
------------------------

Methods: StringBuilder
------------------------
init
    buffer := ByteArray alloc: 16.
    index := 1.
    ^self.
------------------------
ensureSpace: length
    | requiredBlocks freeSpace oldBuffer |
    freeSpace := buffer size - index.
    (length >= freeSpace) ifTrue: [
        requiredBlocks := ((buffer size + length - freeSpace) % 16) + 1.
        oldBuffer := buffer.
        buffer := ByteArray alloc: 16 * requiredBlocks.
        oldBuffer transferBytesTo: buffer index: 1 destIndex: 1 length: oldBuffer size.
    ].
------------------------
append: anObject
    | string |
    string := anObject asString.
    self ensureSpace: string length.
    string transferBytesTo: buffer index: 1 destIndex: index length: string length.
    index := index + string length.
    ^self.
------------------------
prepend: anObject
    | string |
    string := anObject asString.
    self ensureSpace: string length.
    buffer transferBytesTo: buffer index: 1 destIndex: string length + 1 length: index - 1.
    string transferBytesTo: buffer index: 1 destIndex: 1 length: string length.
    index := index + string length.
    ^self.
------------------------
asString
    | string |
    string := String alloc: index.
    buffer transferBytesTo: string index: 1 destIndex: 1 length: index - 1.
    ^string.
------------------------

Class: Stream
Superclass: Object

Methods: Stream
------------------------
hasNext
    ^false.
------------------------
next
    ^nil.
------------------------

Class: Stage
Superclass: Stream
Instance Fields: source block

Methods: Stream
------------------------
hasNext
    ^false.
------------------------
next
    ^nil.
------------------------



