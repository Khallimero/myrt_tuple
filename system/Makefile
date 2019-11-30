include ../Makefile.$(shell uname -m)
include ../myrt.mk

TARGET        = ../$(LIBDIR)/libsystem.a

SOURCES       =	AutoLock.cpp \
		Lockable.cpp \
		Thread.cpp

all: $(TARGET)

clean:
	$(DEL) $(TARGET) ./$(OBJDIR) *~ *.orig

format:
	$(FORMAT) *.h *.cpp

$(TARGET): $(OBJ)
	@$(MKDIR) $(dir $@)
	$(AR) $@ $^
