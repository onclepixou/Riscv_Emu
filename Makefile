CC=g++

SRCDIR  = src
OBJDIR  = obj
BINDIR  = .
LINCDIR = inc

INCDIR= -I $(LINCDIR) -I /usr/include -I /usr/local/include


CFLAGS=  -std=c++11 -g  $(INCDIR)

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(LINCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)



$(OBJDIR)/%.o: $(SRCDIR)/%.cpp 
	$(CC) $(CFLAGS) -c $< -o $@ 


Emulator: $(OBJECTS)
	$(CC) $(CFLAGS)  -o $(BINDIR)/$@ $^ 


.PHONY: clean

clean :
	rm $(OBJDIR)/*.o

