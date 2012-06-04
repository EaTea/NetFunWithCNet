#include <stdio.h>
#include <stdarg.h>

//if this line is uncommented it will specify that the program should output
//verbosely with all its debug statements after recompilation
//#define VERBOSE

//prints a string to stderr like printf
void PRINT_ERR(const char*,...);
