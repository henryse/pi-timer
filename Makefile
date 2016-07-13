TARGETS:=Release Debug

BUILD_DIRS:=$(TARGETS:%=build-%)
CLEAN_DIRS:=$(TARGETS:%=clean-%)

help: settings
	@printf "\e[1;34m[INFO] [information]\e[00m\n\n"
	@echo This make process supports the following targets:
	@echo       clean   - clean up and targets in project
	@echo       build   - build both the project and Docker image
	@echo
	@echo The script supports the following parameters:

CMAKE=cmake

clean: $(CLEAN_DIRS)

$(CLEAN_DIRS):
	mkdir -p build/$(@:clean-%=%)
	cd build/$(@:clean-%=%); $(CMAKE) -DCMAKE_BUILD_TYPE=$(@:clean-%=%) ../..; $(MAKE) clean

build: $(BUILD_DIRS)

$(BUILD_DIRS):
	mkdir -p build/$(@:build-%=%)
	cd build/$(@:build-%=%); $(CMAKE) -DCMAKE_BUILD_TYPE=$(@:build-%=%) ../..; $(MAKE)