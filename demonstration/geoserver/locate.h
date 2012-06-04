/*
	Implemented in SampleProcessing.cpp
	Given n iPod samples, processes the samples and trilaterates the samples
	accordingly, by finding which of the MACs these samples are related to and
	recording them, then performing trilateration if there are at least 3 samples
*/
void processSamples	(
											int, //number of samples
											IPOD_SAMPLE * //pointer to samples
										);

/*
	Implemented in SampleProcessing.cpp
	Given all the APs broadcasting, processes the samples, trilaterates and returns
	the most likely position that these MACs intersect at
	returns 0 if successfully trilaterated, and 1 otherwise
*/
int processLocation	(
											int, //number of APs
											APINFO *, //pointer to APs
											float*, //latitude
											float* //longitude
										);

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

/*
	Implemented in Location.cpp
	A function designed to be called at beginning of program, sets up trilateration
	so that the differences in the bounding rectangle in metres can be calculated
*/
void setupPermissibleArea	(	
														float, //latitude 1
														float, //longitude 1
														float, // latitude 2
														float //longitude 2
													);
