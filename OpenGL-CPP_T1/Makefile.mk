# Makefile para Windows com MinGW-w64

PROG = ProgOpenGL.exe

FONTES = Ponto.cpp \
         Poligono.cpp \
         Temporizador.cpp \
         ListaDeCoresRGB.cpp \
         Triangulo.cpp \
         Quadtree.cpp \
         Prog-Triangulos.cpp

OBJETOS = $(FONTES:.cpp=.o)

CXX = g++

CXXFLAGS = -std=c++14 \
           -g \
           -Iinclude/GL \
           -Wno-write-strings \
           -Wno-narrowing \
           -DWIN32

LDFLAGS = -Llib/x64

LDLIBS = -lfreeglut \
         -lopengl32 \
         -lglu32 \
         -lm

.PHONY: all clean run

all: $(PROG)

$(PROG): $(OBJETOS)
	$(CXX) $(OBJETOS) -o $@ $(LDFLAGS) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(PROG)
	./$(PROG)

clean:
	cmd /C "del /Q $(OBJETOS) $(PROG) 2>NUL"
