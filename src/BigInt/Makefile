#  Makefile 
#  Auteur : Farès BELHADJ
#  Email  : amsi@up8.edu
#  Date   : 28/04/2020
#  Modifié par Neil Bel Hadj pour utiliser cpp et g++
# définition des commandes utilisées
CC = gcc
CXX = g++
ECHO = echo
RM = rm -f
TAR = tar
ZIP = zip
MKDIR = mkdir
CHMOD = chmod
CP = rsync -R
# déclaration des options du compilateur
CFLAGS = -Wall -O3
CXXFLAGS = -Wall -O3 -std=c++20
CPPFLAGS = -I.
LDFLAGS = -lm
# définition des fichiers et dossiers
PACKNAME = Bigint
PROGNAME = bigint
VERSION = 1.0
distdir = $(PACKNAME)_$(PROGNAME)-$(VERSION)
HEADERS = big_int.hpp
SOURCES = main.cpp
OBJ1 = $(SOURCES:.cpp=.o)
OBJ = $(OBJ1:.c=.o)
DISTFILES = $(SOURCES) Makefile $(HEADERS) $(EXTRAFILES)
# Traitements automatiques pour ajout de chemins et options (ne pas modifier)
ifneq (,$(shell ls -d /usr/local/include 2>/dev/null | tail -n 1))
	CPPFLAGS += -I/usr/local/include
endif
ifneq (,$(shell ls -d /opt/local/include 2>/dev/null | tail -n 1))
	CPPFLAGS += -I/opt/local/include
endif
ifneq (,$(shell ls -d $(HOME)/local/include 2>/dev/null | tail -n 1))
	CPPFLAGS += -I$(HOME)/local/include
endif
ifneq (,$(shell ls -d /usr/local/lib 2>/dev/null | tail -n 1))
	LDFLAGS += -L/usr/local/lib
endif
ifneq (,$(shell ls -d /opt/local/lib 2>/dev/null | tail -n 1))
	LDFLAGS += -L/opt/local/lib
endif
ifneq (,$(shell ls -d $(HOME)/local/lib 2>/dev/null | tail -n 1))
	LDFLAGS += -L$(HOME)/local/lib
endif
ifeq ($(shell uname),Darwin)
	MACOSX_DEPLOYMENT_TARGET = 10.8
        CFLAGS += -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
else
        LDFLAGS += -lGL
endif
all: $(PROGNAME)
$(PROGNAME): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(PROGNAME)
%.o: %.c $(HEADERS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
%.o: %.cpp $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
dist: distdir
	$(CHMOD) -R a+r $(distdir)
	$(TAR) zcvf $(distdir).tgz $(distdir)
	$(RM) -r $(distdir)
zip: distdir
	$(CHMOD) -R a+r $(distdir)
	$(ZIP) -r $(distdir).zip $(distdir)
	$(RM) -r $(distdir)
distdir: $(DISTFILES)
	$(RM) -r $(distdir)
	$(MKDIR) $(distdir)
	$(CHMOD) 777 $(distdir)
	$(CP) $(DISTFILES) $(distdir)
clean:
	@$(RM) -r $(PROGNAME) $(OBJ) *~ $(distdir).tgz $(distdir).zip gmon.out	\
