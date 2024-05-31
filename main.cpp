#include <stdio.h>
#include <SDL.h>
#include "SDL_opengl.h"
#include <math.h> // Para funções matemáticas

#define BLOCOS 10 //Quantidade de Blocos

//Lógica da colisão
bool colisao(float Ax, float Ay, float Acomp, float Aalt, float Bx, float By, float Bcomp, float Balt)
{
    // Verifica se o retângulo A está à direita do retângulo B
    if (Ax >= Bx + Bcomp || Bx >= Ax + Acomp) return false;

    // Verifica se o retângulo A está abaixo do retângulo B
    if (Ay >= By + Balt || By >= Ay + Aalt) return false;

    // Se nenhuma das condições acima for atendida, os retângulos estão colidindo
    return true;
}

struct Bloco
{
    float blocoX;
    float blocoY;
    float blocoComp;
    float blocoAlt;
    bool vivo;
};

// Função para desenhar uma forma de círculo
void drawCircle(float cx, float cy, float raio, int num_segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        float x = raio * cosf(theta);
        float y = raio * sinf(theta);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

int main(int argc, char* args[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    // Reservando memória
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Criando janela
    SDL_Window *screen = SDL_CreateWindow("Meu jogo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
    if (screen == nullptr) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Criando contexto OpenGL
    SDL_GLContext glContext = SDL_GL_CreateContext(screen);
    if (glContext == nullptr) {
        printf("Erro ao criar contexto OpenGL: %s\n", SDL_GetError());
        SDL_DestroyWindow(screen);
        SDL_Quit();
        return 1;
    }

    // Limpar a cor da tela
    glClearColor(1, 1, 1, 1);

    // Define a área de visualização
    glViewport(0, 0, 640, 480);

    // Define o modelo de sombreamento
    glShadeModel(GL_SMOOTH);

    // Modo de matriz para desenho 2D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // Define a matriz de projeção como matriz identidade

    // Define a matriz de projeção usando glOrtho
    glOrtho(0, 640, 400, 0, -1, 1);

    // Desabilitar o teste de profundidade para desenho 2D
    glDisable(GL_DEPTH_TEST);

    bool executando = true;
    SDL_Event eventos;

    //Personagem 1
    float perso1X = 300;
    float perso1Y = 350;
    float perso1Comp = 50;
    float perso1Alt = 10;

    //Personagem Bola
    float bolaX = 50;
    float bolaY = 50;
    float bolaRaio = 15; // Raio da bola do inimigo

    float velX = 0.05;
    float velY = 0.05;

    // Inicialização dos blocos
    Bloco blocos[BLOCOS];
    for (int i = 0, blocoX = 4, blocoY = 10; i < BLOCOS; i++, blocoX += 61)
    {
        if (blocoX > 560)
        {
            blocoX = 4;
            blocoY += 23;
        }
        blocos[i].blocoX = blocoX;
        blocos[i].blocoY = blocoY;
        blocos[i].blocoComp = 60;
        blocos[i].blocoAlt = 30;
        blocos[i].vivo = true;
    }

    bool esq = false, dir = false;

    // Loop do jogo
    while (executando)
    {
        // Evento
        while (SDL_PollEvent(&eventos))
        {
            // Fechar com o x da janela
            if (eventos.type == SDL_QUIT)
            {
                executando = false;
            }
            // Fechar com o Esc da janela
            if (eventos.type == SDL_KEYUP && eventos.key.keysym.sym == SDLK_ESCAPE)
            {
                executando = false;
            }
            // Anda ou Não
            if(eventos.type == SDL_KEYDOWN)//Tecla Pressionada
            {
                if(eventos.key.keysym.sym == SDLK_LEFT)
                {
                    esq = true;
                }
                else if(eventos.key.keysym.sym == SDLK_RIGHT)
                {
                    dir = true;
                }
            }
            else if (eventos.type == SDL_KEYUP)//Tecla solta
            {
                if(eventos.key.keysym.sym == SDLK_LEFT)
                {
                    esq = false;
                }
                else if(eventos.key.keysym.sym == SDLK_RIGHT)
                {
                    dir = false;
                }

            }
        }

        // Movimenta do perso1
        const float velocidade = 0.1f; // Define a velocidade de movimento
        if (esq) // Mover para a esquerda
        {
            perso1X -= velocidade;
            if (perso1X < 0) // Verifica se o perso1 está fora da janela na esquerda
            {
                perso1X = 0;
            }
        }
        if (dir) // Mover para a direita
        {
            perso1X += velocidade;
            if (perso1X + perso1Comp > 640) // Verifica se o perso1 está fora da janela na direita
            {
                perso1X = 640 - perso1Comp;
            }
        }

        // Movimenta do bola
        bolaX += velX;
        for(int i = 0; i < BLOCOS; i++)
        {
            if(blocos[i].vivo == true)
            {
                if (colisao(bolaX, bolaY, bolaRaio * 2, bolaRaio * 2, blocos[i].blocoX, blocos[i].blocoY, blocos[i].blocoComp, blocos[i].blocoAlt))
                {
                    velX = -velX;
                    blocos[i].vivo = false;
                    break;
                }
            }
        }
        bolaY += velY;
        for(int i = 0; i < BLOCOS; i++)
        {
            if(blocos[i].vivo == true)
            {
                if (colisao(bolaX, bolaY, bolaRaio * 2, bolaRaio * 2, blocos[i].blocoX, blocos[i].blocoY, blocos[i].blocoComp, blocos[i].blocoAlt))
                {
                    velY = -velY;
                    blocos[i].vivo = false;
                    break;
                }
            }
        }

        // Logica do bols

        if(bolaX < 0 || bolaX + bolaRaio * 2 > 640)
        {
            velX = -velX;
        }
        if(bolaY < 0)
        {
            velY = -velY;
        }
        else if(bolaY + bolaRaio * 2 > 400)
        {
            executando = false;
            printf("\nGame Over\n");
        }
        // Colisão perso1 com a bola
        if(colisao(bolaX, bolaY, bolaRaio * 2, bolaRaio * 2, perso1X, perso1Y, perso1Comp, perso1Alt) == true)
        {
            velY = -velY;
        }

        // Renderização
        glClear(GL_COLOR_BUFFER_BIT); // Limpar o buffer

        // Cor do perso1 vermelho
        glColor4ub(255, 0, 0, 255); // Vermelho

        // Desenho do perso1
        glBegin(GL_QUADS);
        glVertex2f(perso1X, perso1Y); // primeiro ponto
        glVertex2f(perso1X + perso1Comp, perso1Y); // segundo ponto
        glVertex2f(perso1X + perso1Comp, perso1Y + perso1Alt); // terceiro ponto
        glVertex2f(perso1X, perso1Y + perso1Alt); // Quarto ponto
        glEnd();

        // Cor da bola
        glColor4ub(0, 255, 0, 255); // Azul

        // Desenho da bola
        drawCircle(bolaX + bolaRaio, bolaY + bolaRaio, bolaRaio, 20);

        // Desenho dos blocos
        glColor4ub(0, 0, 255, 255);
        for (int i = 0; i < BLOCOS; i++)
        {
            if(blocos[i].vivo == true)
            {
                glBegin(GL_QUADS);
                glVertex2f(blocos[i].blocoX, blocos[i].blocoY);
                glVertex2f(blocos[i].blocoX + blocos[i].blocoComp, blocos[i].blocoY);
                glVertex2f(blocos[i].blocoX + blocos[i].blocoComp, blocos[i].blocoY + blocos[i].blocoAlt);
                glVertex2f(blocos[i].blocoX, blocos[i].blocoY + blocos[i].blocoAlt);
                glEnd();
            }
        }

        SDL_GL_SwapWindow(screen); // Troca o buffer de tela
    }

    // Liberar recursos
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(screen);
    SDL_Quit();

    return 0;
}
