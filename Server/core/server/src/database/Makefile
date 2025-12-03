CXX = g++
ECHO = echo
RM = rm -f
TAR = tar
ZIP = zip
MKDIR = mkdir
CHMOD = chmod
CP = rsync -R
# déclaration des options du compilateur
CXXFLAGS = -std=c++11 -Wall -Wextra  -O3 -fsanitize=address
CPPFLAGS = -I.
LDFLAGS = -lm -fsanitize=address -l sqlite3
# définition des fichiers et dossiers
PACKNAME = proj
PROGNAME = database
VERSION = 1.1
distdir = $(PACKNAME)_$(PROGNAME)-$(VERSION)
SOURCES = database.cpp
OBJ = $(SOURCES:.cpp=.o)
DISTFILES = $(SOURCES) Makefile $(HEADERS)
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

all: $(PROGNAME)
$(PROGNAME): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(PROGNAME)
%.o: %.cpp
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
	@$(RM) -r $(PROGNAME) $(OBJ) *~ $(distdir).tgz $(distdir).zip gmon.out db.sqlite3
