#ifndef Quadtree_h
#define Quadtree_h

#include <vector>
#include "Ponto.h"
#include "Triangulo.h"

typedef struct {
    Ponto centro;
    Ponto meiaLarg;
    Ponto limitesMin;
    Ponto limitesMax;
} AABBQuadtree;

struct NoQuadtree {
    AABBQuadtree aabb; // Delimitador dessa região
    std::vector<NoQuadtree> filhos; // Cada nó pode possuir ate 4 filhos, se não tiver nenhum, ele é um no folha
    std::vector<int> indicesTriangulo; // Armazena os indices referentes aos triangulos candidados a receberem o teste
    int nivel = 0; // O nivel do nó na quadtree
};

bool PontoNoQuadtree(Ponto p, NoQuadtree& no);
bool testeQuadtree(NoQuadtree& no, Triangulo& triangulo);
void divideQuadtree(NoQuadtree& no, int nivelMax);
void testeDeTriangulos(NoQuadtree& noAtual, int indiceTriangulo, std::vector<Triangulo>& triangulos);
void percorreTriangulos(NoQuadtree& noRaiz, std::vector<Triangulo>& triangulos);
void contaNos(int& countNo, NoQuadtree& no);
NoQuadtree* localizaRegiaoDoPonto(Ponto p, NoQuadtree& noAtual);
void desenhaNo(NoQuadtree& no);
void desenhaQuadtree(NoQuadtree& no);
void colorirRegiao(NoQuadtree& no, int cor);

#endif