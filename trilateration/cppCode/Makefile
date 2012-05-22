TARGET = TestTrilateration
CDEP = Circle Point Util Trilateration Location $(TARGET) 
CFILES = $(patsubst %, %.cpp, $(CDEP))
COBJ = $(patsubst %, %.o, $(CDEP))
HEADS = locate.h trilaterate.h serialise.h
CFLAGS = -Wall -Werror -pedantic -ggdb -O2 -lm

all: $(TARGET)
	
.cpp.o:
	g++ $(CFLAGS) -c -o $@ $<

$(TARGET): $(CFILES) $(COBJ) $(HEADS)
	g++ $(CFLAGS) -o $(TARGET) $(COBJ)

clean:
	rm *.o
	rm $(TARGET)
