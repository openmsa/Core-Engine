##############################################################################
#
#    file                 : Make-default.mk
#    created              : Sat Mar 18 23:53:11 CET 2000
#
##############################################################################

# targets:

.PHONY : clean subdirs compil cleansubdirs dep all

ifndef SMS_BASE

error:
	@echo "SMS_BASE should be defined"
	@exit 1

endif

-include ${SMS_BASE}/Make-config


SOURCES_DIR  = source
SOURCES_CPPDIR  = source
OBJECTS_CPPDIR = object/$(ARCH)
OBJECTS_DIR  = object/$(ARCH)

# Objects files from sources
C_SOURCES = $(SOURCES:$(SOURCES_DIR)/%.pc=$(SOURCES_DIR)/%.c)
OBJECTS = $(C_SOURCES:$(SOURCES_DIR)/%.c=$(OBJECTS_DIR)/%.o)
OBJECTSCPP = $(CPPSOURCES:$(SOURCES_CPPDIR)/%.cpp=$(OBJECTS_CPPDIR)/%.o)
#OBJECTSCPP = $(patsubst $(SOURCES_CPPDIR)/%.cpp,$(OBJECTS_CPPDIR)/%.o,$(wildcard $(SOURCES_CPPDIR)/*.cpp))

CINCS = $(INCLUDES:%=-I../%/include)
PROC_INCLUDES = $(INCLUDES:%=include=../%/include)
EXT_LIBS = $(LIBS:%=lib%.a)
VPATH= $(LIBS:%=../%/object/$(ARCH))

# Sub-directories recursion
define recursedirs
for Dir in $$RecurseDirs ;\
do ${MAKE} -C $$Dir $$RecurseFlags SMS_BASE=${SMS_BASE} MAKE_DEFAULT=${MAKE_DEFAULT} SB_ROOT=${SB_ROOT}; \
if [ $$? != 0 ]; then exit 1; fi ; \
done
endef

# Serialize targets
define serialize
for Target in $$Targets ;\
do ${MAKE} $$Target $$RecurseFlags SMS_BASE=${SMS_BASE} MAKE_DEFAULT=${MAKE_DEFAULT} SB_ROOT=${SB_ROOT}; \
if [ $$? != 0 ]; then exit 1; fi ; \
done
endef

# Main target
default: 
	@Targets="precompil dep compil";\
	${serialize}

all: 
	@Targets="precompil dep compil phpcheck";\
	${serialize}

clean:
	@Targets="realclean preclean";\
	RecurseFlags="NODEPS=Y";\
	${serialize}

rebuild_all:
	@Targets="clean all";\
	${serialize}
	
	
ifdef SOURCES
ALLSOURCES = $(SOURCES)
endif

ifdef CPPSOURCES
ALLSOURCES = $(ALLSOURCES) $(CPPSOURCES)
endif


ifdef SOURCES_NOT_WORKING

$(OBJECTS_DIR)/.depend: $(SOURCES_DIR)/*.c
	mkdir -p $(OBJECTS_DIR)
	gcc -E -M -D__DEPEND__ $(CFLAGS) $(CINCS) $< >$(OBJECTS_DIR)/.depend
	perl -pi -e 's,^(\S+:),'"$(OBJECTS_DIR)"'/$$1,' $(OBJECTS_DIR)/.depend

dep: $(OBJECTS_DIR)/.depend

else

dep: ;

endif


ifdef CPPSOURCES

$(OBJECTS_CPPDIR)/%.o : $(SOURCES_CPPDIR)/%.cpp
	mkdir -p $(OBJECTS_CPPDIR)
	-rm -f $@
	$(CPP) -c $< $(CPPFLAGS) $(CINCS) -o $@

endif


ifdef SOURCES

# Object file production rule from a C file
$(OBJECTS_DIR)/%.o : $(SOURCES_DIR)/%.c
	mkdir -p $(OBJECTS_DIR)
	-rm -f $@
	$(CC) -c $< $(CFLAGS) $(CINCS) -o $@


# C file production rule from a Pro*C file
$(SOURCES_DIR)/%.c : $(SOURCES_DIR)/%.pc
	-rm -f $@
	$(PROC) iname=$< oname=$@ $(PROCFLAGS)

endif


ifdef ALLSOURCES
cleanobjs:
	-rm -f ${OBJECTS} ${OBJECTSCPP} $(OBJECTS_DIR)/.depend

ifndef NODEPS
#include .depend only if exists
ifeq ($(OBJECTS_DIR)/.depend,$(wildcard $(OBJECTS_DIR)/.depend))
-include $(OBJECTS_DIR)/.depend
endif
endif

else

cleanobjs: ;

endif



precompil: presubdirs ${PRECOMPIL}

compil: subdirs ${LIBRARY} ${DAEMON} ${PROGRAM} ${OTHER}

preclean: cleanpresubdirs

realclean: cleansubdirs cleanlibs cleanprogs cleandaemons cleanobjs cleangarbage

phpcheck: phpchecksubdirs phpchk

ifdef LIBRARY

$(LIBRARY) : dep $(OBJECTS_DIR)/$(LIBRARY)

# Library production
$(OBJECTS_DIR)/$(LIBRARY) : $(OBJECTS) $(OBJECTSCPP)
	-rm -f $@
	$(LIBRARIAN) $(LIBRARIAN_OPT) $@ $^
	ranlib $@

cleanlibs:
	-rm -f $(OBJECTS_DIR)/${LIBRARY}
	
else

cleanlibs: ;
 
endif


ifeq ($(OBJECTSCPP),"")
  LINKER = $(CC)
else
  LINKER = $(CPP)
endif


ifdef PROGRAM

${PROGRAM}: $(OBJECTS_DIR)/${PROGRAM}

$(OBJECTS_DIR)/${PROGRAM} : $(OBJECTS) $(OBJECTSCPP) $(EXT_LIBS)
	-rm -f $@
	$(LINKER) $^ $(LDFLAGS) -o $@ 

cleanprogs:
	-rm -f $(OBJECTS_DIR)/${PROGRAM}

else

cleanprogs: ;

endif

ifdef DAEMON

${DAEMON} : $(OBJECTS_DIR)/${DAEMON}

$(OBJECTS_DIR)/${DAEMON} : $(OBJECTS) $(OBJECTSCPP) $(EXT_LIBS)
	@echo "OBJECTS    : $(OBJECTS)" 
	@echo "OBJECTSCPP : $(OBJECTSCPP)"
	-rm -f $@
	$(LINKER) $^ $(LDFLAGS) -o $@ 

cleandaemons:
	-rm -f $(OBJECTS_DIR)/${DAEMON}

else

cleandaemons: ;

endif


ifdef SUBDIRS

subdirs: 
	@RecurseDirs="${SUBDIRS}" ; \
	RecurseFlags="compil" ; \
	${recursedirs}

cleansubdirs:
	@RecurseDirs="${SUBDIRS}" ; \
	RecurseFlags="clean" ; \
	${recursedirs}

phpchecksubdirs: 
	@RecurseDirs="${SUBDIRS}" ; \
	RecurseFlags="phpcheck" ; \
	${recursedirs}

else

subdirs: ;
cleansubdirs: ;
phpchecksubdirs: ;

endif

ifdef PRESUBDIRS

presubdirs: 
	@RecurseDirs="${PRESUBDIRS}" ; \
	RecurseFlags="precompil" ; \
	${recursedirs}

cleanpresubdirs:
	@RecurseDirs="${PRESUBDIRS}" ; \
	RecurseFlags="clean" ; \
	${recursedirs}

else

presubdirs: ;
cleanpresubdirs: ;

endif

ifdef GARBAGE

cleangarbage:
	-rm -f ${GARBAGE}

else

cleangarbage: ;

endif

ifdef PHP_SOURCES

phpchk:
	@echo ${PHP_SOURCES} | xargs -n1 php -ddisplay_errors=1 -derror_reporting=8191 -l

else

phpchk: ;

endif

	
