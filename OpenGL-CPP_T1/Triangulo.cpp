//
//  Triangulo.cpp
//  OpenGL
// 
//  Created by Marcio Sarroglia Pinho on 26/09/26.
//  Copyright © 2026 Marcio Sarroglia Pinho. All rights reserved.
//

#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>

using namespace std;


#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif

#include "ListaDeCoresRGB.h"

#include "Triangulo.h"


// **********************************************************************
// void Triangulo::CalculaEnvelope()
//      Calcula o envelope (AABB) do triangulo.
//      Envelope[0] = ponto minimo e Envelope[1] = ponto maximo.
// **********************************************************************
void Triangulo::CalculaEnvelope()
{
    Envelope[0] = Vertices[0];
    Envelope[1] = Vertices[0];

    for (int i = 1; i < 3; i++)
    {
        Envelope[0] = ObtemMinimo(Envelope[0], Vertices[i]);
        Envelope[1] = ObtemMaximo(Envelope[1], Vertices[i]);
    }
}


// **********************************************************************
// Triangulo::Triangulo(int cor)
//      Cria um triangulo com vertices na origem e define a sua cor.
// **********************************************************************
Triangulo::Triangulo(int cor)
{
    Vertices[0] = Ponto(0, 0);
    Vertices[1] = Ponto(0, 0);
    Vertices[2] = Ponto(0, 0);

    this->cor = cor;

    CalculaEnvelope();
}


// **********************************************************************
// Triangulo::Triangulo(Ponto V[], int cor)
//      Cria um triangulo a partir de um vetor de vertices e define a sua cor.
// **********************************************************************
Triangulo::Triangulo(Ponto V[], int cor)
{
    for (int i = 0; i < 3; i++)
        Vertices[i] = V[i];

    this->cor = cor;

    CalculaEnvelope();
}


// **********************************************************************
// Triangulo::Triangulo(Ponto P1, Ponto P2, Ponto P3, int cor)
//      Cria um triangulo a partir de tres pontos e define a sua cor.
// **********************************************************************
Triangulo::Triangulo(Ponto P1, Ponto P2, Ponto P3, int cor)
{
    Vertices[0] = P1;
    Vertices[1] = P2;
    Vertices[2] = P3;

    this->cor = cor;

    CalculaEnvelope();
}


// **********************************************************************
// void Triangulo::setCor(int C)
//      Define a cor do triangulo.
// **********************************************************************
void Triangulo::setCor(int C)
{
    cor = C;
}


// **********************************************************************
// int Triangulo::getCor()
//      Retorna a cor do triangulo.
// **********************************************************************
int Triangulo::getCor()
{
    return cor;
}


// **********************************************************************
// void Triangulo::Pinta()
//      Desenha o triangulo preenchido usando a sua cor.
// **********************************************************************
void Triangulo::Pinta()
{
    defineCor(cor);

    glBegin(GL_TRIANGLES);
        glVertex2f(Vertices[0].x, Vertices[0].y);
        glVertex2f(Vertices[1].x, Vertices[1].y);
        glVertex2f(Vertices[2].x, Vertices[2].y);
    glEnd();
}


// **********************************************************************
// void Triangulo::Pinta(int cor)
//      Desenha o triangulo preenchido usando a cor informada.
// **********************************************************************
void Triangulo::Pinta(int cor)
{
    defineCor(cor);

    glBegin(GL_TRIANGLES);
        glVertex2f(Vertices[0].x, Vertices[0].y);
        glVertex2f(Vertices[1].x, Vertices[1].y);
        glVertex2f(Vertices[2].x, Vertices[2].y);
    glEnd();
}


// **********************************************************************
// void Triangulo::Desenha()
//      Desenha as arestas do triangulo usando a sua cor.
// **********************************************************************
void Triangulo::Desenha()
{
    defineCor(cor);

    glBegin(GL_LINE_LOOP);
        glVertex2f(Vertices[0].x, Vertices[0].y);
        glVertex2f(Vertices[1].x, Vertices[1].y);
        glVertex2f(Vertices[2].x, Vertices[2].y);
    glEnd();
}

// **********************************************************************
// void Triangulo::Desenha(int cor)
//      Desenha as arestas do triangulo usando a cor informada.
// **********************************************************************
void Triangulo::Desenha(int cor)
{
    defineCor(cor);

    glBegin(GL_LINE_LOOP);
        glVertex2f(Vertices[0].x, Vertices[0].y);
        glVertex2f(Vertices[1].x, Vertices[1].y);
        glVertex2f(Vertices[2].x, Vertices[2].y);
    glEnd();
}


// **********************************************************************
// void Triangulo::DesenhaEnvelope()
//      Desenha o envelope (AABB) do triangulo.
// **********************************************************************
void Triangulo::DesenhaEnvelope()
{
    glBegin(GL_LINE_LOOP);
        glVertex2f(Envelope[0].x, Envelope[0].y);
        glVertex2f(Envelope[1].x, Envelope[0].y);
        glVertex2f(Envelope[1].x, Envelope[1].y);
        glVertex2f(Envelope[0].x, Envelope[1].y);
    glEnd();
}


// **********************************************************************
// void Triangulo::getVertices(Ponto V[])
//      Copia os tres vertices do triangulo para o vetor V.
// **********************************************************************
void Triangulo::getVertices(Ponto V[])
{
    for (int i = 0; i < 3; i++)
        V[i] = Vertices[i];
}


// **********************************************************************
// Ponto Triangulo::getVertices(int i)
//      Retorna o vertice de indice i.
// **********************************************************************
Ponto Triangulo::getVertices(int i)
{
    if (i >= 0 && i < 3)
        return Vertices[i];

    return Ponto();
}

// **********************************************************************
// Ponto Triangulo::getEnvelope(int i)
//      Retorna o envelope de indice i.
// **********************************************************************
Ponto Triangulo::getEnvelope(int i)
{
    if (i >= 0 && i < 2)
        return Envelope[i];

    return Ponto();
}


// **********************************************************************
// void Triangulo::imprime()
//      Imprime as coordenadas dos vertices do triangulo.
// **********************************************************************
void Triangulo::imprime()
{
    cout << "[";

    for (int i = 0; i < 3; i++)
    {
        cout << "("
             << Vertices[i].x << ", "
             << Vertices[i].y << ")";

        if (i < 2)
            cout << " - ";
    }

    cout << "]" << endl;
}


// **********************************************************************
// bool Triangulo::PontoNoTriangulo(Ponto P)
//      Verifica se o ponto P esta dentro do triangulo.
//      Pontos sobre as arestas ou vertices sao considerados internos.
// **********************************************************************
bool Triangulo::PontoNoTriangulo(Ponto P)
{
    double d1, d2, d3;

    // Calcula a orientacao de P em relacao a cada aresta
    d1 = (P.x - Vertices[1].x) * (Vertices[0].y - Vertices[1].y) -
         (Vertices[0].x - Vertices[1].x) * (P.y - Vertices[1].y);

    d2 = (P.x - Vertices[2].x) * (Vertices[1].y - Vertices[2].y) -
         (Vertices[1].x - Vertices[2].x) * (P.y - Vertices[2].y);

    d3 = (P.x - Vertices[0].x) * (Vertices[2].y - Vertices[0].y) -
         (Vertices[2].x - Vertices[0].x) * (P.y - Vertices[0].y);

    // Verifica se existem orientacoes com sinais diferentes
    const double EPS = 1e-6;
    bool temNegativo = (d1 < -EPS) || (d2 < -EPS) || (d3 < -EPS);
    bool temPositivo = (d1 > EPS) || (d2 > EPS) || (d3 > EPS);

    return !(temNegativo && temPositivo);
}

// **********************************************************************
// bool Triangulo::PontoNoTriangulo(Ponto P)
//      Verifica se o ponto P esta dentro do triangulo.
//      Pontos sobre as arestas ou vertices sao considerados internos.
// **********************************************************************
bool Triangulo::PontoNoAABB(Ponto P){

     const double EPS = 1e-6;

    if (P.x < Envelope[0].x - EPS) return false;
    if (P.y < Envelope[0].y - EPS) return false;
    if (P.x > Envelope[1].x + EPS) return false;
    if (P.y > Envelope[1].y + EPS) return false;

    return true;
}


