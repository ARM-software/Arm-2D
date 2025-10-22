#======================================================================================================================#
# Source code, Include path and predefined macros                                                                      #
#======================================================================================================================#
CC_SRC  := $(wildcard *.c)
CC_SRC  += $(wildcard platform/*.c)
CC_SRC  += $(wildcard platform/math/*.c)
CC_SRC  += $(wildcard ../../Library/Source/*.c)
CC_SRC  += $(wildcard ../../Helper/Source/*.c)
CC_SRC  += $(wildcard ../../Helper/asset/*.c)
CC_SRC  += $(wildcard ../../examples/common/asset/*.c)
CC_SRC  += $(wildcard ../../examples/common/benchmark/*.c)
CC_SRC  += $(wildcard ../../examples/common/controls/*.c)
CC_SRC  += $(wildcard ../../examples/common/controls/qrcode_box/*.c)
CC_SRC  += $(wildcard ../../examples/common/opcodes/*.c)
CC_SRC  += $(wildcard ../../examples/common/scenes/fitness/*.c)
CC_SRC  += $(wildcard ../../examples/common/scenes/meter/*.c)
CC_SRC  += $(wildcard ../../examples/common/scenes/watch/*.c)
CC_SRC  += $(wildcard ../../examples/common/scenes/alarm_clock/*.c)
CC_SRC  += $(wildcard ../../examples/common/scenes/histogram/*.c)
CC_SRC  += $(wildcard ../../examples/common/scenes/fan/*.c)
CC_SRC  += $(wildcard ../../examples/common/scenes/bubble_charging/*.c)
CC_SRC  += $(wildcard ../../examples/common/scenes/animate_background/*.c)
CC_SRC  += $(wildcard ../../examples/common/scenes/knob/*.c)
CC_SRC  += $(wildcard ../../examples/common/loader/*.c)
CC_SRC  += $(wildcard ../../examples/common/loader/tjpgd_loader/*.c)
CC_SRC  += $(wildcard ../../examples/common/loader/tjpgd_loader/tjpgd/*.c)
CC_SRC  += $(wildcard ../../examples/common/loader/zjpgd_loader/*.c)
CC_SRC  += $(wildcard ../../examples/common/loader/zjpgd_loader/zjpgd/*.c)
CC_SRC  += $(wildcard ../../examples/common/loader/freetype_loader/*.c)
CC_SRC  += $(wildcard ../../examples/demos/*.c)
CC_SRC  += $(wildcard ../../examples/demos/monochrome/*.c)

CC_INC  := .
CC_INC  += platform
CC_INC  += platform/math
CC_INC  += platform/math/dsp
CC_INC  += ../../Library/Include
CC_INC  += ../../Helper/Include
CC_INC  += ../../Helper/built_in
CC_INC  += ../../examples/common/benchmark
CC_INC  += ../../examples/common/controls
CC_INC  += ../../examples/common/controls/qrcode_box
CC_INC  += ../../examples/common/opcodes
CC_INC  += ../../examples/common/loader
CC_INC  += ../../examples/common/loader/tjpgd_loader
CC_INC  += ../../examples/common/loader/zjpgd_loader
CC_INC  += ../../examples/common/loader/freetype_loader
CC_INC  += ../../examples/common/loader/freetype_loader/freetype/include
CC_INC  += ../../examples/common/scenes/fitness
CC_INC  += ../../examples/common/scenes/meter
CC_INC  += ../../examples/common/scenes/watch
CC_INC  += ../../examples/common/scenes/alarm_clock
CC_INC  += ../../examples/common/scenes/histogram
CC_INC  += ../../examples/common/scenes/fan
CC_INC  += ../../examples/common/scenes/bubble_charging
CC_INC  += ../../examples/common/scenes/animate_background
CC_INC  += ../../examples/common/scenes/knob
CC_INC  += ../../examples/demos
CC_INC  += ../../examples/demos/monochrome

CC_DEF  := ARM_SECTION\(x\)=
CC_DEF  += _RTE_
CC_DEF  += VT_SCALING_RATIO=1
CC_DEF  += FT2BUILD_H_

#================================== DO NOT MODIFY UNLESS YOU KNOW THE CONSEQUENCES ====================================#
ifeq ($(OS),Windows_NT)
	CROSS   := i686-w64-mingw32-
	CC      := $(CROSS)gcc
	STRIP   := $(CROSS)strip
	SIZE    := $(CROSS)size
	OUT     := build/arm_2d.exe
	RM      := cmd /c rd /s /q

	CC_INC  += sdl2/32/include
	LD_INC  := sdl2/32/lib/x86
	LD_LIB  := SDL2main SDL2
	CCFLAG  +=  -w
	LDFLAG  +=  -Wl,--warn-common
	LDFLAG  +=  -Wl,--gc-sections
app: build/SDL2.dll $(OUT)

else ifeq ($(shell uname), Darwin)
    # Code for OS X
	CC      := $(CROSS)gcc
	STRIP   := $(CROSS)strip
	SIZE    := $(CROSS)size
	OUT     := build/arm_2d
	RM      := rm -rf
	CCFLAG  += -D_THREAD_SAFE
	CCFLAG  += -ffast-math -Wno-deprecated-ofast -Wno-unknown-warning-option
	CC_INC  += /opt/homebrew/include
	LD_INC  += /opt/homebrew/lib
	LD_LIB  := SDL2 SDL2main
app: $(OUT)

else
	CC      := $(CROSS)gcc
	STRIP   := $(CROSS)strip
	SIZE    := $(CROSS)size
	OUT     := build/arm_2d
	RM      := rm -rf
	CCFLAG  +=  -w
	LD_LIB  := SDL2 m
	LDFLAG  +=  -Wl,--warn-common
	LDFLAG  +=  -Wl,--gc-sections -lm
app: $(OUT)

endif

#======================================================================================================================#
# Common compilation options                                                                                           #
#======================================================================================================================#
CCFLAG  +=  -std=gnu11 -MMD -g
CCFLAG  +=  -ffunction-sections -fdata-sections
CCFLAG  +=  -fno-ms-extensions
CCFLAG  +=  -Wno-macro-redefined

CCFLAG  +=  -O0
//CCFLAG  +=  -flto
//LDFLAG  +=  -flto
LDFLAG  +=  -lpthread

#================================== DO NOT MODIFY UNLESS YOU KNOW THE CONSEQUENCES ====================================#
.DEFAULT_GOAL = all
_Comma := ,

ifeq (${wildcard obj},)
$(shell mkdir obj)
endif

ifeq (${wildcard build},)
$(shell mkdir build)	
endif

CC_OBJ := $(addprefix obj/,$(addsuffix .o,$(notdir $(CC_SRC))))

$(foreach src,$(CC_SRC),$(eval obj/$(notdir $(src)).o : $(src)))

-include $(CC_OBJ:%.o=%.d)

CCSuffix := $(CCFLAG) $(addprefix -I,$(CC_INC)) $(addprefix -D,$(CC_DEF))
LDObject := $(CC_OBJ) $(addprefix -l,$(LD_LIB))
LDSuffix := $(LDFLAG) $(addprefix -Wl$(_Comma)-L,$(LD_INC))

#================================== DO NOT MODIFY UNLESS YOU KNOW THE CONSEQUENCES ====================================#
.PHONY: all
all: app
	@echo Build Completed.

#----------------------------------------------------------------------------------------------------------------------#
build/SDL2.dll:
	@echo Copy SDL2.dll
	@-cmd /c copy sdl2\32\bin\SDL2.dll build\SDL2.dll

#----------------------------------------------------------------------------------------------------------------------#
$(OUT): $(CC_OBJ)
	@echo Linking $(OUT) ...
	@$(CC) $(LDObject) $(LDSuffix) -o $(OUT)

#----------------------------------------------------------------------------------------------------------------------#
obj/%.c.o:
	@echo Compile $(@F:.o=) ...
	@$(CC) -c $< -o $@ $(CCSuffix)

#----------------------------------------------------------------------------------------------------------------------#
.PHONY: clean
clean:
	@-$(RM) build
	@-$(RM) obj
	@echo Clean Completed.
