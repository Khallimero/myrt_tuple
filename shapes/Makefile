include ../Makefile.$(shell uname -m|tr -d ' ')
include ../myrt.mk

TARGET        = ../$(LIBDIR)/libshapes.a

SOURCES       = DecicStar.cpp \
		DecoCube.cpp \
		DecoSphere.cpp \
		HyperCube.cpp \
		HyperTriangle.cpp \
		Juliabulb.cpp \
		Kluchikov.cpp \
		Mandelbulb.cpp \
		OrthoCircles.cpp \
		Scherk.cpp

all: $(TARGET)

clean:
	$(DEL) $(TARGET) ./$(OBJDIR) *~ *.orig

format:
	$(FORMAT) *.h *.cpp

$(TARGET): $(OBJ)
	@$(MKDIR) $(dir $@)
	$(AR) $@ $^
