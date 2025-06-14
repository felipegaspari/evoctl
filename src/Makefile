TARGET = evoctl$(EXEEXT)
SOURCES = main.cxx
# SOURCES = main_debug.cxx
SOURCES += settings_manager.cpp
# SOURCES += transfer_matrix_table.cxx
SOURCES += ../vendor/imgui/imgui.cpp
SOURCES += ../vendor/imgui/imgui_demo.cpp
SOURCES += ../vendor/imgui/imgui_draw.cpp
SOURCES += ../vendor/imgui/imgui_tables.cpp
SOURCES += ../vendor/imgui/imgui_widgets.cpp
SOURCES += ../vendor/imgui/backends/imgui_impl_glfw.cpp
SOURCES += ../vendor/imgui/backends/imgui_impl_opengl3.cpp

OBJECTS = $(SOURCES:.cpp=.o)
OBJECTS := $(OBJECTS:.cxx=.o)
OBJECTS := $(OBJECTS:.c=.o)

ifeq '$(OS)' "Windows_NT"
EXEEXT = .exe
endif

PKGCFGS      = libusb-1.0 glfw3
CXX          = g++
CC           = gcc
CXXFLAGS     = -Wall -I. -DIMGUI_ENABLE_DOCKING -DGLEW_STATIC
CXXFLAGS     += -I../vendor/imgui -I../vendor/imgui/backends -I../vendor/glew/include -I../vendor/json/include
CXXFLAGS     += $(shell pkg-config --cflags $(PKGCFGS))
CFLAGS = -Wall -I. -DIMGUI_ENABLE_DOCKING -DGLEW_STATIC
CFLAGS += -I../vendor/imgui -I../vendor/imgui/backends -I../vendor/glew/include -I../vendor/json/include
CFLAGS += $(shell pkg-config --cflags $(PKGCFGS))
LINKFLAGS    = $(shell pkg-config --libs $(PKGCFGS)) -lGL -L../vendor/glew/lib -lGLEW -Wl,-rpath,../vendor/glew/lib

# GLEW static library
GLEW_LIB = ../vendor/glew/lib/libglew.a

.SUFFIXES: .c .cxx .cpp .h .o $(EXEEXT)

RM    = rm -f
SHELL = /bin/sh
.SILENT:

default all: $(TARGET)

clean:
	$(RM) $(TARGET)
	$(RM) *.o
	$(RM) core

# HOW TO LINK
$(TARGET): $(OBJECTS) $(GLEW_LIB)
	@echo "*** Linking $@..."
	$(CXX) $(OBJECTS) $(LINKFLAGS) -o ../$@

# HOW TO COMPILE
.cxx.o:
	@echo "*** Compile $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

.cpp.o:
	@echo "*** Compile $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

.c.o:
	@echo "*** Compile $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Build GLEW static library if needed
$(GLEW_LIB):
	$(MAKE) -C ../vendor/glew/auto
	$(MAKE) -C ../vendor/glew SYSTEM=linux

