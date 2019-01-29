# GDB/Makefile Ref

## GDB

`b splitline.c:71 if counter == 16`

`p`: print

info b

bt, f; info args

disassemble

## Makefile

### What makefiles contain

```makefile
target … : prerequisites …
		recipe
        …
        …
```

Makefiles contain five kinds of things: *explicit rules*, *implicit rules*, *variable definitions*, *directives*, and *comments*. 显式规则、隐晦规则、变量定义、文件指示、注释

1. *explicit rules*: When writing makefiles, we're majorly writing these rules.
2. *implicit rules:* Automatically generates by `make`.
3. *variable definitions:* `obj = ...` Reference: `$(obj)`.
4. *directives:* `include` other makefiles, and other functions.
5. *comments*: `# This symble for comment`.

### Details

A shallow example (for compiling two source files into one executable, with no `.h` yet):

```makefile
objects = socklib.o EZFileServer.o

EZFileServer : $(objects)
				gcc -Wall -Wextra -g -o EZFileServer $(objects)

.PHONEY : clean
clean :
		-mv EZFileServer $(objects) /tmp

```

Notes:

1. `make` automatically generates rules.
2. `clean`: 
   1. Use `.PHONEY` to seperate `clean` (so that won't be confused with an actual file called *clean*), 
   2. and put it in the end (because the begining of the file would be run **by default**).
3. A `recipe` must begin with `Tab`.
4. `-mv`: `-` means continue in spite of errors from the command.
5. `utils.o : defs.h`: when `defs.h` updated, according to this rule `utils.o` would be recompiled.