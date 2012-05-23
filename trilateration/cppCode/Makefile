SERIAL_READ_TARGET = TestSerialRead
SERIAL_WRITE_TARGET = TestSerialWrite
TRILATERATE_TARGET = TestTrilateration
CDEP = Circle Point Util Trilateration Location
READ_DEP = $(CDEP) $(SERIAL_READ_TARGET) 
WRITE_DEP = $(CDEP) $(SERIAL_WRITE_TARGET) 
TRIL_DEP = $(CDEP) $(TRILATERATE_TARGET) 
CFILES = $(patsubst %, %.cpp, $(CDEP))
COBJ = $(patsubst %, %.o, $(CDEP))
HEADS = locate.h trilaterate.h serialise.h
CFLAGS = -Wall -Werror -pedantic -ggdb -O2 -lm

serialReadTest: $(SERIAL_READ_TARGET)

serialWriteTest: $(SERIAL_WRITE_TARGET)

trilaterateTest: $(TRILATERATE_TARGET)
	
.cpp.o:
	g++ $(CFLAGS) -c -o $@ $<

$(SERIAL_READ_TARGET): $(CFILES) $(COBJ) $(HEADS)
	g++ $(CFLAGS) -o $(SERIAL_READ_TARGET) $(COBJ)

$(SERIAL_WRITE_TARGET): $(CFILES) $(COBJ) $(HEADS)
	g++ $(CFLAGS) -o $(SERIAL_WRITE_TARGET) $(COBJ)

$(TRILATERATE_TARGET): $(CFILES) $(COBJ) $(HEADS)
	g++ $(CFLAGS) -o $(TRILATERATE_TARGET) $(COBJ)

clean:
	rm *.o
	rm $(SERIAL_READ_TARGET)
