NAME = libc.a

CXX = i686-pc-os-g++
AR = i686-pc-os-ar

STANDARD_FLAGS = -g -std=c++17 -nostdinc -nostdlib -nodefaultlibs -ffreestanding -masm=intel
WARNING_FLAGS = -Wall -Wextra -Wundef -Wcast-qual -xc++

CCFLAGS = -MMD -MP $(STANDARD_FLAGS) $(WARNING_FLAGS)
LIBS = -Iinclude -I..

CRTBEGIN_OBJ = $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ = $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)

OBJS = \
	src/stdio.o \
	src/stdlib.o \
	src/string.o \

DEPS = $(OBJS:%.o=%.d)

LINK_LIST = \
	$(CRTBEGIN_OBJ) \
	$(CRTEND_OBJ) \
	$(OBJS) \

all: $(NAME)

$(NAME): $(OBJS)
	@echo "AR $@"; $(AR) $(ARFLAGS) $@ $?

%.o: %.cpp
	@echo "CXX $<"; $(CXX) $(CCFLAGS) $(LIBS) -c -o $@ $<

-include $(DEPS)

.PHONY: clean
clean:
	@echo "CLEAN $(NAME) $(OBJS) $(DEPS)"; rm -f $(NAME) $(OBJS) $(DEPS)
