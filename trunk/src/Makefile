SYSTEM_64	= x86-64_sles10_4.1
SYSTEM_32	= x86_sles10_4.1
SYSTEM		= $(SYSTEM_64)
LIBFORMAT	= static_pic

CPLEXDIR	= /opt/ibm/ILOG/CPLEX_Studio124/cplex
CONCERTDIR	= /opt/ibm/ILOG/CPLEX_Studio124/concert

# ---------------------------------------------------------------------
# Compiler selection 
# ---------------------------------------------------------------------

CCC = g++ -O0

# ---------------------------------------------------------------------
# Compiler options 
# ---------------------------------------------------------------------

CCOPT_64	= -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD
CCOPT_32	= -m32 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD
CCOPT		= $(CCOPT_64)

# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXLIBDIR		= $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR	= $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread

CONCERTINCDIR	= $(CONCERTDIR)/include
CPLEXINCDIR		= $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 

# ------------------------------------------------------------
# Makes
# ------------------------------------------------------------

SOURCES	= resolucion.cpp problemaCPLEX.cpp covers.cpp

OBJECTS	= $(SOURCES:.cpp=.o)

TARGET	= resolucion


all: $(OBJECTS) 
	$(CCC) $(CCFLAGS) $(OBJECTS) -o $(TARGET) $(CCLNFLAGS)

32:
	$(MAKE) all CCOPT='$(CCOPT_32)' SYSTEM='$(SYSTEM_32)'

64:
	$(MAKE) all CCOPT='$(CCOPT_64)' SYSTEM='$(SYSTEM_64)'

%.o: %.cpp
	$(CCC) $(CCFLAGS) -c $<

clean:
	rm -rf *.o *~
	rm -rf $(TARGET)
	rm -rf *.mps *.ord *.sos *.lp *.sav *.net *.msg *.log *.clp
