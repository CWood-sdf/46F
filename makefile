# VEXcode makefile 2019_03_26_01

# show compiler output
VERBOSE = 0

# include toolchain options
include vex/mkenv.mk

# location of the project source cpp and c files
SRC_C  = $(wildcard src/*.cpp) 
SRC_C += $(wildcard src/*/*.cpp) 
SRC_C += $(wildcard include/*.cpp)
SRC_C += $(wildcard include/*/*.cpp)
SRC_C += $(wildcard include/*/*/*.cpp)

OBJA = $(addprefix $(BUILD)/, $(addsuffix .o, $(basename $(SRC_C))) )
OBJ = $(addprefix $(BUILD)/, $(addsuffix .o, $(basename $(SRC_C))) )

# location of include files that c and cpp files depend on
SRC_H  = $(wildcard include/*.h)
SRC_H += $(wildcard include/*/*.h)
SRC_H += $(wildcard include/*/*/*.h)
SRC_H += $(wildcard include/*/*/*/*.h)
SRC_H += $(wildcard include/*/*/*/*/*.h)
SRC_H += $(wildcard include/*/*/*/*/*/*.h)
SRC_H += $(wildcard include/*/*/*/*/*/*/*.h)
SRC_H += $(wildcard include/*/*/*/*/*/*/*/*.h)


# additional dependancies
SRC_A  = makefile

# project header file locations
INC_F  = include . include/lvgl include/lvgl/src include/Odometry

# headers needed to use library
LV_SRC_H += $(wildcard include/lvgl/lvgl.h) 
LV_SRC_H += $(wildcard include/lvgl/src/*.h) 
LV_SRC_H += $(wildcard include/lvgl/src/*/*.h)
LV_SRC_H += include/lv_conf.h
LV_SRC_H += lvgl_inc.h

LV_DST_H = $(addprefix $(BUILD)/include/, $(LV_SRC_H) )
LV_DST_H += $(BUILD)/include/lv_conf.h $(BUILD)/include/lvgl_inc.h

$(BUILD)/include/%: %
	$(Q)$(MKDIR)
	$(Q) $(call copyfile,$^, $@)

vpath %.h include/lvgl/ include/

# override proj name
PROJECTLIB = lib46f

# build targets
all: $(BUILD)/$(PROJECT).bin $(BUILD)/$(PROJECTLIB).a inc

# copy lvgl header files
.PHONY: inc
inc: $(LV_DST_H)
	$(ECHO) "Copy headers to build folder"

# include build rules
include vex/mkrules.mk
