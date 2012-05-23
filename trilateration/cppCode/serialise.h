#include <stdio.h>

/*
	Implemented in Location.cpp
	Reads all the values from a file specified by the const char* parameter
	to the map in Location.cpp
	Does not modify file contents
	Assumes file has a csv structure and will stop reading file early if it
	encounters file not in csv format
*/
void readSamplesFromFile	(
														const char* //filename (path) of the file to read data from
													);
/*
	Implemented in Location.cpp
	Serialises all the values in Location.h's map to a file specified by the
	const char* parameter
	Overwrites current contents of the file
	Writes in csv format
*/
void writeSamplesToFile		(
														const char* //filename (path) of file to write data to
													);
