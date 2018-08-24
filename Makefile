# Copyright Patrick Rohr, 2018

all: LocalFactory TargetDebug TargetRelease

LocalFactory: Workspace/LocalFactory
	cd $< && cmake ${PWD}

TargetDebug: Workspace/TargetDebug
	cd $< && cmake -DCMAKE_TOOLCHAIN_FILE=${PWD}/Configurations/SAMD21/toolchain_samd21g18a.cmake -DDEBUG=1 ${PWD}

TargetRelease: Workspace/TargetRelease
	cd $< && cmake -DCMAKE_TOOLCHAIN_FILE=${PWD}/Configurations/SAMD21/toolchain_samd21g18a.cmake ${PWD}

Workspace/%:
	mkdir -p $@

.PHONY: all LocalFactory TargetDebug TargetRelease