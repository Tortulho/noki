# How to use noki

1º:
Compile everything.

2º:
You can run in REPL mode or File Reading (.noki) mode

Example:

REPL mode: ./noki

FR mode: ./noki filename

# Syntax

Assign/create/modify: x = 10, y = "abc", z = null

Negative numbers: -10

Next update is to allow c/cpp funcs.

Conditional:

if x > 10 && true { # and/or can be used instead of &&/||

  say("Hello World!") # This is a comment, you can use ';' if you want to end lines
  
else {

  say("Hi World!")
  
}

Note: Errors can be represented by null or if is an exception it will throw at c++ runtime_error system

When i finish up the cpp functions i will issue a manual to use noki.
