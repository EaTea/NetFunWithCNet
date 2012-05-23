#include "test.h"

//if this line is uncommented, test serialisation reading from file
#define TEST_SERIAL_READ

//if this line is uncommented, test serialisation writing to file
#define TEST_SERIAL_WRITE

//if this line is uncommented, test trilateration test cases
#define TEST_TRILATERATION

int main(int argc, const char** argv)
{
#ifdef TEST_TRILATERATION
	#ifdef VERBOSE
		PRINT_ERR("Testing trilateration\n");
	#endif
	testTrilateration();
#endif

#ifdef TEST_SERIAL_WRITE
	#ifdef VERBOSE
		PRINT_ERR("Testing serialisation writing\n");
	#endif
	if(argc < 2)
	{
		PRINT_ERR("Testing serialisation write but not enough arguments passed to main, please specify a filename as parameter to this program\n");
	}
	testSerialWrite(argv[1]);
#endif

#ifdef TEST_SERIAL_READ
	#ifdef VERBOSE
		PRINT_ERR("Testing serialisation reading\n");
	#endif
	if(argc < 2)
	{
		PRINT_ERR("Testing serialisation read but not enough arguments passed to main, please specify a filename as parameter to this program\n");
	}
	testSerialRead(argv[1]);
#endif

}
