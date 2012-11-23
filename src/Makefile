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

# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT = -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

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

# Local Variables:
# mode: makefile
# End:
