ifeq ($(OS),Windows_NT)
# Target name
TARGET = arm_2d_rgb565_aci_fvp.dll
# Include directories
INCLUDES = 
else
# Target name
TARGET = arm_2d_rgb565_aci_fvp.o
# Include directories
INCLUDES = -I$(PVLIB_HOME)/include
endif


# C++ compiler
CC = g++
# Linker
LD = g++

# C++ flags
CXXFLAGS = -O3 -Wall -Wall -Wextra -pedantic -fPIC -Wno-unused-parameter -std=c++14
# Dependency-generation flags
DEPFLAGS = -MMD -MP
# linker flags
LDFLAGS = -shared


# Build directories
BUILD_DIR=build
SRCS 	:= arm_2d_rgb565_aci_fvp.cpp ACILibrary.cpp
OBJS 	:= $(SRCS:%.cpp=%.o)
OBJSDIR := $(addprefix $(BUILD_DIR)/, $(OBJS))

# Include compiler-generated dependency rules
DEPS 	:= $(OBJS:%.o=/%.d)
DEPSDIR := $(addprefix $(BUILD_DIR)/, $(DEPS))

.PHONY: clean all

all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJSDIR)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.cpp
	$(CC) $(CXXFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

clean:
	@$(RM) -rf $(BUILD_DIR)

-include $(DEPSDIR)