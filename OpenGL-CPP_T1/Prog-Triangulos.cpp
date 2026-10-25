// **********************************************************************
// PUCRS/Escola Politecnica
// COMPUTACAO GRAFICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************
 
// Para uso no Xcode:
// Abra o menu Product -> Scheme -> Edit Scheme -> Use custom working directory
// Selecione a pasta onde voce descompactou o ZIP que continha este arquivo.
//
//  Para rodar em um terminal Windows, digite
//           mingw32-make -f Makefile.mk
//

#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <random>
#include <cstdio>
#include <algorithm>
#include <cstdlib>

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

#include "Ponto.h"
#include "Quadtree.h"
#include "Consulta.h"
#include "Poligono.h"

#include "Temporizador.h"

#include "ListaDeCoresRGB.h"

// Limites logicos da area de desenho
Ponto Min, Max;

Poligono Poly, RS;

NoQuadtree raiz;

// Vetor de configuracoes 
// A cada atualiza consulta, o vetor avanca e armazena as informacoes de cada configuracao
// Quanto atingir tamanho 2, fazemos a operacao de verificacao em cada uma
std::vector<Configuracao> configuracoes;

// Armazena operacao de igualdade de triangulos das configuracoes
bool ehIgual = false;

bool desenha = false;

float angulo=0.0;

int NIVEL_MAX;

// Contador de nós presente na quadtree
int countNos;

Ponto PosicaoDoCampoDeVisao, PontoClicado;
bool FoiClicado = false;
bool imprimeMat = false;
bool usaQuadtree = true;
bool usaEnvelope = true;
ResultadoConsulta resultado;

#include <vector>

#include "Triangulo.h"


std::vector<Triangulo> Triangulos;



// **********************************************************************
// COMECO DOS METODOS/FUNCOES QUE OPERAM SOBRE OPCOES DE DESENHOS
// **********************************************************************

// **********************************************************************
// void opcaoDesenhaQuadtree()
//      Operacao no qual permite visualizar o desenho da quadtree
//      por meio de uma tecla. 
//      
//      Chamada na funcao keyboard()
//
//      Operacao liga/desliga:
//      opDesenhoQuadtree = true         Pode desenhar
//      opDesenhoQuadtree = false        Nao desenha
// **********************************************************************

bool opDesenhaQuadtree = false;
bool permiteDesenharQuadtree = false;

void opcaoDesenhaQuadtree(){
    opDesenhaQuadtree = !opDesenhaQuadtree;
    permiteDesenharQuadtree = opDesenhaQuadtree;
}

// **********************************************************************
// void realizaDesenhoQuadtree()
//      Operacao no qual verifica se da para realizar o desenho da
//      quadtree, chamando o metodo principal.  
//
//      Chama na funcao display() para exibicao
//
//      Operacao liga/desliga:
//      permiteDesenharQuadtree = true         Pode desenhar
//      permiteDesenharQuadtree = false        Nao desenha
// **********************************************************************

void realizaDesenhoQuadtree(){
    if (permiteDesenharQuadtree){
        desenhaQuadtree(raiz);
    }
}




// **********************************************************************
// void opcaoDestacaRegiaoContemPonto()
//      Operacao no qual permite visualizar o destaque da regiao que contem
//      o ponto por meio de uma tecla. 
//
//      Chamada na funcao keyboard()
//
//      Operacao liga/desliga:
//      opcaoDestacaRegiao = true         Pode destacar
//      opcaoDestacaRegiao = false        Nao destaca
// **********************************************************************

bool opcaoDestacaRegiao = false;
bool permiteDestacarRegiao = false;

void opcaoDestacaRegiaoContemPonto(){
    opcaoDestacaRegiao = !opcaoDestacaRegiao;

    switch (opcaoDestacaRegiao){
        case true:
            permiteDestacarRegiao = true;
        break;

        case false:
            permiteDestacarRegiao = false;
        break;

        deafult:
            permiteDestacarRegiao = false;
        break;
    }
}

// **********************************************************************
// void destacaRegiaoContemPonto()
//      Operacao no qual verifica se da para realizar o destaque da
//      regiao, chamando o metodo principal.  
//
//      Chama na funcao display() para exibicao
//
//      Operacao liga/desliga:
//      permiteDestacarRegiao = true         Pode destacar
//      permiteDestacarRegiao = false        Nao destaca
// **********************************************************************

void destacaRegiaoContemPonto(){
    if (permiteDestacarRegiao && FoiClicado){
        NoQuadtree* regiaoContemPonto = localizaRegiaoDoPonto(PontoClicado, raiz);
        if (regiaoContemPonto != NULL){
            colorirRegiao(*regiaoContemPonto, Blue + 1);
        }
    }
}




// **********************************************************************
// void opcaoDesenhaEnvelopes()
//      Operacao no qual permite visualizar os desenhos das bounding boxes
//      dos triangulos por meio de uma tecla. 
//
//      Chamada na funcao keyboard()
//
//      Operacao liga/desliga:
//      opcaoDesenhaEnvelope = true         Pode desenhar
//      opcaoDesenhaEnvelope = false        Nao desenha
// **********************************************************************

bool opcaoDesenhaEnvelope = false;
bool permiteDesenharEnvelope = false;

void opcaoDesenhaEnvelopes(){
    opcaoDesenhaEnvelope = !opcaoDesenhaEnvelope;

    switch (opcaoDesenhaEnvelope){
        case true:
            permiteDesenharEnvelope = true;
        break;

        case false:
            permiteDesenharEnvelope = false;
        break;

        default:
            permiteDesenharEnvelope = false;
        break;
    }
}

// **********************************************************************
// void desenhaEnvelopes()
//      Operacao no qual verifica se da para realizar os desenhos das
//      bounding boxes dos triangulos, chamando o metodo principal.  
//
//      Chama na funcao display() para exibicao
//
//      Operacao liga/desliga:
//      permiteDesenharEnvelope = true         Pode destacar
//      permiteDesenharEnvelope = false        Nao destaca
// **********************************************************************

void desenhaEnvelopes(){
    if (permiteDesenharEnvelope){
        for (int i = 0; i < Triangulos.size(); i++){
            Triangulos[i].DesenhaEnvelope();
        }
    }
}





// **********************************************************************
// void opcaoDesenhaCandidatosQuadtree()
//      Operacao no qual permite visualizar os candidatos do teste da 
//      quadtree por meio de uma tecla. 
//
//      Chamada na funcao keyboard()
//
//      Operacao liga/desliga:
//      opcaoDesenhaCandidato = true         Pode desenhar
//      opcaoDesenhaCandidato = false        Nao desenha
// **********************************************************************

bool opcaoDesenhaCandidato = false;
bool permiteDesenharCandidatos = false;

void opcaoDesenhaCandidatosQuadtree(){
    opcaoDesenhaCandidato = !opcaoDesenhaCandidato;

    switch (opcaoDesenhaCandidato){
        case true:
            permiteDesenharCandidatos = true;
        break;

        case false:
            permiteDesenharCandidatos = false;
        break;

        default:
            permiteDesenharCandidatos = false;
        break;
    }
}

// **********************************************************************
// void desenhaCandidadosQuadtree()
//      Operacao no qual verifica se da para realizar os desenhos dos
//      candidatos da quadtree, chamando o metodo principal.  
//
//      Chama na funcao display() para exibicao
//
//      Operacao liga/desliga:
//      permiteDesenharCandidatos = true         Pode destacar
//      permiteDesenharCandidatos = false        Nao destaca
// **********************************************************************

void desenhaCandidadosQuadtree(){
    if (permiteDesenharCandidatos){
        desenhaCandidatosDaQuadtree(resultado, Triangulos);
    }
}






// **********************************************************************
// void opcaoDesenhaAprovadosEnvelope()
//      Operacao no qual permite visualizar os triangulos aprovados pelo
//      teste de envelope por meio de uma tecla. 
//
//      Chamada na funcao keyboard()
//
//      Operacao liga/desliga:
//      opcaoAprovadosEnvelope = true         Pode desenhar
//      opcaoAprovadosEnvelope = false        Nao desenha
// **********************************************************************

bool opcaoAprovadosEnvelope = false;
bool permiteDesenharAprovadosEnvelope = false;

void opcaoDesenhaAprovadosEnvelope(){
    opcaoAprovadosEnvelope = !opcaoAprovadosEnvelope;

    switch (opcaoAprovadosEnvelope){
        case true:
            permiteDesenharAprovadosEnvelope = true;
        break;

        case false:
            permiteDesenharAprovadosEnvelope = false;
        break;

        default:
            permiteDesenharAprovadosEnvelope = false;
        break;
    }
}

// **********************************************************************
// void desenhaAprovadosEnvelope()
//      Operacao no qual verifica se da para realizar os desenhos dos
//      aprovados no envelope, chamando o metodo principal.  
//
//      Chama na funcao display() para exibicao
//
//      Operacao liga/desliga:
//      permiteDesenharAprovadosEnvelope = true         Pode destacar
//      permiteDesenharAprovadosEnvelope = false        Nao destaca
// **********************************************************************

void desenhaAprovadosEnvelope(){
    if (permiteDesenharAprovadosEnvelope){
        desenhaAprovadosPeloEnvelope(resultado, Triangulos);
    }
}






// **********************************************************************
// opcaoDesenhaTriangulosContemPonto()
//      Operacao no qual permite visualizar os triangulos que contem o ponto
//      pelo teste de forca bruta por meio de uma tecla. 
//
//      Operacao liga/desliga:
//      opcaoTriangulosContemPonto = true         Pode desenhar
//      opcaoTriangulosContemPonto = false        Nao desenha
// **********************************************************************

bool opcaoTriangulosContemPonto = false;
bool permiteDesenharTriangulosContemPonto = false;

void opcaoDesenhaTriangulosContemPonto(){
    opcaoTriangulosContemPonto = !opcaoTriangulosContemPonto;

    switch (opcaoTriangulosContemPonto){
        case true:
            permiteDesenharTriangulosContemPonto = true;
        break;

        case false:
            permiteDesenharTriangulosContemPonto = false;
        break;

        default:
            permiteDesenharTriangulosContemPonto = false;
        break;
    }
}

// **********************************************************************
// void desenhaTriangulosContemPonto()
//      Operacao no qual verifica se da para realizar os desenhos dos
//      triangulos que contem o ponto, chamando o metodo principal.  
//
//      Chama na funcao display() para exibicao
//
//      Operacao liga/desliga:
//      permiteDesenharTriangulosContemPonto = true         Pode destacar
//      permiteDesenharTriangulosContemPonto = false        Nao destaca
// **********************************************************************

void desenhaTriangulosContemPonto(){
    if (permiteDesenharTriangulosContemPonto)
        for (int i = 0; i < (int)resultado.contem.size(); i++)
            Triangulos[resultado.contem[i]].Pinta(Red);
}










// **********************************************************************
// COMECO DOS METODOS/FUNCOES GERAIS DO PROGRAMA
// **********************************************************************

// **********************************************************************
// float Aleatorio(float min, float max)
//      Gera e retorna um numero real aleatorio no intervalo [min, max].
// **********************************************************************
float Aleatorio(float min, float max)
{
    return min + (rand() / (float)RAND_MAX) * (max - min);
}

// **********************************************************************
// void CriaTriangulos()
//      Cria triangulos com vertices e cores aleatorias.
// **********************************************************************
// Parametros do cenario (podem ser passados na linha de comando)
int N_TRIANGULOS = 10000;
unsigned int SEMENTE = 1;
float TAM_TRIANGULO = 0.5f;   // "raio" medio dos triangulos, em unidades do cenario

void CriaTriangulos()
{
    srand(SEMENTE);
    Triangulos.clear();
    for (int i = 0; i < N_TRIANGULOS; i++)
    {
        float cx = Aleatorio(Min.x, Max.x);
        float cy = Aleatorio(Min.y, Max.y);
        Ponto P1(cx + Aleatorio(-TAM_TRIANGULO, TAM_TRIANGULO), cy + Aleatorio(-TAM_TRIANGULO, TAM_TRIANGULO));
        Ponto P2(cx + Aleatorio(-TAM_TRIANGULO, TAM_TRIANGULO), cy + Aleatorio(-TAM_TRIANGULO, TAM_TRIANGULO));
        Ponto P3(cx + Aleatorio(-TAM_TRIANGULO, TAM_TRIANGULO), cy + Aleatorio(-TAM_TRIANGULO, TAM_TRIANGULO));

        int cor = rand() % (Black + 1);
        Triangulos.push_back(Triangulo(P1, P2, P3, cor));
    }
    cout << "Cenario: semente=" << SEMENTE << " N=" << N_TRIANGULOS
         << " tamanho=" << TAM_TRIANGULO << endl;
}

// ----------------------------------------------------------------------
// BENCHMARK: roda as 4 configuracoes sobre as MESMAS posicoes de consulta,
// confere cada resultado com a forca bruta e acrescenta linhas em benchmark.csv
// ----------------------------------------------------------------------
int NIVEL_INICIAL = 5;        // nivel da quadtree ao iniciar (argumento 4)
bool MODO_BENCH = false;      // se verdadeiro, roda o benchmark e encerra (argumento 5)

void Benchmark(int nConsultas)
{
    std::mt19937 gerador(12345);   // semente propria: mesmas posicoes em todas as execucoes
    std::uniform_real_distribution<float> dx(Min.x, Max.x), dy(Min.y, Max.y);
    std::uniform_real_distribution<float> frac(0.0f, 1.0f);
    std::uniform_int_distribution<int> sorteiaVert(0, 2);
    int nTri = (int)Triangulos.size();
    if (nTri == 0) return;
    std::uniform_int_distribution<int> sorteiaTri(0, nTri - 1);
    std::uniform_int_distribution<int> sorteiaCanto(0, 3);

    const int NTIPOS = 4;
    const char* nomeTipo[NTIPOS] = {"uniforme", "dentro_de_triangulos", "vertices", "cantos"};

    const bool q[4] = {false, false, true, true};
    const bool e[4] = {false, true, false, true};
    const char* nome[4] = {"forca bruta", "so envelope", "so quadtree", "quadtree+envelope"};

    // cabecalho do CSV (so se o arquivo ainda nao existe ou esta vazio)
    bool escreveCabecalho = false;
    {
        std::ifstream teste("benchmark.csv");
        escreveCabecalho = !teste.good() || teste.peek() == std::ifstream::traits_type::eof();
    }
    std::ofstream csv("benchmark.csv", std::ios::app);
    if (escreveCabecalho)
        csv << "N;semente;tamanho;nivel;nos;posicoes;config;consultas;tempo_medio_us;cand_quadtree;aprov_envelope;testes_detalhados;contem;divergencias" << endl;

    cout << "=== Benchmark: N=" << N_TRIANGULOS << " semente=" << SEMENTE << " tamanho=" << TAM_TRIANGULO
         << " nivel=" << NIVEL_MAX << " nos=" << countNos << " consultas=" << nConsultas << " ===" << endl;

    for (int tipo = 0; tipo < NTIPOS; tipo++)
    {
        // ---- monta as posicoes deste tipo (iguais para as 4 configuracoes) ----
        std::vector<Ponto> pos;
        for (int i = 0; i < nConsultas; i++)
        {
            float x = 0, y = 0;
            bool dentro = false;
            int tentativas = 0;
            // so valem posicoes DENTRO da janela (e nela que o usuario consegue clicar;
            // triangulos podem ter vertices alem da borda, esses pontos sao descartados)
            do
            {
                if (tipo == 0)                       // uniforme em todo o cenario
                {
                    x = dx(gerador); y = dy(gerador);
                }
                else if (tipo == 1)                  // centro de um triangulo sorteado
                {
                    Triangulo& T = Triangulos[sorteiaTri(gerador)];
                    x = (T.getVertices(0).x + T.getVertices(1).x + T.getVertices(2).x) / 3.0f;
                    y = (T.getVertices(0).y + T.getVertices(1).y + T.getVertices(2).y) / 3.0f;
                }
                else if (tipo == 2)                  // exatamente sobre um vertice (caso de borda: conta como dentro)
                {
                    Triangulo& T = Triangulos[sorteiaTri(gerador)];
                    Ponto v = T.getVertices(sorteiaVert(gerador));
                    x = v.x; y = v.y;
                }
                else                                 // faixa de 10% perto de um canto da janela
                {
                    int c = sorteiaCanto(gerador);
                    float fx = frac(gerador) * 0.1f * (Max.x - Min.x);
                    float fy = frac(gerador) * 0.1f * (Max.y - Min.y);
                    x = (c & 1) ? Max.x - fx : Min.x + fx;
                    y = (c & 2) ? Max.y - fy : Min.y + fy;
                }
                dentro = (x >= Min.x && x <= Max.x && y >= Min.y && y <= Max.y);
                tentativas++;
            } while (!dentro && tentativas < 1000);

            if (!dentro) { x = dx(gerador); y = dy(gerador); }   // plano B: uniforme
            pos.push_back(Ponto(x, y));
        }

        for (int c = 0; c < 4; c++)
        {
            // aquecimento (nao conta)
            for (int i = 0; i < 50 && i < nConsultas; i++)
                executaConsulta(pos[i], Triangulos, raiz, q[c], e[c]);

            double tempo = 0, cand = 0, aprov = 0, det = 0, cont = 0;
            int diverge = 0;
            for (int i = 0; i < nConsultas; i++)
            {
                ResultadoConsulta r = executaConsulta(pos[i], Triangulos, raiz, q[c], e[c]);
                tempo += r.tempoMicros;
                cand  += r.candidatosQuadtree;
                aprov += r.aprovadosEnvelope;
                det   += r.testesDetalhados;
                cont  += r.contem.size();

                ResultadoConsulta ref = executaConsulta(pos[i], Triangulos, raiz, false, false);
                std::vector<int> a = r.contem, b = ref.contem;
                std::sort(a.begin(), a.end());
                std::sort(b.begin(), b.end());
                if (a != b) diverge++;
            }
            tempo /= nConsultas; cand /= nConsultas; aprov /= nConsultas; det /= nConsultas; cont /= nConsultas;

            printf("%-20s %d) %-18s tempo=%9.2f us | cand=%9.1f | aprov=%9.1f | testes=%9.1f | contem=%6.2f | divergencias=%d\n",
                   nomeTipo[tipo], c + 1, nome[c], tempo, cand, aprov, det, cont, diverge);

            csv << N_TRIANGULOS << ";" << SEMENTE << ";" << TAM_TRIANGULO << ";" << NIVEL_MAX << ";" << countNos << ";"
                << nomeTipo[tipo] << ";" << nome[c] << ";" << nConsultas << ";" << tempo << ";" << cand << ";" << aprov << ";"
                << det << ";" << cont << ";" << diverge << endl;
        }
    }
    cout << "(linhas acrescentadas em benchmark.csv)" << endl << endl;
}

void AtualizaConsulta();   // declarada aqui porque e definida mais abaixo

void ReconstroiQuadtree()
{
    raiz = NoQuadtree();           // zera filhos e indices
    raiz.aabb.centro = Ponto(0, 0);
    raiz.aabb.meiaLarg.x = 10.0f;
    raiz.aabb.meiaLarg.y = 5.0f;
    raiz.nivel = 0;

    raiz.aabb.limitesMin = Ponto (raiz.aabb.centro.x - raiz.aabb.meiaLarg.x, raiz.aabb.centro.y - raiz.aabb.meiaLarg.y);
    raiz.aabb.limitesMax = Ponto (raiz.aabb.centro.x + raiz.aabb.meiaLarg.x, raiz.aabb.centro.y + raiz.aabb.meiaLarg.y);

    divideQuadtree(raiz, NIVEL_MAX);
    percorreTriangulos(raiz, Triangulos);
    countNos = 0;
    contaNos(countNos, raiz);
    cout << "Quadtree: nivel=" << NIVEL_MAX << " nos=" << countNos << endl;

    resultado = ResultadoConsulta();   // o ponteiro "regiao" antigo ficou invalido
    AtualizaConsulta();                // refaz a consulta se ja houver um ponto
}

// **********************************************************************
// void init()
//      Inicializa os parametros da aplicacao e cria os triangulos.
// **********************************************************************
void init()
{
    // Define a cor do fundo da tela (AZUL)
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // define os limites da janela de selecao (Window)
    Min = Ponto (-10, -5);
    Max = Ponto (10, 5);

    // Gera os triangulos aleatorios
    CriaTriangulos();

    
    NIVEL_MAX = NIVEL_INICIAL;
    ReconstroiQuadtree();
}

double nFrames=0;
double TempoTotal=0;
Temporizador T;
double AccumDeltaT=0;
// **********************************************************************
// void ImprimeModelView()
//      Imprime a matriz MODELVIEW corrente.
// **********************************************************************
void ImprimeModelView()
{
    GLfloat M[16];

    glGetFloatv(GL_MODELVIEW_MATRIX, M);

    printf("Matriz MODELVIEW:\n");

    for (int lin = 0; lin < 4; lin++)
    {
        for (int col = 0; col < 4; col++)
        {
            printf("%8.3f ", M[col * 4 + lin]);
        }
        printf("\n");
    }
}
// **********************************************************************
// void animate()
//      Atualiza os parametros da animacao e solicita o redesenho da tela.
// **********************************************************************
void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;
    
    if (AccumDeltaT > 1.0/30) // fixa a atualizacao da tela em 30
    {
        AccumDeltaT = 0;
        angulo+=1.0;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0)
    {
        cout << "Tempo Acumulado: "  << TempoTotal << " segundos. " ;
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames/TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
    }
}
// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
//
// **********************************************************************
int LARG_JANELA = 1000, ALT_JANELA = 500;   // tamanho atual da janela em pixels

void reshape( int w, int h )
{
    LARG_JANELA = w;
    ALT_JANELA = h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    glOrtho(Min.x,Max.x, Min.y,Max.y, 0,1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// **********************************************************************
// void DesenhaEixos()
//      Desenha os eixos horizontal e vertical da area de desenho.
// **********************************************************************
void DesenhaEixos()
{
    Ponto Meio;
    Meio.x = (Max.x+Min.x)/2;
    Meio.y = (Max.y+Min.y)/2;
    Meio.z = (Max.z+Min.z)/2;

    glLineWidth(3);
    glBegin(GL_LINES);
    //  eixo horizontal
        glVertex2f(Min.x,Meio.y);
        glVertex2f(Max.x,Meio.y);
    //  eixo vertical
        glVertex2f(Meio.x,Min.y);
        glVertex2f(Meio.x,Max.y);
    glEnd();
    glLineWidth(1);
}

// **********************************************************************
// void RotacionaAoRedorDeUmPonto(float alfa, Ponto P)
//      Define uma rotacao de alfa graus ao redor do ponto P.
// **********************************************************************
void RotacionaAoRedorDeUmPonto(float alfa, Ponto P)
{
    glTranslatef(P.x, P.y, P.z);
    glRotatef(alfa, 0,0,1);
    glTranslatef(-P.x, -P.y, -P.z);
}

// **********************************************************************
// void DesenhaTriangulos()
//      Desenha todos os triangulos armazenados no vetor Triangulos.
// **********************************************************************
void DesenhaTriangulos()
{
    for (int i = 0; i < Triangulos.size(); i++)
    {
        Triangulos[i].Desenha();
    }
}

// **********************************************************************
// void PintaTriangulosQueContemPonto(Ponto P)
//      Pinta todos os triangulos que contem o ponto P.
// **********************************************************************
void PintaTriangulosQueContemPonto(Ponto P)
{
    for (int i = 0; i < Triangulos.size(); i++)
    {
        if (Triangulos[i].PontoNoTriangulo(P))
        {
            Triangulos[i].Pinta();
        }
    }
}

// **********************************************************************
// void PintaTriangulosQueContemPonto(Ponto P)
//      Alternativa com quadtree para pintar todos os triangulos que contem o ponto P.
//      descartando o teste de força bruta para todos os triangulos (que não tem o ponto) na tela
// **********************************************************************
void OutroPintaTriangulosQueContemPonto(Ponto p, const vector<int>& triangulosEncontrados)
{
    for (int i = 0; i < triangulosEncontrados.size(); i++)
    {
        if (Triangulos[triangulosEncontrados[i]].PontoNoTriangulo(p)){
            Triangulos[triangulosEncontrados[i]].Pinta();
        }
    }
}

// **********************************************************************
// void DesenhaPontoClicado()
//      Desenha o ultimo ponto clicado pelo usuario.
// **********************************************************************
void DesenhaPontoClicado()
{
    glPointSize(8);

    defineCor(Black);

    glBegin(GL_POINTS);
        glVertex2f(PontoClicado.x, PontoClicado.y);
    glEnd();

    glPointSize(1);
}
void ImprimeResultado()
{
    cout << "Quadtree: " << (usaQuadtree ? "ON" : "OFF")
         << " | Envelope: " << (usaEnvelope ? "ON" : "OFF") << endl;
    cout << "Total de triangulos: " << resultado.totalTriangulos << endl;
    cout << "Candidatos da quadtree: " << resultado.candidatosQuadtree << endl;
    cout << "Aprovados pelo envelope: " << resultado.aprovadosEnvelope << endl;
    cout << "Testes detalhados: " << resultado.testesDetalhados << endl;
    cout << "Triangulos que contem o ponto: " << resultado.contem.size() << endl;
    cout << "Tempo: " << resultado.tempoMicros << " us" << endl << endl;
}

// **********************************************************************
// void verificaConfiguracao(int countConfiguracao)
//      Antes da verificacao, precisa atingir tamanho maximo do vetor de
//      configuracoes. Esvazia vetor depois para comecar de novo a verificacao
// **********************************************************************
void verificaConfiguracoes(vector<Configuracao> configuracoes){
    if (configuracoes.size() == 2){
        ehIgual = (seTodosOsTriangulosDeCadaConfiguracaoSaoIguais(configuracoes[0], configuracoes[1]));
        cout << "Quantidade de triangulos iguais? : " << ehIgual << endl;

        // Esvazia vector
        while (!configuracoes.empty()){
            configuracoes.pop_back();
        }
    }
}

void AtualizaConsulta()
{
    if (!FoiClicado) return;
    resultado = executaConsulta(PontoClicado, Triangulos, raiz, usaQuadtree, usaEnvelope);

    // Referencia: forca bruta (config 1). Todas as configuracoes devem dar o mesmo conjunto final.
    ResultadoConsulta ref = executaConsulta(PontoClicado, Triangulos, raiz, false, false);
    std::vector<int> a = resultado.contem, b = ref.contem;
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    cout << (a == b ? "[OK] resultado igual ao da forca bruta" : "[ERRO] DIVERGE da forca bruta") << endl;

    ImprimeResultado();
}

void MudaConfiguracao(bool quadtree, bool envelope)
{
    usaQuadtree = quadtree;
    usaEnvelope = envelope;
    AtualizaConsulta();
    glutPostRedisplay();
}
// **********************************************************************
// void display(void)
//      Desenha a cena e destaca os triangulos que contem o ponto clicado.
// **********************************************************************
void display( void )
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 1) fundo da regiao da quadtree (antes dos triangulos, senao cobre tudo)
    destacaRegiaoContemPonto();

    glLineWidth(1);
    glColor3f(0,0,0);
    DesenhaEixos();

    // 2) triangulos base
    DesenhaTriangulos();

    // 3) camadas opcionais (teclas 5 a 9)
    desenhaEnvelopes();
    realizaDesenhoQuadtree();

    if (FoiClicado)
    {
        desenhaCandidadosQuadtree();      // laranja
        desenhaAprovadosEnvelope();       // violeta

        // 4) resultado final: triangulos que contem o ponto (vermelho)
        for (int i = 0; i < (int)resultado.contem.size(); i++)
        {
            Triangulos[resultado.contem[i]].Pinta(Red);
        }
        DesenhaPontoClicado();
    }

    glutSwapBuffers();
}
// **********************************************************************
// void ContaTempo(double tempo)
//      Conta um intervalo de tempo e informa quantos frames ocorreram.
// **********************************************************************
void ContaTempo(double tempo)
{
    Temporizador T;

    unsigned long cont = 0;
    cout << "Inicio contagem de " << tempo << "segundos ..." << flush;
    while(true)
    {
        tempo -= T.getDeltaT();
        cont++;
        if (tempo <= 0.0)
        {   
            cout << "fim! - Passaram-se " << cont << " frames." << endl;
            break;
        }
    }

}
// **********************************************************************
// Ponto ConverteMouseParaUniverso(int x, int y)
//      Converte as coordenadas do mouse para as coordenadas do universo.
// Este codigo eh baseado em http://hamala.se/forums/viewtopic.php?t=20
// **********************************************************************
Ponto ConverteMouseParaUniverso(int x, int y)
{
    // x cresce para a direita; y do mouse cresce para BAIXO (por isso a inversao)
    float wx = Min.x + (x / (float)LARG_JANELA) * (Max.x - Min.x);
    float wy = Max.y - (y / (float)ALT_JANELA)  * (Max.y - Min.y);
    return Ponto(wx, wy, 0);
}
// **********************************************************************
// void Mouse(int button, int state, int x, int y)
//      Trata o clique do botao esquerdo do mouse.
// **********************************************************************
// Mostra o ponto e os contadores na barra de titulo (usado durante o arrasto)
void AtualizaTitulo()
{
    char buf[300];
    snprintf(buf, sizeof(buf),
        "P(%.2f, %.2f) | total=%d cand=%d aprov=%d testes=%d contem=%d | %.1f us",
        PontoClicado.x, PontoClicado.y, resultado.totalTriangulos,
        resultado.candidatosQuadtree, resultado.aprovadosEnvelope,
        resultado.testesDetalhados, (int)resultado.contem.size(), resultado.tempoMicros);
    glutSetWindowTitle(buf);
}

// Arrastando com o botao esquerdo: atualiza a consulta sem encher o console
void MouseMovimento(int x, int y)
{
    PontoClicado = ConverteMouseParaUniverso(x, y);
    FoiClicado = true;
    resultado = executaConsulta(PontoClicado, Triangulos, raiz, usaQuadtree, usaEnvelope);
    AtualizaTitulo();
    glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
    if (button != GLUT_LEFT_BUTTON)
        return;

    PontoClicado = ConverteMouseParaUniverso(x, y);
    FoiClicado = true;

    // ao apertar e ao soltar: consulta completa, com contadores e validacao no console
    AtualizaConsulta();
    AtualizaTitulo();
    PontoClicado.imprime("- Ponto no universo: ", "\n");

    glutPostRedisplay();
}
// **********************************************************************
// void keyboard(unsigned char key, int x, int y)
//      Trata as teclas comuns pressionadas pelo usuario.
// **********************************************************************
void keyboard ( unsigned char key, int x, int y )
{

    switch ( key )
    {
        case 27:        // Termina o programa qdo
            exit ( 0 );   // a tecla ESC for pressionada
            break;
        case 't':
            ContaTempo(3);
            break;
        case ' ':
            desenha = !desenha;
        break;
        case '1': MudaConfiguracao(false, false); break;
        case '2': MudaConfiguracao(false, true);  break;
        case '3': MudaConfiguracao(true,  false); break;
        case '4': MudaConfiguracao(true,  true);  break;
        // Trata de desenhos liga/desliga na tela
        case '5': opcaoDesenhaQuadtree(); break;
        case '6': opcaoDestacaRegiaoContemPonto(); break;
        case '7': opcaoDesenhaEnvelopes(); break;
        case '8': opcaoDesenhaCandidatosQuadtree(); break;
        case '9': opcaoDesenhaAprovadosEnvelope(); break;
        case '0': opcaoDesenhaTriangulosContemPonto(); break;
        case '+':
        case '=':
            if (NIVEL_MAX < 8) NIVEL_MAX++;
            ReconstroiQuadtree();
            break;
        case '-':
            if (NIVEL_MAX > 1) NIVEL_MAX--;
            ReconstroiQuadtree();
            break;
        case 'b':
            Benchmark(1000);
            break;
        case 'i':
            imprimeMat = true;
        default:
            break;
    }
    glutPostRedisplay();   // fora do switch: vale para qualquer tecla
}
// **********************************************************************
// void arrow_keys(int a_keys, int x, int y)
//      Trata as teclas especiais pressionadas pelo usuario.
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{
    switch ( a_keys )
    {
        case GLUT_KEY_UP:       // Se pressionar UP
            glutFullScreen ( ); // Vai para Full Screen
            break;
        case GLUT_KEY_RIGHT:       // Se pressionar UP
            angulo--;
            break;
        case GLUT_KEY_LEFT:       // Se pressionar UP
            angulo++;
            break;
        case GLUT_KEY_DOWN:     // Se pressionar UP
                                // Reposiciona a janela
            glutPositionWindow (50,50);
            glutReshapeWindow ( 700, 500 );
            break;
        default:
            break;
    }
}

// **********************************************************************
// int main(int argc, char** argv)
//      Inicializa o GLUT, registra as funcoes de callback e inicia o programa.
// **********************************************************************
int  main ( int argc, char** argv )
{
    cout << "Programa OpenGL" << endl;
    if (argc > 1) N_TRIANGULOS = atoi(argv[1]);
    if (argc > 2) SEMENTE = (unsigned int) atoi(argv[2]);
    if (argc > 3) TAM_TRIANGULO = (float) atof(argv[3]);
    if (argc > 4) NIVEL_INICIAL = atoi(argv[4]);
    if (argc > 5) MODO_BENCH = true;
    

    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 1000, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de titulo da janela.
    glutCreateWindow    ( "Trabalho 1 - Triangulos" );

    // executa algumas inicializaes
    init ();
    if (MODO_BENCH) { Benchmark(1000); return 0; }

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // serah chamada automaticamente quando
    // for necessario redesenhar a janela
    glutDisplayFunc ( display );

    // Define que o tratador de evento para
    // o invalidao da tela. A funcao "display"
    // ser chamada automaticamente sempre que a
    // mquina estiver ociosa (idle)
    //glutIdleFunc(animate); para desenhar apenas quando alguém alterar

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // ser chamada automaticamente quando
    // o usurio alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // ser chamada automaticamente sempre
    // o usurio pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" ser chamada
    // automaticamente sempre o usurio
    // pressionar uma tecla especial
    glutSpecialFunc ( arrow_keys );
    glutMouseFunc(Mouse);
    glutMotionFunc(MouseMovimento);   // arrastar o ponto

    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}
