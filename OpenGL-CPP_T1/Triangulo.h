//
//  Triangulo.h
//  OpenGL
//
//  Created by Marcio Sarroglia Pinho on 26/09/26.
//  Copyright © 2026 Marcio Sarroglia Pinho. All rights reserved.
//

#ifndef Triangulo_h
#define Triangulo_h

#include <iostream>

#include "Ponto.h"

class Triangulo
{
    Ponto Vertices[3];
    Ponto Envelope[2]; // AABB do triangulo
    int cor;

    // Calcula o envelope (AABB) do triangulo
    void CalculaEnvelope();

public:
    // Desenha as arestas do triangulo
    void Desenha();
    void Desenha(int cor);

    // Desenha o triangulo preenchido
    void Pinta();
    void Pinta(int cor);

    // Desenha o envelope (AABB) do triangulo
    void DesenhaEnvelope();

    // Metodos construtores
    Triangulo(int cor);

    // Criam a instancia e calculam o envelope
    Triangulo(Ponto V[], int cor);
    Triangulo(Ponto P1, Ponto P2, Ponto P3, int cor);

    // Define e retorna a cor do triangulo
    void setCor(int cor);
    int getCor();

    // Devolve todos os vertices
    void getVertices(Ponto V[]);

    // Retorna o vertice i
    Ponto getVertices(int i);

    // Retorna o envelope no indice i
    Ponto getEnvelope(int i);

    // Imprime os vertices
    void imprime();
    
    // Informa se o ponto esta no triangulo
    bool PontoNoTriangulo(Ponto P);
    
    // Informa se o ponto esta na bounding box do triangulo
    bool PontoNoAABB(Ponto P);
};

#endif /* Triangulo_hpp */
