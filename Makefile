CC=g++
CFLAGS=-Wall -Werror -O3 -std=c++11 -g -Wl,--no-as-needed -ldl -rdynamic -fstack-protector-all

INCLUDE=-Iinclude/

# include statements necessary to link all the individual libraries
INCLUDE_LIBS=-Iexternal/giga/external/catch/include/ -Iexternal/exceptionpp/include/ -Iexternal/msgpp/include/ -Iexternal/giga/include/libs/stacktrace/ -Iexternal/giga/external/cachepp/include/ -Iexternal/giga/include/

LIBS=-pthread

SOURCES+=src/*cc tests/*cc libs/*/*cc
OBJECTS=$(SOURCES:.cc=.o)

EXECUTABLE=entangle-server.app

.PHONY: all clean test prep

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@$(CC) $(CFLAGS) $(INCLUDE_LIBS) $(INCLUDE) $(OBJECTS) -o $@ $(LIBS)

prep:
	@rm -rf tests/files/
	@mkdir -p tests/files/

test: clean $(EXECUTABLE) prep
	@ulimit -c unlimited && time ./$(EXECUTABLE) | tee results.log

clean:
	@rm -f $(EXECUTABLE) *.o *.log core
