include Makefile.$(shell uname -m|tr -d ' ')
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
	wd=`basename $$PWD`;ver=`cat version.txt`;cd ..;$(TAR) -cf $${wd}_$${ver}.tar $${wd};$(TAR) --delete -f $${wd}_$${ver}.tar $${wd}/.git $${wd}/.gitignore $${wd}/.kdev4 $${wd}/myrt_tuple.kdev4;$(COMPRESS) $${wd}_$${ver}.tar

tar_kdev: clean
	wd=`basename $$PWD`;ver=`cat version.txt`;cd ..;$(TAR) -cf $${wd}_$${ver}_kdev.tar $${wd};$(COMPRESS) $${wd}_$${ver}_kdev.tar
	
strip: all
	$(STRIP) $(TARGET)
	
$(TARGET): libs
	@$(MKDIR) $(dir $@)
	$(eval $(REVERSELIB))
	$(CXX) $(CXXFLAGS) $(INCPATH) $(addprefix -I./,$(LIBLIST)) $(SOURCE) -o $(TARGET) -L$(LIBDIR) $(LINKLIBS) $(addprefix -l,$(LIBORDER))

libs: $(LIB)

lib/%.a:
	cd $(subst lib,,$(notdir $(@:.a=)));$(MAKE) $(MAKEFLAGS)
