# Library description
DESCRIPTION.csengine = Crystal Space 3D engine

#-------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Library-specific help commands
LIBHELP += $(NEWLINE)echo $"  make csengine     Make the $(DESCRIPTION.csengine)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#-------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: csengine

all libs: csengine
csengine:
	$(MAKE_TARGET)

endif # ifeq ($(MAKESECTION),roottargets)

#-------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

vpath %.cpp libs/csengine libs/csengine/2d libs/csengine/basic \
  libs/csengine/colldet libs/csengine/light libs/csengine/objects \
  libs/csengine/polygon libs/csengine/scripts

CSENGINE.LIB = $(OUT)$(LIB_PREFIX)csengine$(LIB)
SRC.CSENGINE = $(wildcard libs/csengine/*.cpp libs/csengine/*/*.cpp)
OBJ.CSENGINE = $(addprefix $(OUT),$(notdir $(SRC.CSENGINE:.cpp=$O)))

endif # ifeq ($(MAKESECTION),postdefines)

#------------------------------------------------------------------ targets ---#
ifeq ($(MAKESECTION),targets)

.PHONY: csengine csengineclean

all: $(CSENGINE.LIB)
csengine: $(OUTDIRS) $(CSENGINE.LIB)
clean: csengineclean

$(CSENGINE.LIB): $(OBJ.CSENGINE)
	$(DO.STATIC.LIBRARY)

csengineclean:
	-$(RM) $(CSENGINE.LIB)

ifdef DO_DEPEND
$(OUTOS)csengine.dep: $(SRC.CSENGINE)
	$(DO.DEP) $(OUTOS)csengine.dep
endif

-include $(OUTOS)csengine.dep

endif # ifeq ($(MAKESECTION),targets)
