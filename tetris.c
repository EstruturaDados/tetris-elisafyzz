#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- Definições de Capacidade ---
#define CAPACIDADE_FILA 5   // Tamanho fixo da fila de peças futuras.
#define CAPACIDADE_PILHA 3  // Tamanho máximo da pilha de peças reservadas.

// --- 1. Estrutura de Dados da Peça ---

// Cada peça possui um tipo (nome) e um identificador único (id).
typedef struct {
    char nome;   
    int id;      
} Peca;

// --- 2. Estrutura de Dados da Fila Circular (Queue) ---

typedef struct {
    Peca pecas[CAPACIDADE_FILA];
    int frente;     // Índice da próxima peça a ser removida (dequeue).
    int tras;       // Índice para a próxima inserção (enqueue).
    int contador;   // Número atual de elementos válidos na fila.
} FilaPecas;

// --- 3. Estrutura de Dados da Pilha Linear (Stack) ---

typedef struct {
    Peca pecas[CAPACIDADE_PILHA];
    int topo;       // Índice do último elemento inserido (-1 para pilha vazia).
} PilhaPecas;


// --- Variáveis Globais de Controle ---

FilaPecas fila; 
PilhaPecas pilha;
int proximo_id = 0; // Gerador de IDs únicos.


// -----------------------------------------------------------------------------
//                                 FUNÇÕES UTilitÁRIAS
// -----------------------------------------------------------------------------

/**
 * @brief Gera uma nova peça do Tetris com um tipo aleatório e um id único.
 * @return Peca A nova peça gerada.
 */
Peca gerarPeca() {
    // Tipos de peças comuns no Tetris.
    char tipos[] = {'I', 'O', 'T', 'L', 'J', 'S', 'Z'};
    int num_tipos = sizeof(tipos) / sizeof(tipos[0]);
    
    int indice_aleatorio = rand() % num_tipos; 
    
    Peca novaPeca;
    novaPeca.nome = tipos[indice_aleatorio];
    novaPeca.id = proximo_id;
    
    proximo_id++; // Garante a unicidade do próximo ID.
    
    return novaPeca;
}

// -----------------------------------------------------------------------------
//                            FUNÇÕES DE FILA E PILHA (BÁSICAS)
// -----------------------------------------------------------------------------

// As funções enqueue, dequeue, push e pop são reusadas do código anterior.

/**
 * @brief Insere uma peça no final da fila (enqueue).
 */
void enqueue(Peca novaPeca) {
    fila.pecas[fila.tras] = novaPeca;
    fila.tras = (fila.tras + 1) % CAPACIDADE_FILA;
    if (fila.contador < CAPACIDADE_FILA) {
        fila.contador++;
    }
}

/**
 * @brief Remove uma peça da frente da fila (dequeue).
 * @return int 1 se sucesso, 0 se fila vazia.
 */
int dequeue(Peca *pecaRemovida) {
    if (fila.contador == 0) {
        return 0;
    }

    *pecaRemovida = fila.pecas[fila.frente];
    fila.frente = (fila.frente + 1) % CAPACIDADE_FILA;
    fila.contador--;
    
    return 1;
}

/**
 * @brief Adiciona uma peça ao topo da pilha (push).
 * @return int 1 se sucesso, 0 se pilha cheia.
 */
int push(Peca novaPeca) {
    if (pilha.topo == CAPACIDADE_PILHA - 1) {
        return 0; 
    }

    pilha.topo++; 
    pilha.pecas[pilha.topo] = novaPeca;
    
    return 1;
}

/**
 * @brief Remove uma peça do topo da pilha (pop).
 * @return int 1 se sucesso, 0 se pilha vazia.
 */
int pop(Peca *pecaRemovida) {
    if (pilha.topo == -1) {
        return 0; 
    }

    *pecaRemovida = pilha.pecas[pilha.topo];
    pilha.topo--; 
    
    return 1;
}

// -----------------------------------------------------------------------------
//                              FUNÇÕES DE AÇÕES ESTRATÉGICAS
// -----------------------------------------------------------------------------

/**
 * @brief Ação 4: Troca a peça da frente da fila com o topo da pilha.
 * * É a única ação que NÃO gera uma nova peça, pois é uma troca.
 */
void acaoTrocarPecaAtual() {
    printf("\n--- ACAO 4: TROCAR PEÇA ATUAL ---\n");
    
    // 1. Verifica se ambas as estruturas têm pelo menos 1 peça.
    if (fila.contador == 0) {
        printf("❌ ERRO: Fila de peças futuras VAZIA! Não é possível trocar.\n");
        return;
    }
    if (pilha.topo == -1) {
        printf("❌ ERRO: Pilha de reserva VAZIA! Não é possível trocar.\n");
        return;
    }

    // 2. Realiza a troca: A peça do topo da pilha vai para a frente da fila, e vice-versa.
    Peca pecaFila = fila.pecas[fila.frente];
    Peca pecaPilha = pilha.pecas[pilha.topo];
    
    // Troca
    fila.pecas[fila.frente] = pecaPilha;
    pilha.pecas[pilha.topo] = pecaFila;
    
    printf("✅ TROCA SIMPLES BEM-SUCEDIDA! \n");
    printf("  -> Peça da Fila [%c %d] foi para o Topo da Pilha.\n", pecaFila.nome, pecaFila.id);
    printf("  -> Peça da Pilha [%c %d] foi para a Frente da Fila.\n", pecaPilha.nome, pecaPilha.id);
}

/**
 * @brief Ação 5: Alterna as 3 primeiras peças da fila com as 3 peças da pilha.
 * * Requer que ambas as estruturas tenham capacidade mínima de 3 peças ocupadas.
 */
void acaoTrocaMultipla() {
    printf("\n--- ACAO 5: TROCA MÚLTIPLA (Em Bloco) ---\n");

    // 1. Verifica a condição de capacidade
    if (fila.contador < CAPACIDADE_PILHA) { // CAPACIDADE_PILHA é 3.
        printf("❌ ERRO: Fila precisa de pelo menos 3 peças para a Troca Múltipla. (Contador atual: %d)\n", fila.contador);
        return;
    }
    if (pilha.topo < CAPACIDADE_PILHA - 1) { // topo + 1 = 3
        printf("❌ ERRO: Pilha precisa estar CHEIA (3 peças) para a Troca Múltipla. (Contador atual: %d)\n", pilha.topo + 1);
        return;
    }

    // 2. Realiza a troca iterando sobre os 3 primeiros elementos da Fila e Pilha.
    // A Fila itera a partir da 'frente' de forma circular.
    // A Pilha itera do índice 0 até o 2.
    
    for (int i = 0; i < CAPACIDADE_PILHA; i++) {
        // Calcula o índice circular na Fila para os 3 primeiros elementos
        int indiceFila = (fila.frente + i) % CAPACIDADE_FILA;
        int indicePilha = i; // Posições 0, 1, 2 da pilha (Base -> Topo)
        
        // Troca o conteúdo (swap)
        Peca temp = fila.pecas[indiceFila];
        fila.pecas[indiceFila] = pilha.pecas[indicePilha];
        pilha.pecas[indicePilha] = temp;
    }
    
    printf("✅ TROCA MÚLTIPLA BEM-SUCEDIDA! As 3 primeiras peças da Fila e as 3 peças da Pilha foram alternadas.\n");
}


// -----------------------------------------------------------------------------
//                                AÇÕES BÁSICAS DO JOGO
// -----------------------------------------------------------------------------

/**
 * @brief Ação 1: Simula o jogador "jogando" a peça da frente da fila.
 */
void acaoJogarPeca() {
    Peca jogada;
    printf("\n--- ACAO 1: JOGAR PEÇA ---\n");
    
    if (dequeue(&jogada)) {
        printf("✅ PEÇA JOGADA (removida da Fila): [%c %d].\n", jogada.nome, jogada.id);
        
        // REQUISITO: Reabastece a fila com uma nova peça gerada.
        Peca nova = gerarPeca(); 
        enqueue(nova);
        printf("➕ Nova peca gerada e adicionada ao final da Fila: [%c %d].\n", nova.nome, nova.id);
    } else {
        printf("❌ ERRO: Fila de peças futuras está VAZIA! Não há peças para jogar.\n");
    }
}

/**
 * @brief Ação 2: Move a peça da frente da fila para o topo da pilha de reserva.
 */
void acaoReservarPeca() {
    Peca reservada;
    printf("\n--- ACAO 2: RESERVAR PEÇA ---\n");

    // 1. Tenta remover da Fila
    if (!dequeue(&reservada)) {
        printf("❌ ERRO: Fila de peças futuras está VAZIA! Não há peça para reservar.\n");
        return;
    }

    // 2. Tenta adicionar na Pilha
    if (push(reservada)) {
        printf("✅ PEÇA RESERVADA: Peça [%c %d] movida da Fila para o Topo da Pilha.\n", reservada.nome, reservada.id);
        
        // 3. Reabastece a fila
        Peca nova = gerarPeca(); 
        enqueue(nova);
        printf("➕ Nova peca gerada e adicionada ao final da Fila: [%c %d].\n", nova.nome, nova.id);
    } else {
        // Se a Pilha estiver Cheia, a peça removida da Fila é descartada (simplificação do requisito).
        printf("❌ ERRO: Pilha de reserva está CHEIA (Capacidade: %d). A peça [%c %d] foi descartada.\n", CAPACIDADE_PILHA, reservada.nome, reservada.id);

        // 3. Reabastece a fila, mesmo com a falha na reserva.
        Peca nova = gerarPeca(); 
        enqueue(nova);
        printf("➕ Nova peca gerada e adicionada ao final da Fila: [%c %d].\n", nova.nome, nova.id);
    }
}

/**
 * @brief Ação 3: Simula o jogador "usando" a peça do topo da pilha de reserva.
 */
void acaoUsarPecaReservada() {
    Peca usada;
    printf("\n--- ACAO 3: USAR PEÇA RESERVADA ---\n");

    // 1. Tenta remover da Pilha (Pop)
    if (pop(&usada)) {
        printf("✅ PEÇA USADA (removida da Pilha): [%c %d].\n", usada.nome, usada.id);
        
        // 2. Reabastece a fila (requisito)
        Peca nova = gerarPeca(); 
        enqueue(nova);
        printf("➕ Nova peca gerada e adicionada ao final da Fila: [%c %d].\n", nova.nome, nova.id);
    } else {
        printf("❌ ERRO: Pilha de reserva está VAZIA! Não há peças para usar.\n");
    }
}

// -----------------------------------------------------------------------------
//                            INICIALIZAÇÃO E EXIBIÇÃO
// -----------------------------------------------------------------------------

/**
 * @brief Inicializa a fila e a pilha.
 * * Preenche a fila com 5 peças iniciais.
 */
void inicializarEstruturas() {
    // 1. Inicializa Fila
    fila.frente = 0;
    fila.tras = 0;
    fila.contador = 0;
    proximo_id = 0;

    // 2. Inicializa Pilha
    pilha.topo = -1; 

    // 3. Preenche a fila inicial com CAPACIDADE_FILA peças (requisito)
    for (int i = 0; i < CAPACIDADE_FILA; i++) {
        enqueue(gerarPeca());
    }
}

/**
 * @brief Exibe o estado atual da Fila de Pecas.
 */
void exibirFila() {
    printf("Fila de Pecas Futuras (Tamanho: %d/%d): ", fila.contador, CAPACIDADE_FILA);

    if (fila.contador == 0) {
        printf("VAZIA.\n");
        return;
    }
    
    // Percorre os elementos da fila de forma circular, da Frente para o Fim.
    for (int i = 0; i < fila.contador; i++) {
        int indice = (fila.frente + i) % CAPACIDADE_FILA;
        Peca p = fila.pecas[indice];
        
        printf("[%c %d]", p.nome, p.id);
        
        if (i < fila.contador - 1) {
            printf(" -> ");
        }
    }
    printf(" (FRENTE)\n");
}

/**
 * @brief Exibe o estado atual da Pilha de Reserva.
 */
void exibirPilha() {
    printf("Pilha de Reserva (Tamanho: %d/%d): ", pilha.topo + 1, CAPACIDADE_PILHA);

    if (pilha.topo == -1) {
        printf("VAZIA.\n");
        return;
    }
    
    printf("(TOPO) ");
    // Percorre do topo (pilha.topo) até a base (índice 0).
    for (int i = pilha.topo; i >= 0; i--) {
        Peca p = pilha.pecas[i];
        
        printf("[%c %d]", p.nome, p.id);
        
        if (i > 0) {
            printf(" -> ");
        }
    }
    printf(" (BASE)\n");
}

/**
 * @brief Exibe o estado completo da fila e da pilha.
 */
void exibirEstado() {
    printf("\n====================================================================\n");
    printf("              ESTADO ATUAL DO GERENCIADOR DE PECAS\n");
    printf("====================================================================\n");
    exibirFila();
    exibirPilha();
    printf("====================================================================\n");
}

// -----------------------------------------------------------------------------
//                                FUNÇÃO PRINCIPAL
// -----------------------------------------------------------------------------

int main() {
    // Inicializa o gerador de números aleatórios.
    srand(time(NULL)); 
    
    // Inicializa as estruturas de dados.
    inicializarEstruturas();
    
    printf("Sistema de Gerenciamento de Pecas do Tetris Stack Inicializado!\n");
    exibirEstado();

    int opcao;
    
    do {
        printf("\n## Opcoes de Acao Estrategica ##\n");
        printf("------------------------------------------------------------------\n");
        printf("Codigo | Acao\n");
        printf("------------------------------------------------------------------\n");
        printf("  1    | Jogar peca (Dequeue da Fila)\n");
        printf("  2    | Reservar peca (Fila -> Pilha)\n");
        printf("  3    | Usar peca reservada (Pop da Pilha)\n");
        printf("  4    | TROCAR peca da frente da Fila com o topo da Pilha\n");
        printf("  5    | TROCA MÚLTIPLA: 3 primeiros da Fila por 3 da Pilha\n");
        printf("  0    | Sair do programa\n");
        printf("------------------------------------------------------------------\n");
        printf("Digite o codigo da acao desejada: ");
        
        if (scanf("%d", &opcao) != 1) {
            printf("\nEntrada inválida. Digite um número.\n");
            while(getchar() != '\n');
            opcao = -1;
            continue;
        }

        switch (opcao) {
            case 1:
                acaoJogarPeca();
                break;
            case 2:
                acaoReservarPeca();
                break;
            case 3:
                acaoUsarPecaReservada();
                break;
            case 4:
                acaoTrocarPecaAtual();
                break;
            case 5:
                acaoTrocaMultipla();
                break;
            case 0:
                printf("\nSaindo do simulador. Até logo!\n");
                break;
            default:
                printf("\nCódigo de ação inválido. Escolha uma opção entre 0 e 5.\n");
                break;
        }
        
        // Exibe o estado da fila e pilha após CADA acao (requisito).
        if (opcao != 0) {
            exibirEstado();
        }

    } while (opcao != 0);

    return 0;
}
