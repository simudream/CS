# Library description
DESCRIPTION.csgeom = Crystal Space geometry library

#-------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Library-specific help commands
LIBHELP += $(NEWLINE)echo $"  make csgeom       Make the $(DESCRIPTION.csgeom)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#-------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: csgeom

all libs: csgeom
csgeom:
	$(MAKE_TARGET)

endif # ifeq ($(MAKESECTION),roottargets)

#-------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp libs/csgeom

CSGEOM.LIB = $(OUT)$(LIB_PREFIX)csgeom$(LIB)
SRC.CSGEOM = $(wildcard libs/csgeom/*.cpp)
OBJ.CSGEOM = $(addprefix $(OUT),$(notdir $(SRC.CSGEOM:.cpp=$O)))

endif # ifeq ($(MAKESECTION),postdefines)

#------------------------------------------------------------------ targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: csgeom csgeomclean

all: $(CSGEOM.LIB)
csgeom: $(OUTDIRS) $(CSGEOM.LIB)
clean: csgeomclean

$(CSGEOM.LIB): $(OBJ.CSGEOM)
	$(DO.STATIC.LIBRARY)

csgeomclean:
	-$(RM) $(CSGEOM.LIB)

ifdef DO_DEPEND
$(OUTOS)csgeom.dep: $(SRC.CSGEOM)
	$(DO.DEP) $(OUTOS)csgeom.dep
endif

-include $(OUTOS)csgeom.dep

endif # ifeq ($(MAKESECTION),targets)
