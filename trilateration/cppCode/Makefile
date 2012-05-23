TEST_TARGET = Test

CDEP = Circle Point Util Trilateration Location SampleProcessing
CFILES = $(patsubst %, %.cpp, $(CDEP))
COBJ = $(patsubst %, %.o, $(CDEP))

C_TEST_DEPS = $(CDEP) $(TEST_TARGET) TestTrilateration TestSerialRead TestSerialWrite
CFILES_TEST = $(patsubst %, %.cpp, $(C_TEST_DEPS))
COBJ_TEST = $(patsubst %, %.o, $(C_TEST_DEPS))

HEADS = trilaterate.h #serialise.h debug.h sample_processing.h locate.h 
CFLAGS = -Wall -Werror -pedantic -ggdb -O2 -lm

tests: ipod3230.h $(TEST_TARGET)
	rm ipod3230.h

#temporary hack to get ipod3230.h
ipod3230.h:
	cp ../../ipod3230.h .

.cpp.o:
	g++ $(CFLAGS) -c -o $@ $<

$(TEST_TARGET): $(CFILES_TEST) $(COBJ_TEST) $(HEADS)
	g++ $(CFLAGS) -o $(TEST_TARGET) $(COBJ_TEST)

clean:
	rm *.o
	rm $(TEST_TARGET)
