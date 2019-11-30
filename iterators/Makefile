include ../Makefile.$(shell uname -m)
include ../myrt.mk

TARGET        = ../$(LIBDIR)/libiterators.a

SOURCES       =	NestedIterator.cpp \
		PlanarSphericIterator.cpp \
		RoundedIterator.cpp \
		SphericIterator.cpp \
		VolumicSphericIterator.cpp

all: $(TARGET)

clean:
	$(DEL) $(TARGET) ./$(OBJDIR) *~ *.orig

format:
	$(FORMAT) *.h *.cpp

$(TARGET): $(OBJ)
	@$(MKDIR) $(dir $@)
	$(AR) $@ $^
