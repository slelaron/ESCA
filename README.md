# CWE-467 
### Analyzer that detects CWE-467: Use of sizeof() on a Pointer Type
###### for C and C++ projects

This analyzer can help you to detect CWE-467: Use of sizeof() on a Pointer Type

C++17 version must be  or more
Before use you need to install z3 from https://github.com/Z3Prover/z3

### Explanation:

Care should be taken to ensure sizeof returns the size of the data structure itself, and not the size of the pointer to the data structure.

In this example, sizeof(foo) returns the size of the pointer.

```
double *foo;
...
foo = (double *)malloc(sizeof(foo));
```

In this example, sizeof(\*foo) returns the size of the data structure and not the size of the pointer.

```
double *foo;
...
foo = (double *)malloc(sizeof(*foo));
```

If analyzer detects sizeof from pointer, it will signalize you about the problem with following error:

```
The code calls sizeof() on a malloced pointer type, which always returns the wordsize/8 on line /path/to/your/file:line:offset
```

### Usage:

To start detection print 
`./CWE467 <source file>`

To get help print
`./CWE467 -h`
