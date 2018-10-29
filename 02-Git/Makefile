TARGET := rps
ODIR ?= build
OBJODIR := $(ODIR)/obj

# A simple variant is to prefix commands with $(Q) - that's useful
# for commands that shall be hidden in non-verbose mode.
#
#	$(Q)ln $@ :<
#
# To put more focus on warnings, be less verbose as default
# Use 'make V=1' to see the full commands
ifeq ("$(origin V)", "command line")
	BUILD_VERBOSE = $(V)
else
	BUILD_VERBOSE = 0
endif

ifeq ($(BUILD_VERBOSE),1)
	Q =
else
	Q = @
endif

# Do not print "Entering directory ...",
# but we want to display it when entering to the output directory
# so that IDEs/editors are able to understand relative filenames.
MAKEFLAGS += --no-print-directory

# Cross compiling and selecting different set of gcc/bin-utils
# ---------------------------------------------------------------------------
#
# CROSS_COMPILE specify the prefix used for all executables used
# during compilation. Only gcc and related bin-utils executables
# are prefixed with $(CROSS_COMPILE).
# CROSS_COMPILE can be set on the command line
# make CROSS_COMPILE=ia64-linux-
# Alternatively CROSS_COMPILE can be set in the environment.
CROSS_COMPILE ?= 

# Make variables (CC, etc...)
AS		= $(CROSS_COMPILE)gcc
LD		= $(CROSS_COMPILE)gcc
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP	= $(CROSS_COMPILE)objdump
SIZE	= $(CROSS_COMPILE)size

# File System Utilities
MKDIR	= mkdir -p
RM		= rm -f
MV		= mv -f

CPUFLAGS :=

COMMONFLAGS := \
	-g3 -O2 \
	-fmessage-length=0 \
	-ffunction-sections \
	-fdata-sections \
	-Wfloat-equal \
	-Wuninitialized \
	-Wextra \
	-Wall

CCFLAGS := \
	$(CPUFLAGS) \
	$(COMMONFLAGS) \
	-std=gnu11

CPPFLAGS := \
	$(CPUFLAGS) \
	$(COMMONFLAGS) \
	-std=c++1y

LDFLAGS := \
	$(CPUFLAGS) \
	$(LDFILES) \
	-Wl,-Map="$(ODIR)/$(TARGET).map" \
	-Wl,--gc-sections \
	-Wl,--print-memory-usage

#******************************************************************************
# Header File
INCLUDES :=

#******************************************************************************
# C File
CSRCS := main.c

#******************************************************************************
# CPP File
CPPSRCS :=

#******************************************************************************
# ASM File (*.S)
ASRCS :=

#******************************************************************************
# ASM File (*.s)
ASRCs :=

#******************************************************************************
# Binary resource (*)
BSRC :=

OBJS :=	$(CSRCS:%.c=$(OBJODIR)/%.o) \
		$(CPPSRCS:%.cpp=$(OBJODIR)/%.o) \
		$(ASRCs:%.s=$(OBJODIR)/%.o) \
		$(ASRCS:%.S=$(OBJODIR)/%.o) \
		$(BSRC:%=$(OBJODIR)/%)

DEPS :=	$(CSRCS:%.c=$(OBJODIR)/%.d) \
		$(CPPSRCS:%.cpp=$(OBJODIR)/%.d) \
		$(ASRCs:%.s=$(OBJODIR)/%.d) \
		$(ASRCS:%.S=$(OBJODIR)/%.d)

CFLAGS = $(CCFLAGS) $(INCLUDES)
CPFLAGS = $(CPPFLAGS) $(INCLUDES)

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),disasm)
ifdef DEPS
sinclude $(DEPS)
endif
endif
endif

$(OBJODIR)/%.o: %.c
	@echo [CC] $<
	$(Q)$(MKDIR) $(dir $@)
	$(Q)$(CC) -MT $@ -MMD -MP -MF $(OBJODIR)/$*.Td $(CFLAGS) -c -o $@ $<
	$(Q)$(MV) $(OBJODIR)/$*.Td $(OBJODIR)/$*.d && touch $@

$(OBJODIR)/%.o: %.cpp
	@echo [CPP] $<
	$(Q)$(MKDIR) $(dir $@)
	$(Q)$(CC) -MT $@ -MMD -MP -MF $(OBJODIR)/$*.Td $(CPFLAGS) -c -o $@ $<
	$(Q)$(MV) $(OBJODIR)/$*.Td $(OBJODIR)/$*.d && touch $@

$(OBJODIR)/%.o: %.s
	@echo [AS] $<
	$(Q)$(MKDIR) $(dir $@)
	$(Q)$(AS) $(CFLAGS) -M -o $(OBJODIR)/$*.d $<
	$(Q)$(AS) $(CFLAGS) -MMD -MP -MF $(OBJODIR)/$*.d -MT$@ -c -o $@ $<

$(OBJODIR)/%.o: %.S
	@echo [AS] $<
	$(Q)$(MKDIR) $(dir $@)
	$(Q)$(AS) $(CFLAGS) -M -o $(OBJODIR)/$*.d $<
	$(Q)$(AS) $(CFLAGS) -MMD -MP -MF $(OBJODIR)/$*.d -MT$@ -c -o $@ $<

$(OBJODIR)/%: %
	@echo [OBJCOPY] $<
	$(Q)$(MKDIR) $(dir $@)
	$(Q)$(OBJCOPY) $(BIN2OBJ) $< $@

#******************************************************************************
# Targets
#
PHONY += all
all: postbuild

PHONY += prebuild
prebuild:
	@echo ' '

mainbuild: prebuild elf

postbuild: mainbuild
	@echo 'Print Size:'
	$(Q)$(SIZE) --format=berkeley "$(ODIR)/$(TARGET).elf"
	@echo ' '

elf: $(OBJS)
	@echo [LD] $@
	$(Q)$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(ODIR)/$(TARGET).elf
	@echo ' '

PHONY += clean
clean:
	$(Q)$(RM) -r $(ODIR)
	@echo ' '

disasm: elf
	$(Q)$(OBJDUMP) -h -S -z $(ODIR)/$(TARGET).elf > $(ODIR)/$(TARGET).S
	@echo ' '

PHONY += listc
listc:
	@echo $(CSRCS) $(CPPSRCS) $(CASRCS)

# Declare the contents of the .PHONY variable as phony.  We keep that
# information in a variable so we can use it in if_changed and friends.
.PHONY: $(PHONY)
