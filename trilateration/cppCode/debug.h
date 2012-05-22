#include <stdio.h>
#include <stdarg.h>

//prints a string to stderr like printf
//essentially a perror clone
void PRINT_ERR(const char*,...);
