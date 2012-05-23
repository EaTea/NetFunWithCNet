TEST_TARGET = Test

CDEP = Circle Point Util Trilateration Location
CFILES = $(patsubst %, %.cpp, $(CDEP))
COBJ = $(patsubst %, %.o, $(CDEP))

C_TEST_DEPS = $(CDEP) $(TEST_TARGET) TestTrilateration TestSerialRead TestSerialWrite
CFILES_TEST = $(patsubst %, %.cpp, $(C_TEST_DEPS))
COBJ_TEST = $(patsubst %, %.o, $(C_TEST_DEPS))

HEADS = locate.h trilaterate.h serialise.h debug.h
CFLAGS = -Wall -Werror -pedantic -ggdb -O2 -lm

tests: $(TEST_TARGET)

.cpp.o:
	g++ $(CFLAGS) -c -o $@ $<

$(TEST_TARGET): $(CFILES_TEST) $(COBJ_TEST) $(HEADS)
	g++ $(CFLAGS) -o $(TEST_TARGET) $(COBJ_TEST)

clean:
	rm *.o
	rm $(TEST_TARGET)
