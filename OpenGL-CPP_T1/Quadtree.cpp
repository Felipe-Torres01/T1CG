#include "Quadtree.h"

// **********************************************************************
// bool PontoNoQuadtree(Ponto p, AABBQuadtree)
// **********************************************************************
bool PontoNoQuadtree(Ponto p, NoQuadtree& no) {
    Ponto minimoAABBQuadtree = Ponto (no.aabb.centro.x - no.aabb.meiaLarg.x, no.aabb.centro.y - no.aabb.meiaLarg.y);
    Ponto maximoAABBQuadtree = Ponto (no.aabb.centro.x + no.aabb.meiaLarg.x, no.aabb.centro.y + no.aabb.meiaLarg.y);

    if (p.x < minimoAABBQuadtree.x) {
        return false;
    }

    if (p.y < minimoAABBQuadtree.y) {
        return false;
    }

    if (p.x > maximoAABBQuadtree.x) {
        return false;
    }

    if (p.y > maximoAABBQuadtree.y) {
        return false;
    }

    return true;
}
// **********************************************************************
// bool testeQuadtree(NoQuadtree& no, Triangulo& triangulo)
//      Verifica se o envelope do triangulo sobrepoe uma quadtree
// **********************************************************************
bool testeQuadtree(NoQuadtree& no, Triangulo& triangulo){
    Ponto minimoNo = Ponto (no.aabb.centro.x - no.aabb.meiaLarg.x, no.aabb.centro.y - no.aabb.meiaLarg.y);
    Ponto maximoNo = Ponto (no.aabb.centro.x + no.aabb.meiaLarg.x, no.aabb.centro.y + no.aabb.meiaLarg.y);

    Ponto minimoTriangulo = triangulo.getEnvelope(0);
    Ponto maximoTriangulo = triangulo.getEnvelope(1);

    // Se triangulo esta a esquerda do no
    if (maximoTriangulo.x < minimoNo.x){
        return false;
    }

    // Se triangulo esta a direita do no
    if (minimoTriangulo.x > maximoNo.x){
        return false;
    }

    // Se triangulo esta abaixo
    if (maximoTriangulo.y < minimoNo.y){
        return false;
    }

    // Se triangulo esta acima
    if (minimoTriangulo.y > maximoNo.y){
        return false;
    }

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
// **********************************************************************
void divideQuadtree(NoQuadtree& no, int nivelMax){

    if (no.nivel == nivelMax){
        return;
    }

    NoQuadtree no1, no2, no3, no4;

    // Inferior esquerdo
    no1.aabb.meiaLarg.x = no.aabb.meiaLarg.x / 2;
    no1.aabb.meiaLarg.y = no.aabb.meiaLarg.y / 2;
    no1.aabb.centro = Ponto (no.aabb.centro.x - no1.aabb.meiaLarg.x, no.aabb.centro.y - no1.aabb.meiaLarg.y); 
    no1.nivel = no.nivel + 1;

    // Superior esquerdo
    no2.aabb.meiaLarg.x = no.aabb.meiaLarg.x / 2;
    no2.aabb.meiaLarg.y = no.aabb.meiaLarg.y / 2;
    no2.aabb.centro = Ponto (no.aabb.centro.x - no2.aabb.meiaLarg.x, no.aabb.centro.y + no2.aabb.meiaLarg.y);
    no2.nivel = no.nivel + 1;

    // Superior direito
    no3.aabb.meiaLarg.x = no.aabb.meiaLarg.x / 2;
    no3.aabb.meiaLarg.y = no.aabb.meiaLarg.y / 2;
    no3.aabb.centro = Ponto (no.aabb.centro.x + no3.aabb.meiaLarg.x, no.aabb.centro.y + no3.aabb.meiaLarg.y);
    no3.nivel = no.nivel + 1;

    // Inferior direito
    no4.aabb.meiaLarg.x = no.aabb.meiaLarg.x / 2;
    no4.aabb.meiaLarg.y = no.aabb.meiaLarg.y / 2;
    no4.aabb.centro = Ponto (no.aabb.centro.x + no4.aabb.meiaLarg.x, no.aabb.centro.y - no4.aabb.meiaLarg.y);
    no4.nivel = no.nivel + 1;

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
    if (testeQuadtree(noAtual, triangulos[indiceTriangulo])){
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
//      para cada um, o processo de associação às regiões da quadtree,
//      a partir do nó raiz.
// **********************************************************************

void percorreTriangulos(NoQuadtree& noRaiz, std::vector<Triangulo>& triangulos){
    for (int i = 0; i < triangulos.size(); i++){
        testeDeTriangulos(noRaiz, i, triangulos);
    }
}





