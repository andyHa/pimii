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
id
    <Primitive:23>
    ^0.
------------------------
hash
    self id.
------------------------
class
    <Primitive:12>
    ^self.
------------------------

Methods: Behaviour
------------------------
basicNew
    <Primitive:10>
    ^self.
------------------------
basicNew: size
    <Primitive:11>
    ^self.
------------------------
new
    <Primitive:10>
    ^self.
------------------------
new: size
    <Primitive:11>
    ^self.
------------------------
name
    ^name.
------------------------
name
    ^name.
------------------------
asString
    ^name plainString.
------------------------

Methods: Symbol
------------------------
plainString
    <Primitive:31>
    ^self.
------------------------
asString
    ^'#' , self plainString.
------------------------

Methods: String
------------------------
asString
    ^self.
------------------------
asSymbol
    <Primitive:30>
    ^self.
------------------------
, other
    <Primitive:32>
    ^self , other asString.
------------------------


Class Methods: System
------------------------
log: aString
    <Primitive:33>
    ^self.
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
ifFalse: aBlock
    ^nil.
------------------------
& other
    ^other.
------------------------
|| other
    ^true.
------------------------

Methods: False
------------------------
not
    ^true.
------------------------
ifTrue: aBlock
    ^nil.
------------------------
ifFalse: aBlock
    ^aBlock value.
------------------------
& other
    ^false.
------------------------
|| other
    ^other.
------------------------

Methods: SmallInteger
------------------------
negate
    ^self * -1.
------------------------
abs
    (self < 0) ifTrue:  [ ^self negated. ].
    ^self.
------------------------
asString: radix
    | result value |

    result := ''.
    (self < 0) ifTrue: [ result := '-'. ].

    value := self abs.

    [ value >= radix ] whileTrue:
        [   result := ((Character digitValue: (value % radix)) toString), result.
            value := value // radix. ].

    result := ((Character digitValue: value) toString), result.
    ^result.
------------------------

Methods: BlockContext
------------------------
fork
    <Primitive:25>
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
    <Primitive:26>
------------------------
signal
    <Primitive:27>
------------------------

Class: Terminal
Superclass: Object
Class Fields: root

Class Methods: Terminal
------------------------
nextEvent
    <Primitive:34>
------------------------
size
    <Primitive:35>
------------------------
at: aPoint color: aColorIndex put: aString
    <Primitive:36>
------------------------
showCursor
    <Primitive:37>
------------------------
hideCursor
    <Primitive:38>
------------------------
draw
    <Primitive:39>
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


Methods: SmallInteger
------------------------
@ other
    ^Point x: self y: other.
------------------------
