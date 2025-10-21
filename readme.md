# XD LLVM COMPILER

XD is a small language I am currently developing. It is supposed to be a C like language with slightly different syntax

## About

XD compiler is built in C++ using LLVM's API to generate LLVM IR Code.
This project was more or less for learning how compilers work. I have created a compiler that compiles directly to x64 assembly, however I wanted to challenge myself in creating something a little more complex.
The hardest part of this small project was generating IR with LLVM. As a new developer, I found it tricky to understand the documentation.
This project takes a lot of inspiration from LLVM'S Kaleidoscope documentation, however I do things VERY differently. For example, Kaleidoscope does the lexing, the parsing, and the code generation all in one go kind of and doesn't necessarily builds an Abstract Syntax Tree.

This compiler is using a recursive descent parser. You may see the languages grammar in grammar.txt. Although I'm not quite sure if i wrote the grammar properly.

This is still a work in progress and Im going to be working on this project a lot. Hopefully I can make this project sophisticated enough to actually self host the compiler

## Syntax

The syntax is very simple and easy to understand.

Functions:
```
fn int foo(){
  // some statements
}
```

Variables:
```
fn int foo(){
  let int x = 10;
}
```

I went with this approach because it seemed more simpler to me. 

# Compiling the compiler

To compile this project you need a few things.

If You're windows its preferable to use WSL. I personally used an ubuntu instance of WSL. Then within the Ubuntu session I installed LLVM from there.

If you're on Linux then just install LLVM for your specific operating system. 

Follow this guide here: https://llvm.org/docs/GettingStarted.html#getting-the-source-code-and-building-llvm

Its prefered to build llvm with ninja as it is a bit faster than make in my experience.

After installation you may do the following:

```
cd xd-llvm
```

```
mkdir build && cd build
```

```
cmake ..
```

```
make
````

Now you have successfully compiled XD.