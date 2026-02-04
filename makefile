# the above include may override NUSAROOT and NUSAINC to allow external
# directories to be included in the build
-include nusa_inc.mk

NUSAMAKEROOT ?= .
NUSAROOT ?= .
NUSAINC ?=
BUILDROOT ?= .
DEFAULT_PROJECT ?=
TOOLCHAIN_PREFIX ?=

# check if NUSAROOT is already a part of NUSAINC list and add it only if it is not
ifeq ($(filter $(NUSAROOT),$(NUSAINC)), )
NUSAINC := $(NUSAROOT) $(NUSAINC)
endif

# add the external path to NUSAINC
ifneq ($(NUSAROOT),.)
NUSAINC += $(NUSAROOT)/external
else
NUSAINC += external
endif

export NUSAMAKEROOT
export NUSAROOT
export NUSAINC
export BUILDROOT
export DEFAULT_PROJECT
export TOOLCHAIN_PREFIX
export RUN_UNITTESTS_AT_BOOT

# veneer makefile that calls into the engine with nusa as the build root
# if we're the top level invocation, call ourselves with additional args
_top:
	@$(MAKE) -C $(NUSAMAKEROOT) -rR -f $(NUSAROOT)/engine.mk $(addprefix -I,$(NUSAINC)) $(MAKECMDGOALS)

# If any arguments were provided, create a recipe for them that depends
# on the _top rule (thus calling it), but otherwise do nothing.
# "@:" (vs empty rule ";") prevents extra "'foo' is up to date." messages from
# being emitted.
$(MAKECMDGOALS): _top
	@:

.PHONY: _top
