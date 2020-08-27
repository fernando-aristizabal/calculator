# calculator
Command-line calculator with variable assignments and trig functions

Uses shunting yard algorithm to convert expressions to reverse polish notation for order of operations

Accepts sin, cos, and log functions.
Supports five operators: +, -, *, /,^.

Assigns mathematical constants:
    Pi = 3.14169
    e = 2.718

Allows for variable assignment with keyword "let" (let <variable-name> = <expression>)
Exit by typing in "quit"



Compile
g++ (Ubuntu 7.5.0-3ubuntu1~16.04) 7.5.0

g++ -std=c++11 calculator.cpp -o calculator


