PROGRAM = Web\fractalExplorer
EXT     = .html

# Add your objs to generate in OBJS var
OBJS = Includes\raylib\utils.o Includes\raylib\rtextures.o Includes\imgui\imgui.o Includes\imgui\imgui_draw.o Includes\imgui\imgui_stdlib.o Includes\imgui\imgui_tables.o Includes\imgui\imgui_widgets.o Includes\rlImGui\rlImGui.o Sources\FractalRenderer.o Sources\main.o Sources\Ui.o

CXX = em++ -std=c++17

OPTIM_FLAGS ?= -O3
CXXFLAGS = $(OPTIM_FLAGS) -g -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable
CPPFLAGS = -IHeaders -IIncludes -IIncludes\imgui -IIncludes\raylib -MMD -DPLATFORM_WEB
LDFLAGS  = -LLib -sUSE_GLFW=3 -sASYNCIFY -sASSERTIONS -sALLOW_MEMORY_GROWTH
LDFLAGS += --pre-js Web\download.js --preload-file imgui.ini --preload-file Shaders\Mandelbrot.frag --preload-file Shaders\JuliaSet.frag
LDLIBS   = -lraylib

DEPS=$(OBJS:.o=.d)

.PHONY: all clean cleanWeb

all: $(PROGRAM)$(EXT)

-include $(DEPS)

%.o: %.c
	emcc -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@
	
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

$(PROGRAM)$(EXT): $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	clean.bat

cleanWeb:
	cleanWeb.bat
