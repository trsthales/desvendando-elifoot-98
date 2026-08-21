/**
 * player_evolution.c - Evolução/Declínio de Força e Roleta de Disciplina
 * Engenharia Reversa do Elifoot 98 (Funções: FUN_17fb_4a41 e FUN_17fb_100b)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_STRENGTH 50 // Offset 0x32
#define MIN_STRENGTH 1

// Níveis de Comportamento no Elifoot 98
typedef enum {
    COMP_MUITO_CORRETO = 1, // Peso 1 na roleta
    COMP_CORRETO       = 2, // Peso 2
    COMP_NORMAL        = 4, // Peso 4
    COMP_AGRESSIVO     = 7, // Peso 7
    COMP_MUITO_AGRESSIVO = 10 // Peso 10
} Comportamento;

typedef struct {
    char nome[30];
    int forca;             // 1 a 50
    int posicao;           // 0=Goleiro, 1=Defesa, 2=Médio, 3=Ataque
    Comportamento disciplina;
    int cartoesAmarelos;
    int cartoesVermelhos;
    int desempenhoPartida; // 1 = Positivo (Vitória/Destaque), -1 = Negativo (Desgaste/Derrota)
} Player;

int RandomRoll(int max) {
    if (max <= 0) return 0;
    return rand() % max;
}

/**
 * FUN_17fb_4a41 - Atualiza o ganho ou perda de força no pós-jogo
 * Comprovado: Goleiros têm trava de 50% extra para evoluir.
 */
void ProcessarEvolucaoForca(Player *jogador, int nivelBaseTime) {
    // ==========================================
    // CENÁRIO 1: GANHO DE FORÇA (+1)
    // ==========================================
    if (jogador->desempenhoPartida == 1) {
        // Goleiros (posicao 0) precisam vencer um sorteio extra de 50%
        if ((jogador->posicao != 0) || (RandomRoll(2) == 0)) {
            if (jogador->forca < MAX_STRENGTH) {
                jogador->forca++;

                // Trava de nivelamento: não sobe mais que +5 acima da base do clube
                if ((jogador->forca > nivelBaseTime + 5) || (jogador->forca > MAX_STRENGTH)) {
                    jogador->forca--; // Cancela incremento
                }
            }
        }
    }
    // ==========================================
    // CENÁRIO 2: PERDA DE FORÇA (-1)
    // ==========================================
    else if (jogador->desempenhoPartida == -1) {
        // 50% de tolerância antes de aplicar a perda de força
        if (RandomRoll(2) == 0 && jogador->forca > MIN_STRENGTH) {
            jogador->forca--;

            // Trava de nivelamento: não cai mais que -5 abaixo da base do clube
            if ((jogador->forca < nivelBaseTime - 5) || (jogador->forca < MIN_STRENGTH)) {
                jogador->forca++; // Cancela decremento
            }
        }
    }
}

/**
 * FUN_17fb_100b - Roleta Ponderada de Faltas e Cartões
 * Comprovado: Agressividade só serve para aumentar a fatia na roleta de infrações.
 */
int SortearInfratorFalta(Player *titulares[], int totalTitulares) {
    int pesosAcumulados[11];
    int pesoTotal = 0;

    // 1. Monta a roleta ponderada baseada no comportamento de cada atleta
    for (int i = 0; i < totalTitulares; i++) {
        pesoTotal += titulares[i]->disciplina;
        pesosAcumulados[i] = pesoTotal;
    }

    // 2. Sorteia o ponto de parada da roleta
    int sorteio = RandomRoll(pesoTotal);

    // 3. Identifica qual jogador foi sorteado
    for (int i = 0; i < totalTitulares; i++) {
        if (sorteio < pesosAcumulados[i]) {
            return i; // Índice do jogador que cometeu a falta / recebeu o cartão
        }
    }

    return 0;
}