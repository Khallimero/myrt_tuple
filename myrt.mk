SHELL:=/bin/bash

OBJDIR        = .obj
LIBDIR        = lib
LIBLIST       = core texture shape renderer

INCPATH       = $(addprefix -I../,$(LIBLIST))
LIB           = $(addsuffix .a,$(addprefix $(LIBDIR)/lib,$(LIBLIST)))
REVERSELIB    = $(foreach l,$(LIBLIST),$(eval LIBORDER=$(l) $(LIBORDER)))

OBJ           = $(join $(addsuffix ./$(OBJDIR)/, $(dir $(SOURCES))), $(notdir $(SOURCES:.cpp=.o)))

./$(OBJDIR)/%.o : %.cpp
	@$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCPATH) -c $< -o $@
