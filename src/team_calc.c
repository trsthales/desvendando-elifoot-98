/**
 * team_calc.c - Cálculo de Força dos Setores e Bônus Pré-Jogo
 * Engenharia Reversa do Elifoot 98 (Função original: FUN_17fb_397d)
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define SECTOR_GOALKEEPER 0 // Offset 0x51
#define SECTOR_DEFENSE    1 // Offset 0x53
#define SECTOR_MIDFIELD   2 // Offset 0x55
#define SECTOR_ATTACK     3 // Offset 0x57

#define NATIONALITY_BONUS 3 // Bônus comprovado de mesma nacionalidade (+3)

typedef struct {
    char nome[30];
    int forca;             // Offset 0x1a (1 a 50)
    int posicao;           // Offset 0x19 (0=G, 1=D, 2=M, 3=A)
    char nacionalidade[4]; // Offset 0x1b (ex: "BRA", "ING", "POR")
    int condicaoFisica;
    int jogosContrato;
} PlayerData;

typedef struct {
    PlayerData *dadosJogador;
    int isTitular;         // Offset 0x10 (1 se estiver nos 11 titulares)
    int bonusPosicao;      // Offset 0x0a (Ajuste de alinhamento tático)
} PlayerSlot;

typedef struct {
    char nomeClube[30];
    char paisClube[4];     // Offset 0x4a (ex: "ING")
    int totalJogadores;
    PlayerSlot *elenco[30];
    int setorForca[4];     // Offsets 0x51, 0x53, 0x55, 0x57
} TeamStruct;

/**
 * FUN_17fb_397d - Calcula o poder de cada setor antes do pontapé inicial
 * Fórmula: Poder_Setor = SUM( (Força * 2) + (Mesma_Nacionalidade * 3) + BonusPosicao )
 */
void CalcularForcaSetores(TeamStruct *time) {
    // 1. Zera a força dos 4 setores (Goleiro, Defesa, Meio-Campo, Ataque)
    for (int i = 0; i <= 3; i++) {
        time->setorForca[i] = 0;
    }

    // 2. Itera sobre todos os atletas do clube
    for (int i = 0; i < time->totalJogadores; i++) {
        PlayerSlot *slot = time->elenco[i];

        // Processa apenas os 11 jogadores escalados como titulares
        if (slot != NULL && slot->isTitular == 1) {
            PlayerData *jogador = slot->dadosJogador;

            // Compara o País do Clube (0x4a) com a Nacionalidade do Atleta (0x1b)
            bool mesmoPais = (strncmp(time->paisClube, jogador->nacionalidade, 3) == 0);

            // Identifica o setor de destino (0 a 3)
            int setor = jogador->posicao;

            // Aplicação da fórmula matemática comprovada no binário:
            // - Força nominal entra dobrada (* 2)
            // - Atletas do mesmo país concedem +3 pontos fixos
            // - Soma do ajuste posicional (penalidade se fora de posição)
            time->setorForca[setor] += (jogador->forca * 2) + 
                                       (mesmoPais ? NATIONALITY_BONUS : 0) + 
                                       slot->bonusPosicao;
        }
    }
}