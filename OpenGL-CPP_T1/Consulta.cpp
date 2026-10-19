#include <chrono>
#include "Consulta.h"

ResultadoConsulta executaConsulta(Ponto p,
                                  std::vector<Triangulo>& triangulos,
                                  NoQuadtree& raiz,
                                  bool usaQuadtree,
                                  bool usaEnvelope)
{
    ResultadoConsulta r;
    r.totalTriangulos = triangulos.size();

    auto inicio = std::chrono::steady_clock::now();

    // subdivisao espacial 
    const std::vector<int>* candidatos = NULL;   // todos os triangulos
    int n = triangulos.size();

    if (usaQuadtree){
        r.regiao = localizaRegiaoDoPonto(p, raiz);
        if (r.regiao != NULL){
            candidatos = &r.regiao->indicesTriangulo;
            n = candidatos->size();
        } else {
            n = 0;   // ponto fora da area
        }
    }
    r.candidatosQuadtree = n;

    for (int k = 0; k < n; k++){
        int i = (candidatos != NULL) ? (*candidatos)[k] : k;

        // envelope 
        if (usaEnvelope){
            if (!triangulos[i].PontoNoAABB(p)){
                continue;
            }
            r.aprovados.push_back(i);
        }

        //teste 
        r.testesDetalhados++;
        if (triangulos[i].PontoNoTriangulo(p)){
            r.contem.push_back(i);
        }
    }

    // Sem envelope, todos os candidatos "passam" para o teste detalhado
    r.aprovadosEnvelope = usaEnvelope ? r.aprovados.size() : n;

    auto fim = std::chrono::steady_clock::now();
    r.tempoMicros = std::chrono::duration<double, std::micro>(fim - inicio).count();

    return r;
}