include ../Makefile.$(shell uname -m)
include ../myrt.mk

TARGET        = ../$(LIBDIR)/libcore.a

SOURCES       =	Arithmetic.cpp \
		AutoLock.cpp \
		Color.cpp \
		Complex.cpp \
		Geometric.cpp \
		Lockable.cpp \
		Map.cpp \
		Mark.cpp \
		NestedIterator.cpp \
		Pair.cpp \
		Picture.cpp \
		PlanarSphericIterator.cpp \
		Point.cpp \
		Poly.cpp \
		PolyRoot.cpp \
		Ray.cpp \
		Rotation.cpp \
		RoundedIterator.cpp \
		SphericIterator.cpp \
		Thread.cpp \
		Treble.cpp \
		Tuple.cpp \
		Vector.cpp \
		VolumicSphericIterator.cpp

all: $(TARGET)

clean:
	$(DEL) $(TARGET) ./$(OBJDIR) *~ *.orig

format:
	$(FORMAT) *.h *.cpp

$(TARGET): $(OBJ)
	@$(MKDIR) $(dir $@)
	$(AR) $@ $^
