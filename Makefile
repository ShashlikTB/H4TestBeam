INC_DIR = ./
CXX		=g++
LD		=g++
CXXFLAGS	=-O2 -ggdb -std=gnu++0x -Wall -Wsign-compare
LDFLAGS		=-lz -lm  -Wl,--no-as-needed
SOFLAGS		=-fPIC -shared 
SHELL		=bash
###
SrcSuf        = cpp
HeadSuf       = h
ObjSuf        = o
DepSuf        = d
DllSuf        = so
StatSuf       = a
ExeSuf        = exe

## Packages	= Hodo2text

CppTestFiles=$(wildcard test/*.$(SrcSuf))
Packages=$(patsubst test/%.$(SrcSuf),%.$(ExeSuf),$(CppTestFiles) )

### ----- OPTIONS ABOVE ----- ####

#InfoLine = \033[01\;31m compiling $(1) \033[00m
InfoLine = compiling $(1)

BASEDIR=$(shell pwd)
BINDIR=$(BASEDIR)/bin
SRCDIR = $(BASEDIR)/src
HDIR = $(BASEDIR)/include
EXTLIBDIR = -L$(BASEDIR)/build/lib/ -lTB

### Main Target, first
.PHONY: all
all: info $(Packages) | $(BINDIR)

CXXFLAGS	+=`root-config --cflags`
LDFLAGS 	+=`root-config --libs`

LDFLAGS 	+= $(EXTLIBDIR)
LDBOOSTFLAGS 	=-lboost_program_options

.PRECIOUS:*.ObjSuf *.DepSuf *.DllSuf

Deps=$(patsubst %,$(BINDIR)/%.$(DepSuf),$(Objects) $(Packages) )

############### EXPLICIT RULES ###############

$(BINDIR):
	mkdir -p $(BINDIR)

info:
	@echo "--------------------------"
	@echo "Compile on $(shell hostname)"
	@echo "Packages are: $(Packages)"
	@echo "Objects are: $(Objects)"
	@echo "--------------------------"
	@echo "DEBUG:"

.PHONY: $(Packages) 
$(Packages): % : $(BINDIR)/% | $(BINDIR)
	@echo $(call InfoLine , $@ )

$(addprefix $(BINDIR)/,$(Packages)): $(BINDIR)/%.$(ExeSuf) : $(BASEDIR)/test/%.$(SrcSuf) | $(BINDIR)
	@echo $(call InfoLine , $@ )
	$(CXX) $(CXXFLAGS) -o $@ $< -I$(INC_DIR) -I$(HDIR) $(LDBOOSTFLAGS) $(LDFLAGS)


.PHONY: clean
clean:
	-rm -v bin/*.$(ExeSuf)

