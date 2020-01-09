include ../Makefile.$(shell uname -m|tr -d ' ')
include ../myrt.mk

TARGET        = ../$(LIBDIR)/libbuilders.a

SOURCES       =	BoxedCollectionShape.cpp \
		BoyApery.cpp \
		CoilingShapeBuilder.cpp \
		GrannyKnot.cpp \
		Helix.cpp \
		Knot.cpp \
		MobiusStrip.cpp \
		MobiusTore.cpp \
		PLYShape.cpp \
		ParametricIsoSurface.cpp \
		ParametricPrimitive.cpp \
		ParametricSurfacePrimitive.cpp \
		ParametricTore.cpp \
		Riemann.cpp \
		ShapeBuilder.cpp \
		SquareKnot.cpp \
		StraightShapeBuilder.cpp \
		TorusKnot.cpp \
		UmbilicTore.cpp

all: $(TARGET)

clean:
	$(DEL) $(TARGET) ./$(OBJDIR) *~ *.orig

format:
	$(FORMAT) *.h *.cpp

$(TARGET): $(OBJ)
	@$(MKDIR) $(dir $@)
	$(AR) $@ $^
