include ../Makefile.$(shell uname -m)
include ../myrt.mk

TARGET        = ../$(LIBDIR)/librenderer.a

SOURCES       = Camera.cpp \
		InfiniteLight.cpp \
		Light.cpp \
		PhotonBox.cpp \
		PhotonHitCollection.cpp \
		PhotonHitCollectionElement.cpp \
		PhotonHit.cpp \
		PhotonMap.cpp \
		PhotonMapElement.cpp \
		PointLight.cpp \
		Renderer.cpp \
		RendererQuality.cpp \
		Scene.cpp \
		SphericLight.cpp

all: $(TARGET)

clean:
	$(DEL) $(TARGET) ./$(OBJDIR) *~ *.orig

format:
	$(FORMAT) *.h *.cpp

$(TARGET): $(OBJ)
	@$(MKDIR) $(dir $@)
	$(AR) $@ $^
