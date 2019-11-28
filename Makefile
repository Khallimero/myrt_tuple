include Makefile.$(shell uname -m)
include myrt.mk

SOURCE        = myrt.cpp
TARGET        = ./bin/myrt

all: $(TARGET)

clean: libclean
	$(DEL) $(TARGET) ./bin *.ppm *~ *.orig

libclean:
	$(foreach lib,$(LIBLIST) scenes,pushd $(lib);$(MAKE) clean;popd;)
	$(DEL) ./lib

format: libformat
	$(FORMAT) $(SOURCE)

libformat: format
	$(foreach lib,$(LIBLIST) scenes,pushd $(lib);$(MAKE) format;popd;)

tar: clean
	wd=`basename $$PWD`;ver=`cat version.txt`;cd ..;$(TAR) -cf $${wd}_$${ver}.tar $${wd};$(TAR) --delete -f $${wd}_$${ver}.tar $${wd}/.git;$(COMPRESS) $${wd}_$${ver}.tar

$(TARGET): libs
	@$(MKDIR) $(dir $@)
	$(eval $(REVERSELIB))
	$(CXX) $(CXXFLAGS) $(INCPATH) $(addprefix -I./,$(LIBLIST)) $(SOURCE) -o $(TARGET) -L$(LIBDIR) $(LINKLIBS) $(addprefix -l,$(LIBORDER))

libs: $(LIB)

lib/%.a:
	cd $(subst lib,,$(notdir $(@:.a=)));$(MAKE) $(MAKEFLAGS)
