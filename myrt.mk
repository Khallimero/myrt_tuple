SHELL:=/bin/bash

OBJDIR        = .obj
LIBDIR        = lib
LIBLIST       = system core iterators textures primitives CSG builders shapes renderer

INCPATH       = $(addprefix -I../,$(LIBLIST))
LIB           = $(addsuffix .a,$(addprefix $(LIBDIR)/lib,$(LIBLIST)))
REVERSELIB    = $(foreach l,$(LIBLIST),$(eval LIBORDER=$(l) $(LIBORDER)))

OBJ           = $(join $(addsuffix ./$(OBJDIR)/, $(dir $(SOURCES))), $(notdir $(SOURCES:.cpp=.o)))

./$(OBJDIR)/%.o : %.cpp
	@$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCDIR) $(INCPATH) -c $< -o $@

NB_THREAD     = `$(NPROC)`
