INCDIR = -I. -I$(RMANHOME)/include
DBG    = -g3 -gdwarf-2 -O0 -rdynamic
OPT    = -fPIC -shared -std=c++11
CPP    = g++
SLIB   = $(RMANHOME)/share/rasdaman/udf
CFLAGS = $(DBG) $(OPT) $(INCDIR) -Wl,-soname
LINK   = -lm -ldl -L$(RMANHOME)/lib -lrasodmg -lraslib

all: dtm_hillshade

dtm_hillshade: dtm_hillshade.cc
	$(CPP) $(CFLAGS),$(SLIB)/$@.so -o $(SLIB)/$@.so $@.cc $(LINK)

clean:
	/bin/rm -f *.o *.so
