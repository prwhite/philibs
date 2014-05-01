#!/usr/bin/env make

OBJECTS += pnimathtest.o

OBJECTS += ../src/pnibox2.o
OBJECTS += ../src/pnibox3.o
OBJECTS += ../src/pnicylinder.o
#OBJECTS += ../src/pnidoubletrait.o
OBJECTS += ../src/pnifloatmath.o
OBJECTS += ../src/pnifloattrait.o
OBJECTS += ../src/pnifrustum.o
OBJECTS += ../src/pnimath.o
OBJECTS += ../src/pnimathstream.o
OBJECTS += ../src/pnimatrix4.o
#OBJECTS += ../src/pnimatstack.o
OBJECTS += ../src/pniplane.o
OBJECTS += ../src/pnipolytope.o
OBJECTS += ../src/pniquat.o
OBJECTS += ../src/pnirand.o
OBJECTS += ../src/pniseg.o
OBJECTS += ../src/pnisphere.o
OBJECTS += ../src/pnivec2.o
OBJECTS += ../src/pnivec3.o
OBJECTS += ../src/pnivec4.o

TARGET = pnimathtest

CXXFLAGS += -std=c++11 -I../src
LD = c++ -std=c++11

# TARGET RULES

test: $(TARGET)
	$(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LOADLIBES) $(LDLIBS) -o $@

clean:
	rm $(OBJECTS) $(TARGET)

.PHONY: test
