GEOSERVER = geoserver
TEST_TARGET = Test

#trilateration dependencies and test dependencies
CDEP = Circle Point Util Trilateration Location SampleProcessing
CFILES = $(patsubst %, %.cpp, $(CDEP))
COBJ = $(patsubst %, %.o, $(CDEP))

C_TEST_DEPS = $(CDEP) $(TEST_TARGET) TestTrilateration TestSerialRead TestSerialWrite
CFILES_TEST = $(patsubst %, %.cpp, $(C_TEST_DEPS))
COBJ_TEST = $(patsubst %, %.o, $(C_TEST_DEPS))

#geoserver dependencies
#add the name of all the files geoserver requires without extensions to CDEP_GEO
CDEP_GEO = $(CDEP) $(GEOSERVER)
CFILES_GEO = (patsubst %, %.cpp, $(CDEP_GEO))
COBJ_GEO = $(patsubst %, %.o, $(CDEP_GEO))

HEADS = trilaterate.h ipod3230.h
CFLAGS = -Wall -Werror -pedantic -ggdb -O2 -lm

#compile geoserver by calling "make geo"
geo: $(GEOSERVER)

$(GEOSERVER): $(CFILES_GEO) $(COBJ_GEO) $(HEADS)
	g++ $(CFLAGS) -o $(GEOSERVER) $(COBJ_GEO)


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
