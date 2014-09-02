CC=g++
CFLAGS=-Wall -Werror -O3 -std=c++11 -g -Wl,--no-as-needed -ldl -rdynamic -fstack-protector-all

INCLUDE=-Iinclude/

# include statements necessary to link all the individual libraries
INCLUDE_LIBS=-Iexternal/giga/external/catch/include/ -Iexternal/exceptionpp/include/ -Iexternal/giga/include/libs/stacktrace/ -Iexternal/giga/external/cachepp/include/ -Iexternal/giga/include/

LIBS=-pthread

S_SOURCES+=src/*cc libs/*/*cc
S_OBJECTS=$(S_SOURCES:.cc=.o)

T_SOURCES+=tests/*cc libs/*/*cc
T_OBJECTS=$(T_SOURCES:.cc=.o)

S_EXECUTABLE=entangle.app
T_EXECUTABLE=tests.app

.PHONY: all clean test

all: $(S_SOURCES) $(S_EXECUTABLE)

$(S_EXECUTABLE): $(S_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDE_LIBS) $(INCLUDE) $(S_OBJECTS) -o $@ $(LIBS)

$(T_EXECUTABLE): $(T_OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDE_LIBS) $(INCLUDE) $(T_OBJECTS) -o $@ $(LIBS)

test: clean $(S_EXECUTABLE) $(T_EXECUTABLE)

clean:
	@rm -f $(S_EXECUTABLE) $(T_EXECUTABLE) *.o *.log core
