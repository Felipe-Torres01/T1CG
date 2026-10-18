#ifndef Quadtree_h
#define Quadtree_h

#include <vector>
#include "Ponto.h"
#include "Triangulo.h"

typedef struct {
    Ponto centro;
    Ponto meiaLarg;
} AABBQuadtree;

struct NoQuadtree {
    AABBQuadtree aabb;
    std::vector<NoQuadtree> filhos;
    std::vector<int> indicesTriangulo;
    int nivel = 0;
};

bool PontoNoQuadtree(Ponto p, NoQuadtree& no);
bool testeQuadtree(NoQuadtree& no, Triangulo& triangulo);
void divideQuadtree(NoQuadtree& no, int nivelMax);
void testeDeTriangulos(NoQuadtree& noAtual, int indiceTriangulo, std::vector<Triangulo>& triangulos);
void percorreTriangulos(NoQuadtree& noRaiz, std::vector<Triangulo>& triangulos);
void contaNos(int& countNo, NoQuadtree& no);
NoQuadtree* localizaRegiaoDoPonto(Ponto p, NoQuadtree& noAtual);

#endif