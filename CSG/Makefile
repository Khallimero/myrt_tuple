include ../Makefile.$(shell uname -m|tr -d ' ')
include ../myrt.mk

TARGET        = ../$(LIBDIR)/libCSG.a

SOURCES       = AddShape.cpp \
		CSGShape.cpp \
		InterShape.cpp \
		SubShape.cpp

all: $(TARGET)

clean:
	$(DEL) $(TARGET) ./$(OBJDIR) *~ *.orig

format:
	$(FORMAT) *.h *.cpp

$(TARGET): $(OBJ)
	@$(MKDIR) $(dir $@)
	$(AR) $@ $^
