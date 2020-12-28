# Naja
*The programming language for learning how to write code*

## Motivation

Naja's primary goal is to make programming accessible for everyone.

As any seasoned programmer knows; there is no "best" programming language. A language is simply a tool, and the best tool is the one best suited to a job.

We live in a world where knowledge within the field of informatics is both in increasing demand and, arguably, sorely needed in the general population.

Naja aims to achieve its goals and help solve these problems by providing a programming language, of which the primary focus is to be easily teachable.

For speed we have C, for mobility we have Java, for web development we have JavaScript and for everything else we have Python. Python, with its focus on readability, is great for learning how to code. It is however, first and foremost, a tool made for building.

Naja will be the tool for teaching.

## Current Roadmap

### 1) The Compiler

A compiler might seem unnecessary at first. If the goal is to teach, why not just build an interpreter like the one Python has?

The first reason is accessibility. We believe making a LLVM compiler for Naja is the best way to ensure it is available on as many platforms as possible.

The second reason is an opportunity for deeper understanding. If Naja can be compiled on any platform, an interpreter written in Naja can be compiled on any platform. If we then implement the interpreter in Naja, anyone who learns it can learn how the interpreter works, and in this way gain deeper knowledge about the nature of how programs work, on a fundamental level.

### 2) The Interpreter

Being able to run Naja with an interpreter is important for ease of access.

Teaching someone how to print "Hello World!" to the terminal should never take more than five minutes. We shouldn't have to explain why we need to make a new file. Why we need to write something in that file, then give that file to an application. How this application makes a new application. Or how we now need to run this new application, just to get our output. We should be able to simply install the interpreter, explain what a print statement is, what a string is, how to put the two together; and there's the output.

Further down the line, with the interpreter written in Naja as an example, it will be easier to learn in an intuitive way, about how programs and their languages work in general.

### 3) The Tutorials

With the compiler and interpreter in place, the next step will be developing documentation and tutorials. From printing "Hello World!" to the terminal, to implementing the Naja interpreter itself.

## Build Instructions

Naja was developed using Premake 5.0.0-alpha15, available for download at https://premake.github.io/.

To generate makefiles and build the project with make, simply run:

    $ premake gmake && make

By default, Naja compiles using the Clang toolset. However, Premake can be configured generate make and project files for other toolsets and IDEs, such as GCC, Visual Studio or XCode. All you need to do is make the appropriate modifications to the premake script (`premake5.lua`).

To learn more about this, please refer to the Premake documentation (https://github.com/premake/premake-core/wiki).

