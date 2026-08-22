# 🗺️ Roadmap: Projeto Desvendando o Elifoot 98

Este documento organiza o plano de pesquisa, a publicação da série de artigos técnicos, o aprofundamento na engenharia reversa do executável de 16 bits e os próximos passos para a comunidade *open-source*.

---

## 📍 Status Atual do Projeto
* [x] Engenharia reversa dos módulos principais (`match_engine`, `team_calc`, `player_evolution`).
* [x] Criação e estruturação do repositório oficial no GitHub.
* [x] Publicação da Parte 1 no TabNews (Top 3 em Relevantes).
* [x] Publicação da Parte 1 no Reddit (`r/programacao` - Top 1 do dia com +14K views).
* [x] Pseudocódigo em C limpo disponibilizado na pasta `/src`.

---

## 📚 Fase 1: A Série de Artigos Técnicos (Multi-Part Series)

Plano de lançamento dos capítulos temáticos para o TabNews, Reddit (`r/programacao`, `r/brdev`, `r/gamesEcultura`, `r/linuxbrasil`), Dev.to e GitHub:

- [x] **Parte 1 — A Descoberta:** Visão geral da pesquisa, primeiras quebras de mitos (5-0-5, bônus de nacionalidade e fator casa).
- [ ] **Parte 2 — O Laboratório de Engenharia Reversa no Linux:**
  - Configurando o Debian 12 para lidar com executáveis Win16 NE (16 bits) via `winevdm`.
  - Configuração correta do Ghidra para Borland Delphi 1.0 (resolvendo o enigma de *Pascal Strings* e *DS = 0x8484*).
  - Depuração dinâmica com Cheat Engine no Linux (`ptrace_scope`, busca em 2 Bytes e breakpoints de hardware).
- [ ] **Parte 3 — A Física dos 90 Minutos (O Motor de Jogo):**
  - O laço temporal minuto a minuto (`FUN_3e3c_069b`).
  - A assimetria matemática do Mando de Campo (`Random(6000)` vs `Random(7000)` e `Random(90)` vs `Random(120)`).
  - O duelo tático e a tabela de desfechos (`GOLO`, `Defendeu`, `Ao poste`, `À barra`).
  - A prova matemática de por que o esquema **5-0-5** quebra o jogo.
- [ ] **Parte 4 — A Anatomia do Jogador (Memória e Structs):**
  - Análise do Hex Dump da struct do jogador (provando a ausência de atributos ocultos).
  - A confirmação do Bônus de Nacionalidade ($+3$ pontos por jogador local em `FUN_17fb_397d`).
  - O mito dos "Craques" com asterisco (`*`) e a Roleta Ponderada de Faltas (`FUN_17fb_100b`).
- [ ] **Parte 5 — A Fábrica de Craques (Evolução e Desgaste):**
  - Como a Moral da Equipe e as vitórias influenciam a evolução rodada a rodada (`FUN_17fb_4a41`).
  - A trava dos Goleiros (por que evoluem 2x mais devagar).
  - A âncora de nivelamento do clube ($\pm 5$ pontos).
- [ ] **Parte 6 — Contratos, Salários e Finanças (O Grand Finale):**
  - Como funciona o contador regressivo de partidas.
  - A prova de que pagar salários milionários **não altera** o rendimento do atleta em campo.
  - A microeconomia da bilheteria e o *sweet spot* do preço de ingressos.

---

## 🔬 Fase 2: Aprofundamento da Engenharia Reversa

Investigações complementares no binário `ELIFOOT.EXE`:

- [ ] **Mercado de Transferências & Leilão:** Mapear o algoritmo que a IA dos outros clubes usa para dar lances e precificar jogadores.
- [ ] **Sistema de Arbitragem:** Descobrir se os árbitros sorteados (ex: *Peter Mikkelsen*) possuem atributos ocultos de rigor/disciplina.
- [ ] **Regras de Demissão da Diretoria:** Identificar o teto de caixa negativo (bancarrota) e derrotas consecutivas para demissão.
- [ ] **Formato dos Arquivos de Save e Times (`.SAV` / `.EFT`):** Documentar o layout binário dos arquivos de times e saves para permitir editores modernos.

---

## 🎮 Fase 3: O Protótipo Nativo ("OpenElifoot" / AppImage)

Explorar o desenvolvimento de uma implementação moderna e aberta (*clean-room*):

- [ ] Criar um *wrapper* simples em C/CLI (terminal via `ncurses`) que consome `match_engine.c` e `team_calc.c`.
- [ ] Avaliar a criação de uma interface gráfica 2D leve (usando Raylib, SDL2 ou Godot).
- [ ] Configurar pipeline de CI/CD no GitHub Actions para compilar e gerar um **AppImage** nativo para Linux.

---

## 🤝 Fase 4: Comunidade e Tributo Histórico

- [ ] Entrar em contato com o autor original (**André Elias**) via canais oficiais para apresentar a pesquisa como um tributo de preservação histórica.
- [ ] Organizar e revisar as *Issues* e *Pull Requests* de desenvolvedores que queiram colaborar com a documentação ou com a base de código em C.

---

*Documento mantido por [@trsthales](https://github.com/trsthales) e comunidade.*