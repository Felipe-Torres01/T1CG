#ifndef Consulta_h
#define Consulta_h

#include <vector>
#include "Ponto.h"
#include "Triangulo.h"
#include "Quadtree.h"

struct ResultadoConsulta {
    int totalTriangulos = 0;
    int candidatosQuadtree = 0;   // saida da subdivisao espacial
    int aprovadosEnvelope = 0;    // saida do envelope
    int testesDetalhados = 0;     
    double tempoMicros = 0;       
    NoQuadtree* regiao = NULL;    
    std::vector<int> aprovados;   
    std::vector<int> contem;      
};

ResultadoConsulta executaConsulta(Ponto p,
                                  std::vector<Triangulo>& triangulos,
                                  NoQuadtree& raiz,
                                  bool usaQuadtree,
                                  bool usaEnvelope);

#endif
