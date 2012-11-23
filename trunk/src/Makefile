SYSTEM     = x86-64_sles10_4.1
LIBFORMAT  = static_pic

#------------------------------------------------------------
#
# When you adapt this makefile to compile your CPLEX programs
# please copy this makefile and set CPLEXDIR and CONCERTDIR to
# the directories where CPLEX and CONCERT are installed.
#
#------------------------------------------------------------

CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio124/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio124/concert

# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = g++ -O0
CC  = gcc -O0

# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD
COPT  = -m64 -fPIC -fno-strict-aliasing 

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread
CLNFLAGS  = -L$(CPLEXLIBDIR) -lcplex -lm -pthread


all:
	make all_c
	make all_cpp

execute: all
	make execute_c
	make execute_cpp

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CFLAGS  = $(COPT)  -I$(CPLEXINCDIR)
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 

# ------------------------------------------------------------
#
# The examples
#
# ------------------------------------------------------------

SOURCES = resolucion.cpp problemaCPLEX.cpp

OBJECTS = $(SOURCES:.cpp=.o)

TARGET = resolucion

all: $(OBJECTS) 
	$(CCC) $(CCFLAGS) $(OBJECTS) -o $(TARGET) $(CCLNFLAGS)

#
# produce the .o files from the .cc and .c files
#

%.o: %.cpp
	$(CCC) $(CCFLAGS) -c $<

clean:
	rm -rf *.o *~
	rm -rf $(TARGET)
	rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log *.clp

#lpex2: lpex2.o
#	$(CC) $(CFLAGS) lpex2.o -o lpex2 $(CLNFLAGS)
#lpex2.o: $(EXSRCC)/lpex2.c
#	$(CC) -c $(CFLAGS) $(EXSRCC)/lpex2.c -o lpex2.o

#facility: facility.o
#	$(CCC) $(CCFLAGS) facility.o -o facility $(CCLNFLAGS)
#facility.o: $(EXSRCCPP)/facility.cpp
#	$(CCC) -c $(CCFLAGS) $(EXSRCCPP)/facility.cpp -o facility.o

# Local Variables:
# mode: makefile
# End:
