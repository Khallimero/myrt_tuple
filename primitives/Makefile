include ../Makefile.$(shell uname -m)
include ../myrt.mk

TARGET        = ../$(LIBDIR)/libprimitives.a

SOURCES       = Cone.cpp \
		CustomIsoSurface.cpp \
		Cylinder.cpp \
		Disc.cpp \
		Ellipse.cpp \
		FractalShape.cpp \
		Gyroid.cpp \
		Hit.cpp \
		Hyperboloid.cpp \
		IsoSurface.cpp \
		Plan.cpp \
		PlaneShape.cpp \
		QuadricShape.cpp \
		ShapeCollection.cpp \
		ShapeCollectionShape.cpp \
		Shape.cpp \
		Sphere.cpp \
		Tore.cpp \
		Triangle.cpp

all: $(TARGET)

clean:
	$(DEL) $(TARGET) ./$(OBJDIR) *~ *.orig

format:
	$(FORMAT) *.h *.cpp

$(TARGET): $(OBJ)
	@$(MKDIR) $(dir $@)
	$(AR) $@ $^
