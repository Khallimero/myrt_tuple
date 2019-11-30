include ../Makefile.$(shell uname -m)
include ../myrt.mk

TARGET        = ../$(LIBDIR)/libtextures.a

SOURCES       = BasicTexture.cpp \
		BumpMap.cpp \
		CarveMap.cpp \
		ChequeredPattern.cpp \
		CloudPattern.cpp \
		CustomPerlinPattern.cpp \
		CustomTexture.cpp \
		FirePattern.cpp \
		MarblePattern.cpp \
		MercuryPattern.cpp \
		PerlinBumpMap.cpp \
		PerlinNoise.cpp \
		PerlinPattern.cpp \
		PictureTexture.cpp \
		RandomBumpMap.cpp \
		StripedPattern.cpp \
		TextureBumpMap.cpp \
		TextureCarveMap.cpp \
		Texture.cpp \
		WoodPattern.cpp \
		ZebraPattern.cpp

all: $(TARGET)

clean:
	$(DEL) $(TARGET) ./$(OBJDIR) *~ *.orig

format:
	$(FORMAT) *.h *.cpp

$(TARGET): $(OBJ)
	@$(MKDIR) $(dir $@)
	$(AR) $@ $^
