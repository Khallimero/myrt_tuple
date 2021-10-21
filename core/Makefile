include ../Makefile.$(shell uname -m|tr -d ' ')
include ../myrt.mk

TARGET        = ../$(LIBDIR)/libcore.a

SOURCES       =	Arithmetic.cpp \
		Color.cpp \
		Complex.cpp \
		Geometric.cpp \
		Grid.cpp \
		Mark.cpp \
		Pair.cpp \
		Picture.cpp \
		Point.cpp \
		Poly.cpp \
		PolyRoot.cpp \
		Ray.cpp \
		Rotation.cpp \
		Treble.cpp \
		Tuple.cpp \
		Vector.cpp

all: $(TARGET)

clean:
	$(DEL) $(TARGET) ./$(OBJDIR) *~ *.orig

format:
	$(FORMAT) *.h *.cpp

$(TARGET): $(OBJ)
	@$(MKDIR) $(dir $@)
	$(AR) $@ $^
