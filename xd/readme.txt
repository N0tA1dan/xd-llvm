let int x = 10;

the grammar would be 
let type identifier = expression;

to do: 
    update llvm and use llvm for code generation. check llvm kaleidoscope

to fully install llvm check out the first answer to this reply:
    https://stackoverflow.com/questions/68752972/how-do-you-build-all-of-llvm

essentially, llvm provides a thing called a IrBuilderBase. This is a class with various methods in order to generate the IR from the AST tree.

i plan on using llvm for generation.

i cant seem to find this seg fault for the life of me. I think it has something to do with InitializeModule or some sirts

Try adding a better optional value for the token type. So i can support different variable types and different number types like floats or ints.