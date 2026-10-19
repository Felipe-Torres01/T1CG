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

bool desenha = false;

float angulo=0.0;

int NIVEL_MAX;

int countNos, countTriangulosEmRegioes;

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
void CriaTriangulos()
{
    for (int i = 0; i < 10000; i++)
    {
        Ponto P1(Aleatorio(Min.x, Max.x), Aleatorio(Min.y, Max.y));
        Ponto P2(Aleatorio(Min.x, Max.x), Aleatorio(Min.y, Max.y));
        Ponto P3(Aleatorio(Min.x, Max.x), Aleatorio(Min.y, Max.y));

        int cor = rand() % (Black + 1);
        Triangulos.push_back(Triangulo(P1, P2, P3, cor));
    }
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

    
    NIVEL_MAX = 5;
    countNos = 0;
    raiz.aabb.centro = Ponto (0, 0);
    raiz.aabb.meiaLarg.x = 10.0f;
    raiz.aabb.meiaLarg.y = 5.0f;
    raiz.nivel = 0;
    divideQuadtree(raiz, NIVEL_MAX);
    percorreTriangulos(raiz,Triangulos);
    contaNos(countNos, raiz);
    cout << "Nos armazenados: " << countNos << endl;
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
void reshape( int w, int h )
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela
    
    glOrtho(Min.x,Max.x, Min.y,Max.y, 0,1);
    //glOrtho(0,10, 0,10, 0,1);

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

void AtualizaConsulta()
{
    if (!FoiClicado) return;
    resultado = executaConsulta(PontoClicado, Triangulos, raiz, usaQuadtree, usaEnvelope);
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
    // Limpa a tela coma cor de fundo
    glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites logicos da area OpenGL dentro da Janela
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    // Coloque aqui as chamadas das rotinas que desenham os objetos
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
   
    glLineWidth(1);
    glColor3f(0,0,0); // R, G, B  [0..1]
    DesenhaEixos();
    
    DesenhaTriangulos();
    
    /*
    if (FoiClicado)
    {
        PintaTriangulosQueContemPonto(PontoClicado);
        DesenhaPontoClicado();
    }
    */

    
        if (FoiClicado)
        {
            for (int i = 0; i < resultado.contem.size(); i++)
            {
                Triangulos[resultado.contem[i]].Pinta();
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
    GLint viewport[4];
    GLdouble modelview[16], projection[16];
    GLfloat wx = x, wy, wz;
    GLdouble ox = 0.0, oy = 0.0, oz = 0.0;

    glGetIntegerv(GL_VIEWPORT, viewport);

    // Inverte a coordenada Y
    y = viewport[3] - y;
    wy = y;

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    glReadPixels(x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);

    gluUnProject(wx, wy, wz,
                 modelview, projection, viewport,
                 &ox, &oy, &oz);

    return Ponto(ox, oy, oz);
}
// **********************************************************************
// void Mouse(int button, int state, int x, int y)
//      Trata o clique do botao esquerdo do mouse.
// **********************************************************************
void Mouse(int button, int state, int x, int y)
{
    if (state != GLUT_DOWN)
        return;

    if (button != GLUT_LEFT_BUTTON)
        return;

    PontoClicado = ConverteMouseParaUniverso(x, y);

    FoiClicado = true;
    AtualizaConsulta();
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
        case 'i':
            imprimeMat = true;
        default:
            break;
    }
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

    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}
