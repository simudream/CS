# This is a subinclude file used to define the rules needed
# to build the GGI 2D driver -- ggi2d (ggi2d.so)

# Driver description
DESCRIPTION.ggi2d = Crystal Space GGI 2D driver

#------------------------------------------------------------- rootdefines ---#
ifeq ($(MAKESECTION),rootdefines)

# Driver-specific help commands
DRIVERHELP += \
  $(NEWLINE)echo $"  make ggi2d        Make the $(DESCRIPTION.ggi2d)$"

endif # ifeq ($(MAKESECTION),rootdefines)

#------------------------------------------------------------- roottargets ---#
ifeq ($(MAKESECTION),roottargets)

.PHONY: ggi2d ggi2dclean

all plugins drivers drivers2d: ggi2d

ggi2d:
	$(MAKE_TARGET) MAKE_DLL=yes
ggi2dclean:
	$(MAKE_CLEAN)

endif # ifeq ($(MAKESECTION),roottargets)

#------------------------------------------------------------- postdefines ---#
ifeq ($(MAKESECTION),postdefines)

LIBS._GGI2D = -lggi

# The GGI 2D driver
ifeq ($(USE_SHARED_PLUGINS),yes)
  GGI2D=$(OUTDLL)ggi2d$(DLL)
  LIBS.GGI2D+=$(LIBS._GGI2D)
  DEP.GGI2D=$(CSGEOM.LIB) $(CSUTIL.LIB) $(CSSYS.LIB)
else
  GGI2D=$(OUT)$(LIB_PREFIX)ggi2d$(LIB)
  DEP.EXE+=$(GGI2D)
  LIBS.EXE+=$(LIBS._GGI2D)
  CFLAGS.STATIC_SCF+=$(CFLAGS.D)SCL_GGI2D
endif
DESCRIPTION.$(GGI2D) = $(DESCRIPTION.ggi2d)
SRC.GGI2D = $(wildcard plugins/video/canvas/ggi/*.cpp $(SRC.COMMON.DRV2D))
OBJ.GGI2D = $(addprefix $(OUT),$(notdir $(SRC.GGI2D:.cpp=$O)))

endif # ifeq ($(MAKESECTION),postdefines)

#----------------------------------------------------------------- targets ---#
ifeq ($(MAKESECTION),targets)

vpath %.cpp plugins/video/canvas/ggi

.PHONY: ggi2d ggi2dclean

# Chain rules
clean: ggi2dclean

ggi2d: $(OUTDIRS) $(GGI2D)

$(GGI2D): $(OBJ.GGI2D)
#	$(DO.PLUGIN) $(LIBS.GGI2D)

$(OUT)%$O: plugins/video/canvas/ggi/%.cpp
	$(DO.COMPILE.CPP) $(CFLAGS.GGI2D)

$(GGI2D): $(OBJ.GGILIB2D) $(DEP.GGI2D)
	$(DO.PLUGIN) $(LIBS.GGI2D)

ggi2dclean:
	$(RM) $(GGI2D) $(OBJ.GGI2D) $(OUTOS)ggi2d.dep

ifdef DO_DEPEND
dep: $(OUTOS)ggi2d.dep
$(OUTOS)ggi2d.dep: $(SRC.GGI2D)
	$(DO.DEP)
else
-include $(OUTOS)ggi2d.dep
endif

endif # ifeq ($(MAKESECTION),targets)
