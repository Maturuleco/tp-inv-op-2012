ROOT_DIR = /opt/ibm/ILOG/CPLEX_Studio124

CPLEX_LIB       = -L$(ROOT_DIR)/cplex/lib/x86-64_sles10_4.1/static_pic/ -L$(ROOT_DIR)/cplex/lib/x86-64_sles10_4.1/static_pic/ -L$(ROOT_DIR)/concert/lib/x86-64_sles10_4.1/static_pic/
LP_LIBS         = $(CPLEX_LIB)
INC_DIR		= -I$(ROOT_DIR)/cplex/include/ -I$(ROOT_DIR)/concert/include/

CCC = g++

CCFLAGS = -m64 -O2 -fPIC -fexceptions -DNDEBUG -DIL_STD

LDFLAGS = $(LP_LIBS) -lc -lm

EXTRAFLAGS = -lpthread -lm -lnsl -lilocplex -lconcert -lcplex

SOURCES = resolucion.cpp

OBJECTS = $(SOURCES:.cpp=.o)

TARGET = resolucion

all: $(OBJECTS) 
	$(CCC) -o $(TARGET) $(OBJECTS) $(CCFLAGS) $(LDFLAGS) $(EXTRAFLAGS)

#
# produce the .o files from the .cc and .c files
#

%.o: %.cpp
	$(CCC) $(CCFLAGS) $(INC_DIR) -c $<

clean:
	rm -f $(OBJECTS)
	rm -f $(TARGET)
