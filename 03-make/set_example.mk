# this declaration gives you ability to assign some default value to 
# variable if it is not set as environment variable
OVERRIDEN ?= Declare shell variable OVERRIDEN and export it to see the difference 

# these variables values still can be overriden by attaching something like
# SOME_VALUE=other value at the end of the make invocation

SOME_VALUE := initial value

IMMEDIATE_SET := $(SOME_VALUE)

LAZY_SET = $(SOME_VALUE)

SOME_VALUE = modified value

all:
	@echo Lazy set: $(LAZY_SET)
	@echo Immediate set: $(IMMEDIATE_SET)
	@echo Set if absent: $(OVERRIDEN)

