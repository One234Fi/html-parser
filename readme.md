# A work in progress html parser

## Build
``` sh
cc bob.c -o bob
./bob
```

To change the build configuration, bob.c can be modified directly. Bob 
will automatically detect that he has been changed, and recompile himself
before compiling the program the next time you run him.


## Usage
The main program is built as 'main.out' by default.
``` sh
./main.out [options] <filepath>
```
The default invocation will print all content in a given html file (IE: strip
text out from tags).

| options | |
| --- | --- |
| --parse-urls | instead of printing all content, the program will print all hrefs |


## Tests
Each set of tests are in their own executable named '<test name>_tst.out'.
