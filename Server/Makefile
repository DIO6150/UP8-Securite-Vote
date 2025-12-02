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
CFLAGS = -Wall -Wextra -O3 
CXXFLAGS = -std=c++20
CPPFLAGS = -I. -Iheaders
LDFLAGS = -lm
# définition des fichiers et dossiers
PACKNAME = Build
PROGNAME = build.out
VERSION = 1.0
distdir = $(PACKNAME)_$(PROGNAME)-$(VERSION)
HEADERS = headers/colors.hpp headers/server.hpp headers/server_hooks.hpp headers/utils.hpp headers/command_status.hpp headers/string_pretty.hpp
SOURCES =  main.cpp server.cpp colors.cpp
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
        CXXFLAGS += -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
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
