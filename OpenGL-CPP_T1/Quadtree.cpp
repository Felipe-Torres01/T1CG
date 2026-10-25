#include "Quadtree.h"

#include "ListaDeCoresRGB.h"

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

// **********************************************************************
// bool PontoNoQuadtree(Ponto p, AABBQuadtree)
//      Verifica se o ponto esta em uma regiao da quadtree
// **********************************************************************
bool PontoNoQuadtree(Ponto p, NoQuadtree& no) {
    
    Ponto limiteMinimoAABBQuadtree = no.aabb.limitesMin;
    Ponto limiteMaximoAABBQuadtree = no.aabb.limitesMax;


    // Se o ponto está abaixo dos limites mínimos da região da quadtree
    if (p.x < limiteMinimoAABBQuadtree.x) {
        return false;
    }
    if (p.y < limiteMinimoAABBQuadtree.y) {
        return false;
    }

    // Se o ponto está acima dos limites máximos da região da quadtree
    if (p.x > limiteMaximoAABBQuadtree.x) {
        return false;
    }
    if (p.y > limiteMaximoAABBQuadtree.y) {
        return false;
    }

    // Retorna true somente se o ponto está dentro dos límites mínimos e máximos da região,
    // indicando que está dentro do envelope
    return true;
}
// **********************************************************************
// bool testeQuadtree(NoQuadtree& no, Triangulo& triangulo)
//      Verifica se o envelope do triangulo sobrepoe uma quadtree
// **********************************************************************
bool testeEnvelopeTrianguloNoQuadtree(NoQuadtree& no, Triangulo& triangulo){

    // Limites minimos e maximos da região/no da quadtree
    Ponto limiteMinimoAABBNo = no.aabb.limitesMin;
    Ponto limiteMaximoAABBNo = no.aabb.limitesMax;

    // Limites minimos e maximos do envelope do triangulo
    Ponto limiteMinimoTriangulo = triangulo.getEnvelope(0);
    Ponto limiteMaximoTriangulo = triangulo.getEnvelope(1);

    // Se o envelope do triangulo esta a esquerda do no
    if (limiteMaximoTriangulo.x < limiteMinimoAABBNo.x){
        return false;
    }

    // Se o envelope do triangulo esta a direita do no
    if (limiteMinimoTriangulo.x > limiteMaximoAABBNo.x){
        return false;
    }

    // Se o envelope do triangulo esta abaixo
    if (limiteMaximoTriangulo.y < limiteMinimoAABBNo.y){
        return false;
    }

    // Se o envelope do triangulo esta acima
    if (limiteMinimoTriangulo.y > limiteMaximoAABBNo.y){
        return false;
    }

    // Retorna true somente se as coordenadas do envelope do triangulo estiver dentro
    // dos limites minimo e maximo da regiao/no da quadtree
    return true;
}

// **********************************************************************
// void contaNos(int& countNo, NoQuadtree& no)
//      Retorna o numero de nos da quadtree
// **********************************************************************
void contaNos(int& countNo, NoQuadtree& no){

    countNo += 1; 

    for (int i = 0; i < no.filhos.size(); i++){
        contaNos(countNo, no.filhos[i]);
    }
}

// **********************************************************************
// NoQuadtree* localizaRegiaoDoPonto(Ponto p, NoQuadtree& noAtual)
//      Localiza a regiao da quadtree onde esta o ponto.
//      Devolve o endereco da folha, ou NULL se o ponto estiver fora.
// **********************************************************************
NoQuadtree* localizaRegiaoDoPonto(Ponto p, NoQuadtree& noAtual){

    if (!PontoNoQuadtree(p, noAtual)){
        return NULL;   // ponto fora deste no
    }

    for (int i = 0; i < noAtual.filhos.size(); i++){
        if (PontoNoQuadtree(p, noAtual.filhos[i])){
            return localizaRegiaoDoPonto(p, noAtual.filhos[i]);
        }
    }

    return &noAtual;   // folha (ou nenhum filho contem o ponto)
}
// **********************************************************************
//  void divideQuadtree(NoQuadtree& no)
//      Realiza a divisão de uma quadtree criando seus nós filhos até atingir 
//      o nível máximo (NIVEL_MAX = 5).
//
//      Cada um dos filhos recebe como meias larguras (x e y) as metades da meia largura em x e y (divido por dois) do no pai
//      Cada um dos filhos recebe como centro a subtracao ou soma (dependendo de onde esta a regiao do filho, acima ou abaixo) 
//      de suas meias larguras com o centro (em x e y) do pai
// **********************************************************************
void divideQuadtree(NoQuadtree& no, int nivelMax){

    if (no.nivel == nivelMax){
        return;
    }

    // nos filhos que serao adicionados ao no pai
    NoQuadtree no1, no2, no3, no4;

    // Inferior esquerdo
    no1.aabb.meiaLarg.x = no.aabb.meiaLarg.x / 2;
    no1.aabb.meiaLarg.y = no.aabb.meiaLarg.y / 2;
    no1.aabb.centro = Ponto (no.aabb.centro.x - no1.aabb.meiaLarg.x, no.aabb.centro.y - no1.aabb.meiaLarg.y); 
    no1.nivel = no.nivel + 1;
    no1.aabb.limitesMin = Ponto (no1.aabb.centro.x - no1.aabb.meiaLarg.x, no1.aabb.centro.y - no1.aabb.meiaLarg.y);
    no1.aabb.limitesMax = Ponto (no1.aabb.centro.x + no1.aabb.meiaLarg.x, no1.aabb.centro.y + no1.aabb.meiaLarg.y);

    // Superior esquerdo
    no2.aabb.meiaLarg.x = no.aabb.meiaLarg.x / 2;
    no2.aabb.meiaLarg.y = no.aabb.meiaLarg.y / 2;
    no2.aabb.centro = Ponto (no.aabb.centro.x - no2.aabb.meiaLarg.x, no.aabb.centro.y + no2.aabb.meiaLarg.y);
    no2.nivel = no.nivel + 1;
    no2.aabb.limitesMin = Ponto (no2.aabb.centro.x - no2.aabb.meiaLarg.x, no2.aabb.centro.y - no2.aabb.meiaLarg.y);
    no2.aabb.limitesMax = Ponto (no2.aabb.centro.x + no2.aabb.meiaLarg.x, no2.aabb.centro.y + no2.aabb.meiaLarg.y);

    // Superior direito
    no3.aabb.meiaLarg.x = no.aabb.meiaLarg.x / 2;
    no3.aabb.meiaLarg.y = no.aabb.meiaLarg.y / 2;
    no3.aabb.centro = Ponto (no.aabb.centro.x + no3.aabb.meiaLarg.x, no.aabb.centro.y + no3.aabb.meiaLarg.y);
    no3.nivel = no.nivel + 1;
    no3.aabb.limitesMin = Ponto (no3.aabb.centro.x - no3.aabb.meiaLarg.x, no3.aabb.centro.y - no3.aabb.meiaLarg.y);
    no3.aabb.limitesMax = Ponto (no3.aabb.centro.x + no3.aabb.meiaLarg.x, no3.aabb.centro.y + no3.aabb.meiaLarg.y);

    // Inferior direito
    no4.aabb.meiaLarg.x = no.aabb.meiaLarg.x / 2;
    no4.aabb.meiaLarg.y = no.aabb.meiaLarg.y / 2;
    no4.aabb.centro = Ponto (no.aabb.centro.x + no4.aabb.meiaLarg.x, no.aabb.centro.y - no4.aabb.meiaLarg.y);
    no4.nivel = no.nivel + 1;
    no4.aabb.limitesMin = Ponto (no4.aabb.centro.x - no4.aabb.meiaLarg.x, no4.aabb.centro.y - no4.aabb.meiaLarg.y);
    no4.aabb.limitesMax = Ponto (no4.aabb.centro.x + no4.aabb.meiaLarg.x, no4.aabb.centro.y + no4.aabb.meiaLarg.y);

    // no pai com seus filhos
    no.filhos.push_back(no1);
    no.filhos.push_back(no2);
    no.filhos.push_back(no3);
    no.filhos.push_back(no4);

    // Continua a dividir pelos filhos
    for (int i = 0; i < no.filhos.size(); i++){
        divideQuadtree(no.filhos[i],  nivelMax);
    }
}
// **********************************************************************
// void testeDeTriangulos(NoQuadtree& noAtual, int indiceTriangulo)
//      Verifica se o envelope do triângulo indicado pelo índice se
//      sobrepõe à região do nó atual. Em caso positivo, armazena o
//      índice no nó e verifica recursivamente seus filhos.
// **********************************************************************
void testeDeTriangulos(NoQuadtree& noAtual, int indiceTriangulo, std::vector<Triangulo>& triangulos){

    // Verifica se o envelope do triangulo sobrepoe uma quadtree 
    if (testeEnvelopeTrianguloNoQuadtree(noAtual, triangulos[indiceTriangulo])){
        noAtual.indicesTriangulo.push_back(indiceTriangulo);

        // Faz o teste com os filhos do nó 
        for (int i = 0; i < noAtual.filhos.size(); i++){
            testeDeTriangulos(noAtual.filhos[i], indiceTriangulo, triangulos);
        }
    }
    else {
        return;
    }
}
// **********************************************************************
//  percorreTriangulos(NoQuadtree& noRaiz)
//      Percorre todos os triângulos armazenados no vetor e inicia,
//      para cada um, o teste para verificar se um triangulo esta dentro da regiao 
//      a partir do nó raiz.
// **********************************************************************

void percorreTriangulos(NoQuadtree& noRaiz, std::vector<Triangulo>& triangulos){
    for (int i = 0; i < triangulos.size(); i++){
        testeDeTriangulos(noRaiz, i, triangulos);
    }
}

// **********************************************************************
//  void desenhaNo(NoQuadtree& no)
//      Desenha as linhas do no a partir de um dado como parametro 
// **********************************************************************

void desenhaNo(NoQuadtree& no){
    glBegin(GL_LINE_LOOP);

        // Ponto do canto inferior esquerdo, limites minimo do no
        glVertex2f(no.aabb.limitesMin.x, no.aabb.limitesMin.y);

        // Ponto do canto superior esquerdo
        glVertex2f(no.aabb.centro.x - no.aabb.meiaLarg.x, no.aabb.centro.y + no.aabb.meiaLarg.y);

        // Ponto do canto superior direito, limites maximo do no
        glVertex2f(no.aabb.limitesMax.x, no.aabb.limitesMax.y);

        // Ponto do canto inferior direito
        glVertex2f(no.aabb.centro.x + no.aabb.meiaLarg.x, no.aabb.centro.y - no.aabb.meiaLarg.y);
    
    glEnd();
}

// **********************************************************************
//  void desenhaQuadtree(NoQuadtree& no)
//      Desenha a quadtree inteira 
// **********************************************************************

void desenhaQuadtree(NoQuadtree& no){

    // desenha o no
    desenhaNo(no);

    // Funcao recursiva que percorre todos os nos para desenhar
    for (int i = 0; i < no.filhos.size(); i++){
        desenhaQuadtree(no.filhos[i]);
    }
}

// **********************************************************************
//  void colorirRegiao(NoQuadtree& no)
//      Pinta a regiao com uma cor e no dado como parametro
// **********************************************************************

void colorirRegiao(NoQuadtree& no, int cor){
    defineCor(cor);

    glBegin(GL_QUADS);

        // Ponto do canto inferior esquerdo, limites minimo do no
        glVertex2f(no.aabb.limitesMin.x, no.aabb.limitesMin.y);

        // Ponto do canto superior esquerdo
        glVertex2f(no.aabb.centro.x - no.aabb.meiaLarg.x, no.aabb.centro.y + no.aabb.meiaLarg.y);

        // Ponto do canto superior direito, limites maximo do no
        glVertex2f(no.aabb.limitesMax.x, no.aabb.limitesMax.y);

        // Ponto do canto inferior direito
        glVertex2f(no.aabb.centro.x + no.aabb.meiaLarg.x, no.aabb.centro.y - no.aabb.meiaLarg.y);
    
    glEnd();
    
}




