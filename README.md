# ESCA 
### Experimental Static Code Analyzer
###### for C and C++ projects

This analyzer can help you to find memory leaks in your project.

C++17 version must be  or more
Before use you need to install z3 from https://github.com/Z3Prover/z3

To start it with `cmake`, you need to install flag `CMAKE_EXPORT_COMPILE_COMMANDS` to `ON`
and run `ESCA --cmake <path/to/your/compile_comands.json>`

To start with any dir you need run `find_cpp_files.py --dir <project dir>` and then run 
`ESCA --files files_for_ESCA.txt`