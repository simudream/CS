# Library description
DESCRIPTION.csparser = Crystal Space world/model parser library

#-------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Library-specific help commands
LIBHELP += $(NEWLINE)echo $"  make csparser     Make the $(DESCRIPTION.csparser)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#-------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: csparser

all libs: csparser
csparser:
	$(MAKE_TARGET)

endif # ifeq ($(MAKESECTION),roottargets)

#-------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp libs/csparser

CSPARSER.LIB = $(OUT)$(LIB_PREFIX)csparser$(LIB)
SRC.CSPARSER = $(wildcard libs/csparser/*.cpp)
OBJ.CSPARSER = $(addprefix $(OUT),$(notdir $(SRC.CSPARSER:.cpp=$O)))

endif # ifeq ($(MAKESECTION),postdefines)

#------------------------------------------------------------------ targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: csparser csparserclean

all: $(CSPARSER.LIB)
csparser: $(OUTDIRS) $(CSPARSER.LIB)
clean: csparserclean

$(CSPARSER.LIB): $(OBJ.CSPARSER)
	$(DO.STATIC.LIBRARY)

csparserclean:
	-$(RM) $(CSPARSER.LIB)

ifdef DO_DEPEND
$(OUTOS)csparser.dep: $(SRC.CSPARSER)
	$(DO.DEP) $(OUTOS)csparser.dep
endif

-include $(OUTOS)csparser.dep

endif # ifeq ($(MAKESECTION),targets)
