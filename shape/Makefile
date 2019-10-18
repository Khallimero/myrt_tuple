include ../Makefile.$(shell uname -m)
include ../myrt.mk

TARGET        = ../$(LIBDIR)/libshape.a

SOURCES       = AddShape.cpp \
		BoxedCollectionShape.cpp \
		BoyApery.cpp \
		CoilingShapeBuilder.cpp \
		Cone.cpp \
		CSGShape.cpp \
		CustomIsoSurface.cpp \
		Cylinder.cpp \
		DecicStar.cpp \
		DecoCube.cpp \
		DecoSphere.cpp \
		Disc.cpp \
		Ellipse.cpp \
		FractalShape.cpp \
		GrannyKnot.cpp \
		Gyroid.cpp \
		Helix.cpp \
		Hit.cpp \
		Hyperboloid.cpp \
		HyperCube.cpp \
		HyperTriangle.cpp \
		InterShape.cpp \
		IsoSurface.cpp \
		Juliabulb.cpp \
		Kluchikov.cpp \
		Knot.cpp \
		Mandelbulb.cpp \
		MobiusStrip.cpp \
		MobiusTore.cpp \
		OrthoCircles.cpp \
		ParametricIsoSurface.cpp \
		ParametricPrimitive.cpp \
		ParametricSurfacePrimitive.cpp \
		ParametricTore.cpp \
		Plan.cpp \
		PlaneShape.cpp \
		PLYShape.cpp \
		QuadricShape.cpp \
		Riemann.cpp \
		ShapeBuilder.cpp \
		ShapeCollection.cpp \
		ShapeCollectionShape.cpp \
		Shape.cpp \
		Scherk.cpp \
		Sphere.cpp \
		SquareKnot.cpp \
		StraightShapeBuilder.cpp \
		SubShape.cpp \
		Tore.cpp \
		TorusKnot.cpp \
		Triangle.cpp \
		UmbilicTore.cpp

all: $(TARGET)

clean:
	$(DEL) $(TARGET) ./$(OBJDIR) *~ *.orig

format:
	$(FORMAT) *.h *.cpp

$(TARGET): $(OBJ)
	@$(MKDIR) $(dir $@)
	$(AR) $@ $^
