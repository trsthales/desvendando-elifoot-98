/**
 * match_engine.c - Motor de Simulação de Partidas e Cálculo de Gols
 * Engenharia Reversa do Elifoot 98 (Funções: FUN_3e3c_069b, FUN_17fb_a6eb, FUN_17fb_46df, FUN_17fb_81ed, FUN_17fb_c1bf)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Desfechos da Tabela de Finalização (Offset 8484:1785)
typedef enum {
    OUTCOME_GOAL = 0,      // "GOLO"
    OUTCOME_SAVED = 1,     // "Defendeu"
    OUTCOME_POST = 2,      // "Ao poste"
    OUTCOME_BAR = 3,       // " barra"
    OUTCOME_WIDE = 4       // "Para fora"
} ShotOutcome;

typedef struct {
    int forcaOfensivaCasa; // Offset 0x4c (Calculado em team_calc)
    int forcaOfensivaFora; // Offset 0x4e
    int forcaDefesaCasa;
    int forcaDefesaFora;
    int rematesCasa;       // Offset 0x4f
    int rematesFora;
    int golsCasa;          // Offset 0x59
    int golsFora;
    int minutoAtual;       // Offset 0x18e (1 a 90)
} MatchState;

// Delphi 1.0 System.Random(N) - Retorna inteiro entre 0 e N-1
int Random(int max) {
    if (max <= 0) return 0;
    return rand() % max;
}

/**
 * FUN_17fb_81ed - Duelo Direto entre Atacante e Defensor/Goleiro
 * Comprovado: O comportamento NÃO entra no cálculo do desarme.
 */
ShotOutcome ResolverDueloFinalizacao(int forcaAtacante, int forcaGoleiro) {
    int rollAtacante = Random(forcaAtacante);
    int rollDefensor = Random(forcaGoleiro);

    // 25% de chance de evento especial / desvio (Random(4) == 0)
    if (Random(4) == 0) {
        if (rollAtacante < 4) return OUTCOME_POST; // Trave vertical
        if (rollAtacante < 10) return OUTCOME_BAR; // Travessão
        return OUTCOME_WIDE;
    }

    // Duelo padrão: quem tirar o maior valor vence o lance
    if (rollAtacante > rollDefensor) {
        return OUTCOME_GOAL;   // GOLO!
    } else {
        return OUTCOME_SAVED;  // Goleiro defendeu
    }
}

/**
 * FUN_17fb_46df - Gerador Secundário de Chances (Anti-Goleada e Sorte Pura)
 */
bool ChecarChanceSecundaria(bool isCasa, int remates, int gols, int minuto) {
    int divisor = isCasa ? 90 : 120;
    int luckyDivisor = isCasa ? 180 : 270;
    int limiar = 150 - (remates + (gols * 3)); // Mecanismo redutor anti-snowball

    int roll = Random(divisor);
    if ((roll < limiar) || (Random(luckyDivisor) == 0)) {
        if (Random(45) < (minuto % 45)) {
            return true;
        }
    }
    return false;
}

/**
 * FUN_17fb_a6eb & FUN_3e3c_069b - Loop de Simulação por Minuto de Jogo
 */
void SimularMinutoPartida(MatchState *partida) {
    partida->minutoAtual++;

    // -------------------------------------------------------------
    // 1. CHECAGEM DE PÊNALTI (FUN_17fb_aa11 - 1% de chance por minuto)
    // -------------------------------------------------------------
    int rollPenalti = Random(1000);
    if (rollPenalti < 10) {
        bool penaltiCasa = (Random(2) == 0);
        if (penaltiCasa) {
            if (ResolverDueloFinalizacao(45, partida->forcaDefesaFora / 4) == OUTCOME_GOAL) {
                partida->golsCasa++;
            }
            partida->rematesCasa++;
            return;
        } else {
            if (ResolverDueloFinalizacao(45, partida->forcaDefesaCasa / 4) == OUTCOME_GOAL) {
                partida->golsFora++;
            }
            partida->rematesFora++;
            return;
        }
    }

    // -------------------------------------------------------------
    // 2. GERADOR DE CHANCES DO TIME DA CASA (Divisor 6000)
    // -------------------------------------------------------------
    bool chanceCasa = (Random(6000) < partida->forcaOfensivaCasa) ||
                      ChecarChanceSecundaria(true, partida->rematesCasa, partida->golsCasa, partida->minutoAtual);

    if (chanceCasa) {
        partida->rematesCasa++;
        ShotOutcome desfecho = ResolverDueloFinalizacao(partida->forcaOfensivaCasa / 3, partida->forcaDefesaFora / 3);
        if (desfecho == OUTCOME_GOAL) {
            partida->golsCasa++;
        }
    }

    // -------------------------------------------------------------
    // 3. GERADOR DE CHANCES DO TIME DE FORA (Divisor 7000 - Penalidade Visitante)
    // -------------------------------------------------------------
    bool chanceFora = (Random(7000) < partida->forcaOfensivaFora) ||
                      ChecarChanceSecundaria(false, partida->rematesFora, partida->golsFora, partida->minutoAtual);

    if (chanceFora) {
        partida->rematesFora++;
        ShotOutcome desfecho = ResolverDueloFinalizacao(partida->forcaOfensivaFora / 3, partida->forcaDefesaCasa / 3);
        if (desfecho == OUTCOME_GOAL) {
            partida->golsFora++;
        }
    }
}