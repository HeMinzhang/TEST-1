CC=gcc
CXX=g++
CFLAGS=-Wall -pthread
CXXFLAGS=-Wall -std=c++11
LDFLAGS=-lpthread -lboost_system -lrt -lboost_regex -lboost_log_setup -lboost_log -lboost_thread -lboost_filesystem -lboost_locale -lboost_program_options -lboost_chrono 
SELINKGW=selinkgw
EXE_SELINKGW=$(SELINKGW)
PREFIX=/opt/selink
SRC_ENCRYPTION=$(wildcard encryption/*.c)
OBJ_ENCRYPTION=$(addprefix encryption/obj/,$(notdir $(SRC_ENCRYPTION:.c=.o)))
SRC_SECUBE=$(wildcard secube/*.c)
OBJ_SECUBE=$(addprefix secube/obj/,$(notdir $(SRC_SECUBE:.c=.o)))
SRC_UTIL=$(wildcard util/*.cpp)
OBJ_UTIL=$(addprefix util/obj/,$(notdir $(SRC_UTIL:.cpp=.o)))
SRC_PROXY=$(wildcard proxy/*.cpp) $(wildcard proxy/provider/*.cpp)
OBJ_PROXY=$(addprefix proxy/obj/,$(notdir $(SRC_PROXY:.cpp=.o)))
SRC_SELINKGW=$(wildcard selinkgw/*.cpp)
OBJ_SELINKGW=$(addprefix selinkgw/obj/,$(notdir $(SRC_SELINKGW:.cpp=.o)))
INC=-I.
DEF=-D_GNU_SOURCE -DBOOST_ALL_DYN_LINK

OBJ_ALL_SELINKGW=$(OBJ_ENCRYPTION) $(OBJ_SECUBE) $(OBJ_UTIL) $(OBJ_PROXY) $(OBJ_SELINKGW)

all: dirs $(OBJ_ALL_SELINKGW) bin/$(EXE_SELINKGW)

bin/$(EXE_SELINKGW): $(OBJ_ALL_SELINKGW)
	$(CXX) $(DEF) $^ $(LDFLAGS) -o $@

encryption/obj/%.o: encryption/%.c
	$(CC) $(DEF) $(INC) -c $(CFLAGS) $< -o $@

secube/obj/%.o: secube/%.c
	$(CC) $(DEF) $(INC) -c $(CFLAGS) $< -o $@
	
util/obj/%.o: util/%.cpp
	$(CXX) $(DEF) $(INC) -c $(CXXFLAGS) $< -o $@

proxy/obj/%.o: proxy/provider/%.cpp
	$(CXX) $(DEF) $(INC) -c $(CXXFLAGS) $< -o $@

proxy/obj/%.o: proxy/%.cpp
	$(CXX) $(DEF) $(INC) -c $(CXXFLAGS) $< -o $@

selinkgw/obj/%.o: selinkgw/%.cpp
	$(CXX) $(DEF) $(INC) -c $(CXXFLAGS) $< -o $@

dirs:
	mkdir -p bin
	mkdir -p encryption/obj
	mkdir -p secube/obj
	mkdir -p util/obj
	mkdir -p proxy/obj
	mkdir -p selinkgw/obj

clean:
	rm -f bin/$(EXE_SELINKGW)
	rm -f encryption/obj/*.o
	rm -f secube/obj/*.o
	rm -f util/obj/*.o
	rm -f proxy/obj/*.o
	rm -f selinkgw/obj/*.o

install:
	mkdir -p $(PREFIX)
	install -m 0755 bin/$(EXE_SELINKGW) $(PREFIX)/$(EXE_SELINKGW)
	if [ ! -e $(PREFIX)/keys.json ]; then install -m 600 example/keys.json $(PREFIX)/keys.json ; fi
	if [ ! -e $(PREFIX)/selinkgw.json ]; then install -m 600 example/selinkgw.json $(PREFIX)/selinkgw.json ; fi
	install -m 0644 example/$(SELINKGW).service /usr/lib/systemd/system/$(SELINKGW).service
	
.PHONY: dirs all clean install

