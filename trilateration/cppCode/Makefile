TARGET = Test
CDEP = Circle Location Util Trilateration $(TARGET) 
CFILES = $(patsubst %, %.cpp, $(CDEP))
COBJ = $(patsubst %, %.o, $(CDEP))
HEADS = trilat.h
CFLAGS = -Wall -Werror -pedantic -ggdb -O2 -lm

all: $(TARGET)
	
.cpp.o:
	g++ $(CFLAGS) -c -o $@ $<

$(TARGET): $(CFILES) $(COBJ) $(HEADS)
	g++ $(CFLAGS) -o $(TARGET) $(COBJ)

clean:
	rm *.o
