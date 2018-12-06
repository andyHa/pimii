Class: Boolean
Superclass: Object

Class: True
Superclass: Boolean

Class: False
Superclass: Boolean

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


