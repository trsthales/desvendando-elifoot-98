# 🛠️ Desvendando o Elifoot 98 (Parte 2): O Laboratório de Engenharia Reversa em 16-Bit no Linux

Na [Parte 1](https://www.tabnews.com.br/trsthales/desvendei-o-codigo-do-elifoot-98-apos-25-anos-engenharia-reversa-em-16-bit-com-ghidra-no-linux), mostramos as descobertas matemáticas sobre o motor do **Elifoot 98** (o exploit do 5-0-5, o bônus de +3 de nacionalidade e a assimetria do mando de campo).

Neste segundo artigo, vamos abrir os bastidores técnicos: **como montar do zero um laboratório de engenharia reversa no Debian 12 para depurar e descompilar executáveis legados de 16 bits** utilizando o **Ghidra**, o **Cheat Engine nativo para Linux** e o **WineVDM**.

---

## 🛑 1. O Desafio dos 16 Bits no Linux Moderno

Processadores e kernels modernos de 64 bits não executam binários legados de 16 bits (`Win16 NE`) nativamente. Se você tentar rodar diretamente com `wine ELIFOOT.EXE`, o Wine falha ao tentar linearizar endereços segmentados (`Failed to linearize address` e *Page Fault*).

Para resolver isso, usamos o **WineVDM (*otvdm*)**, um emulador de 16 bits que se integra ao Wine.

### Passo a Passo de Instalação do WineVDM:

```bash
cd ~/Downloads
# 1. Baixar o release estável do WineVDM
curl -sL "https://github.com/otya128/winevdm/releases/download/v0.8.0/otvdm-v0.8.1.zip" -o winevdm.zip
unzip -o winevdm.zip -d winevdm
cd winevdm/otvdm-v0.8.1

# 2. Registrar os ganchos (hooks) de 16 bits no registro do Wine
wine rundll32.exe setupapi.dll,InstallHinfSection DefaultInstall 128 ./install.inf
wine rundll32.exe setupapi.dll,InstallHinfSection DefaultInstall 128 ./installw.inf
```

### 💡 A Pegadinha Clássica do `eli.cod`:
Se ao abrir o jogo você se deparar com o erro **`Invalid file: eli.cod`**, a solução é simples: o `eli.cod` corrompido precisa ser apagado para que o jogo gere um arquivo limpo na inicialização:

```bash
rm -f ~/Downloads/winevdm/otvdm-v0.8.1/WINDOWS/eli.cod
rm -f ~/.wine/drive_c/windows/eli.cod
rm -f ~/Downloads/elifoot98/eli.cod
```

Agora, basta entrar na pasta do jogo e executar:
```bash
cd ~/Downloads/elifoot98 && wine ELIFOOT.EXE
```

---

## 🔍 2. Inspeção Dinâmica de Memória: Cheat Engine Nativo no Linux

Pouca gente sabe, mas o [Cheat Engine](https://www.cheatengine.org/) possui um **binário oficial compilado nativamente para Linux** (`cheatengine-x86_64`), eliminando a necessidade de rodar o CE dentro do Wine.

### Resolvendo Dependências e Permissões no Debian 12:

```bash
# 1. Instalar bibliotecas gráficas do Qt6
sudo apt update && sudo apt install -y libqt6printsupport6 qt6-base-dev

# 2. Liberar a restrição de leitura de memória do kernel (ptrace_scope)
echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope

# 3. Executar o Cheat Engine carregando as bibliotecas locais
cd ~/Downloads/CheatEngineLinux771
LD_LIBRARY_PATH=. ./cheatengine-x86_64
```

### Como escanear a memória do Elifoot 98:
1. Clique no ícone de processos do Cheat Engine e selecione **`winevdm.exe`** (o processo que hospeda o espaço de memória do jogo).
2. **Configuração Vital:** Altere o **Value Type** de `4 Bytes` para **`2 Bytes`** (ou `All`). Como sistemas de 16 bits usam inteiros de 2 bytes (`SmallInt`), buscar em 4 bytes fará você perder valores como Força e Contratos.

---

## 🧬 3. Engenharia Reversa Estática com o Ghidra

O Ghidra (desenvolvido pela NSA) é excelente para arquiteturas legadas x86 de 16 bits.

### 3.1. Importação e Opções de Análise
1. Crie um projeto no Ghidra e importe o `ELIFOOT.EXE` como formato **`New Executable (NE) Windows`**.
2. Na janela **Analysis Options**:
   * Mantenha as opções padrão;
   * 💡 **Dica de Ouro:** Marque **`[x] Decompiler Parameter ID`**. Em binários pequenos de 16 bits, essa opção faz o Ghidra deduzir automaticamente as convenções de chamada (`__stdcall16far`), argumentos de função e variáveis de pilha locais, deixando o código C muito mais limpo!

---

### 3.2. O Segredo das *Pascal Strings* do Delphi 1.0
O Elifoot 98 não foi compilado em C nem em Visual Basic, mas sim em **Borland Delphi 1.0 (Turbo Pascal)**. 

Em Pascal, as strings não terminam com `\0` (*null-terminated*); elas são do tipo `ShortString` (onde o **primeiro byte indica o tamanho da palavra**).

Para encontrar todos os textos do jogo:
1. Vá em **`Search` -> `For Strings...`**
2. Configure a janela:
   * **`[x] Pascal Strings`** (Marcado)
   * **`[ ] Require Null Termination`** (Desmarcado)
   * **`Minimum Length:`** `3` *(para não ignorar palavras curtas como "Gol" ou "Ano")*
3. Clique em **Search**.

---

### 3.3. A Descoberta da Tabela de Desfechos de Gol
Ao filtrar por **`gol`** na lista de Pascal Strings e dar um **duplo clique em `"GOLO"` (no endereço `8484:1785`)**, revelamos a matriz completa de desfechos de finalização do jogo no segmento `DS = 0x8484`:

* `8484:1785`: `"GOLO"`
* `8484:1791`: `"Defendeu"` (defesa do goleiro)
* `8484:179c`: `"Ao poste"` (bola na trave vertical)
* `8484:17a8`: `" barra"` (bola no travessão)
* `8484:17b2`: `"Ao lado"` (chute para fora na lateral)
* `8484:17bd`: `"Por cima"` (chute por cima do travessão)

---

### 3.4. Destravando o Descompilador (*Decompiler Process Died*)
Durante a análise, se o Ghidra tentar interpretar dados/textos como código executável (por exemplo, confundir os bytes `47 6F 6C 6F` de *"Golo"* com instruções `INC DI`, `OUTSW`, `INSB`), o descompilador C trava.

**Como destravar:**
1. Selecione as instruções falsas no painel central (**Listing**).
2. Pressione **`U`** (*Undefine*) ou **`C`** (*Clear Code Bytes*) para limpar os opcodes falsos.
3. Clique com o botão direito no byte de texto e selecione **`Data` -> `string`**.

---

## ⏱️ 4. O Momento Eureca: Encontrando o Motor da Partida (`FUN_3e3c_069b`)

Para localizar o laço que comanda o tempo de jogo nos 90 minutos:
1. No menu, vá em **`Search` -> `Scalar...`**
2. Busque pelo escalar **`90`** (ou `0x5A` em hexadecimal).
3. Entre os resultados, encontramos a instrução:
   ```assembly
   CMP word ptr ES:[DI + 0x18e], 0x5a
   ```
4. Ao dar duplo clique nessa instrução, a função **`FUN_3e3c_069b`** abriu no descompilador, revelando o relógio da partida em código C legível:

```c
// O relógio da partida avançando minuto a minuto:
*(int *)((int)puVar8 + 0x18e) = *(int *)((int)puVar8 + 0x18e) + 1;

// Checagem do tempo regulamentar (1 a 90 minutos):
if ((0 < minuto) && (minuto <= 90)) {
    // Sorteio de lances de ataque no minuto:
    cVar5 = FUN_17fb_a6eb(...);
}
```

---

## 🎯 Conclusão e Próximos Passos

Com este laboratório configurado, quebramos a barreira do binário de 16 bits e conectamos a depuração dinâmica da memória (Cheat Engine) diretamente ao descompilador estático em C (Ghidra).

Na **Parte 3**, vamos dissecar a matemática completa da função `FUN_17fb_a6eb`: as probabilidades exatas de cada minuto, a assimetria do mando de campo (`Random(6000)` vs `Random(7000)`) e a comprovação matemática de por que a tática **5-0-5** domina o jogo.

---

* Todo o código-fonte em C e a documentação do projeto estão abertos no GitHub:  
👉 **Repositório:** [https://github.com/trsthales/desvendando-elifoot-98](https://github.com/trsthales/desvendando-elifoot-98)*