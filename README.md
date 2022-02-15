# Overview
> cilo is a text editor with syntax highlighting and search written in C99.

cilo is a minimal text editor, which implements a small feature set - while having zero third party dependencies. Designed to be simple, cilo, nonetheless, supports most common text editor operations.

# Release Build
cilo is built using CMake and requires at least C99. clang-tidy and clang-format config files are also provided.

```
git clone https://github.com/buzzcut-s/cilo.git
cd cilo/
mkdir build
cd build/
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
cd src/
```

The binary is built in the ```cilo/build/src/``` directory

## Usage

```bash
cilo              # Start a new text buffer
cilo <file path>  # Open a file
```

### Keyboard shortcuts

| Keyboard shortcut | Description                                                   |
| ----------------- | ------------------------------------------------------------- |
| Ctrl-F            | Incremental search; use arrows to navigate                    |
| Ctrl-S            | Save the buffer to the current file, or specify the file path |
| Ctrl-Q            | Quit                                                          |

# Syntax Highlighting
- cilo has a very extensible system to add new syntax highlights for different file types. By default, cilo supports highlighting for the C language. See the ```editor/syntax``` module for more details. 

# Implementation Details
- cilo uses an in-memory array of lines which is created when loading new files, and flushed when saving files. This was the approach taken by the Visual Studio Code team until 2018 (See: [here](https://code.visualstudio.com/blogs/2018/03/23/text-buffer-reimplementation)); if it was good enough for them - it is good enough for us. Specialized data structures such as gap buffers and piece tables were looked into, but due to the exponential increase in complexity when implementing them - a simple data layout was chosen as the default.
- Since cilo uses no third party dependencies (not even curses), we control the terminal using fairly standard (if sometimes less documented) VT100 (and similar) escape sequences.
- Uses portable C99, tested on both GCC and Clang, with no compiler specific extensions. We only support Linux.
- Supports file type detection to enable language specific highlights.
- Supports incremental search.

I am using ```cilo``` as my [default git editor](https://git-scm.com/book/en/v2/Customizing-Git-Git-Configuration#Basic-Client-Configuration).

# License
cilo is licensed under the MIT License, © 2022 Piyush Kumar. See [LICENSE](https://github.com/buzzcut-s/clocks/blob/main/LICENSE) for more details.

cilo is inspired by [`kilo`](https://github.com/antirez/kilo), another text editor written in C. kilo is licensed under the BSD 2-Clause License, © 2016 Salvatore Sanfilippo. See [LICENSE](https://github.com/antirez/kilo/blob/master/LICENSE) for more details.

