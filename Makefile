CPP_FILES:=$(wildcard *.cpp)
OBJ_FILES:=$(patsubst %.cpp,%.o,$(CPP_FILES))
DEP_FILES:=deps.d
PROGS:=$(patsubst %.cpp,%,$(CPP_FILES))
CPPFLAGS:=-O2 -MMD -MF $(DEP_FILES)

all: $(PROGS)

-include $(DEP_FILES)

$(PROGS): % : %.o
	$(CXX) -o $@ $< $(CPPFLAGS)

$(OBJ_FILES): %.o : %.cpp
	$(CXX) -c $< $(CPPFLAGS)

clean:
	$(RM) $(DEP_FILES) $(OBJ_FILES) $(PROGS)
