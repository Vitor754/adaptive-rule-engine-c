//#################### CABECALHO ##############################
// NOME>: VITOR MANOEL ALVES DE SOUSA
// DISICIPLINA>: CES-11
// TURMA>: 4
// ID DO LAB>: LAB 08

//#################### BIBLIOTECAS E IMPORTS ##################
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

//#################### ESTRUTURAS E VARIAVEIS GLOBAIS ###################

struct rede{
    int Z; // Numero atual de roteadores (ou maior ID atingido)
    int K; // Banda de referencia
    int **matriz_bandwidth; // Matriz de Adjacencia (Largura de Banda)
    int **matriz_custo;     // Matriz de Custos (Calculada para OSPF)
};
typedef struct rede TipoRede;

// Globais para armazenar os resultados do DFS (Balanceamento de Carga)
int caminhos_encontrados[50][100]; // Armazena ate 50 caminhos possiveis
int saltos_encontrados[50];        // Armazena o tamanho (hops) de cada caminho
int qtd_caminhos = 0;              // Contador de caminhos encontrados

//#################### FUNCOES AUXILIARES OSPF ################

// Calcula o custo de um link baseado na formula OSPF: C = K / Banda
// Regra: Truncar para inteiro e minimo = 1.
void calcular_custo_link(TipoRede *rede, int i, int j) {
    if (i == j) {
        rede->matriz_custo[i][j] = 0;
        return;
    }
    int banda = rede->matriz_bandwidth[i][j];
    // Se banda for 0 ou -1 (deletado), nao ha conexao
    if (banda <= 0) {
        rede->matriz_custo[i][j] = INT_MAX;
        return;
    }

    double custo_calculado = (double)rede->K / (double)banda;
    int custo_inteiro = (int)custo_calculado;

    // Custo minimo deve ser 1
    if (custo_inteiro < 1) {
        rede->matriz_custo[i][j] = 1;
    } else {
        rede->matriz_custo[i][j] = custo_inteiro;
    }
}

// Percorre a matriz inteira recalculando custos (usado na inicializacao)
void preencher_matriz_custo_inicial(TipoRede *rede) {
    for (int i = 0; i < rede->Z; i++) {
        for (int j = 0; j < rede->Z; j++) {
            calcular_custo_link(rede, i, j);
        }
    }
}

//#################### ALGORITMOS DE BUSCA ####################

// Algoritmo de Dijkstra: Encontra o MENOR CUSTO total da origem ate todos os nos.
// Retorna o array 'dist' preenchido.
void dijkstra(TipoRede *rede, int idx_origem, int *dist, int *pred, int *visitado) {
    int Z = rede->Z;
    // Inicializacao
    for (int i = 0; i < Z; i++) {
        dist[i] = INT_MAX;
        pred[i] = -1;
        visitado[i] = 0;
    }
    dist[idx_origem] = 0;

    // Loop principal (relaxamento)
    for (int count = 0; count < Z - 1; count++) {
        int min_dist = INT_MAX;
        int u = -1;

        // Escolhe o no nao visitado com menor distancia
        for (int i = 0; i < Z; i++) {
            if (visitado[i] == 0 && dist[i] <= min_dist) {
                min_dist = dist[i];
                u = i;
            }
        }

        if (u == -1) break; // Grafo desconexo ou fim da busca
        visitado[u] = 1;

        // Atualiza vizinhos
        for (int v = 0; v < Z; v++) {
            if (visitado[v] == 0 && rede->matriz_custo[u][v] != INT_MAX) {
                int custo_link = rede->matriz_custo[u][v];
                int novo_custo = dist[u] + custo_link;

                if (novo_custo < dist[v]) {
                    dist[v] = novo_custo;
                    pred[v] = u;
                }
            }
        }
    }
}

// Busca em Profundidade (DFS): Encontra TODOS os caminhos que satisfazem o custo alvo.
void dfs_busca_caminhos(TipoRede *rede, int u, int idx_destino,
                        int custo_alvo, int custo_atual,
                        int *caminho_atual, int saltos_atual, int *visitado) {

    // Se custo estourou, volta.
    if (custo_atual > custo_alvo) return;

    //Chegou ao destino com custo exato
    if (u == idx_destino) {
        if (custo_atual == custo_alvo) {
            for(int i=0; i < saltos_atual; i++){
                caminhos_encontrados[qtd_caminhos][i] = caminho_atual[i];
            }
            saltos_encontrados[qtd_caminhos] = saltos_atual;
            qtd_caminhos++;
        }
        return;
    }
    visitado[u] = 1;
    for (int v = 0; v < rede->Z; v++) {
        int custo_link = rede->matriz_custo[u][v];

        if (custo_link != INT_MAX && !visitado[v]) {
            // Adiciona no caminho (apenas nos intermediarios)
            if (v != idx_destino) {
                caminho_atual[saltos_atual] = v;
            }

            int prox_saltos = (v != idx_destino) ? saltos_atual + 1 : saltos_atual;

            dfs_busca_caminhos(rede, v, idx_destino, custo_alvo,
                               custo_atual + custo_link,
                               caminho_atual, prox_saltos, visitado);
        }
    }
    visitado[u] = 0; // Libera o no para outros caminhos
}

// Ordena os caminhos encontrados (Bubble Sort)
// Criterio 1: Menor numero de saltos.
// Criterio 2: Ordem lexicografica dos IDs.
void ordenar_caminhos() {
    for (int i = 0; i < qtd_caminhos - 1; i++) {
        for (int j = 0; j < qtd_caminhos - i - 1; j++) {
            int trocar = 0;

            if (saltos_encontrados[j] > saltos_encontrados[j+1]) {
                trocar = 1;
            }
            else if (saltos_encontrados[j] == saltos_encontrados[j+1]) {
                // Desempate por IDs
                for (int k = 0; k < saltos_encontrados[j]; k++) {
                    if (caminhos_encontrados[j][k] > caminhos_encontrados[j+1][k]) {
                        trocar = 1;
                        break;
                    }
                    if (caminhos_encontrados[j][k] < caminhos_encontrados[j+1][k]) {
                        break;
                    }
                }
            }

            if (trocar) {
                // Troca saltos
                int temp_s = saltos_encontrados[j];
                saltos_encontrados[j] = saltos_encontrados[j+1];
                saltos_encontrados[j+1] = temp_s;
                // Troca caminhos (usando memcpy para arrays)
                int temp_c[100];
                memcpy(temp_c, caminhos_encontrados[j], 100*sizeof(int));
                memcpy(caminhos_encontrados[j], caminhos_encontrados[j+1], 100*sizeof(int));
                memcpy(caminhos_encontrados[j+1], temp_c, 100*sizeof(int));
            }
        }
    }
}

//#################### GERENCIAMENTO DE REDE ##################

// Adiciona um roteador aumentando a matriz dinamicamente (realloc)
void router_add(TipoRede *rede) {
    int Z_aux = rede->Z + 1; // Novo tamanho

    // Realoca vetor de ponteiros
    int **nova_matriz_banda = (int**) realloc(rede->matriz_bandwidth, Z_aux * sizeof(int*));
    int **nova_matriz_custo = (int**) realloc(rede->matriz_custo, Z_aux * sizeof(int*));

    rede->matriz_bandwidth = nova_matriz_banda;
    rede->matriz_custo = nova_matriz_custo;

    // Realoca linhas existentes
    for (int i = 0; i < rede->Z; i++) {
        rede->matriz_bandwidth[i] = (int*) realloc(rede->matriz_bandwidth[i], Z_aux * sizeof(int));
        rede->matriz_custo[i] = (int*) realloc(rede->matriz_custo[i], Z_aux * sizeof(int));
    }

    // Aloca a nova linha
    rede->matriz_bandwidth[rede->Z] = (int*) malloc(Z_aux * sizeof(int));
    rede->matriz_custo[rede->Z]     = (int*) malloc(Z_aux * sizeof(int));

    // Inicializa a nova linha e coluna
    for (int i = 0; i < rede->Z; i++) {
        rede->matriz_bandwidth[i][rede->Z] = 0;
        rede->matriz_custo[i][rede->Z]     = INT_MAX;
        rede->matriz_bandwidth[rede->Z][i] = 0;
        rede->matriz_custo[rede->Z][i]     = INT_MAX;
    }

    // Inicializa a diagonal do novo roteador
    rede->matriz_bandwidth[rede->Z][rede->Z] = 0;
    rede->matriz_custo[rede->Z][rede->Z]     = 0;

    rede->Z = Z_aux;
    printf("Roteador %d adicionado\n", Z_aux);
}

// Remove roteador usando Delecao Logica (marca diagonal com -1)
void router_del(TipoRede *rede, int id) {
    // 1. Validação (Mesma de antes)
    if (id < 1 || id > rede->Z || rede->matriz_bandwidth[id-1][id-1] == -1) {
        printf("Roteador %d nao existe\n", id);
        return;
    }

    int idx = id - 1;

    // Zerar conexões (Mesmo de antes)
    for (int i = 0; i < rede->Z; i++) {
        rede->matriz_bandwidth[idx][i] = 0;
        rede->matriz_bandwidth[i][idx] = 0;
        rede->matriz_custo[idx][i] = INT_MAX;
        rede->matriz_custo[i][idx] = INT_MAX;
    }

    // Marcação Lógica (Mesmo de antes)
    rede->matriz_bandwidth[idx][idx] = -1;

    printf("Roteador %d removido\n", id);

    // Se deletamos o maior ID, devemos reduzir o tamanho da rede (Z)
    if (id == rede->Z) {
        rede->Z--;

        while (rede->Z > 0 && rede->matriz_bandwidth[rede->Z - 1][rede->Z - 1] == -1) {
             rede->Z--;
        }
    }
}

// Adiciona link respeitando LACP (apenas links de mesma velocidade agregam)
void add_link(TipoRede *rede, int id1, int id2, int banda){
    int idx1 = id1 - 1;
    int idx2 = id2 - 1;
    int banda_atual = rede->matriz_bandwidth[idx1][idx2];
    int base = 0;

    if(banda_atual == 0){
        // Novo link
       rede->matriz_bandwidth[idx1][idx2] += banda;
       rede->matriz_bandwidth[idx2][idx1] += banda;
       calcular_custo_link(rede, idx1, idx2);
       calcular_custo_link(rede, idx2, idx1);
       printf("Link entre os roteadores %d e %d adicionado\n", id1, id2);
    }
    else{
        // LACP: Verifica a base do link existente
        if(banda_atual % 1000 == 0) base = 1000;
        else if(banda_atual % 100 == 0) base = 100;
        else if(banda_atual % 10 == 0) base = 10;

        if(banda == base){
           rede->matriz_bandwidth[idx1][idx2] += banda;
           rede->matriz_bandwidth[idx2][idx1] += banda;
           calcular_custo_link(rede, idx1, idx2);
           calcular_custo_link(rede, idx2, idx1);
           printf("Link entre os roteadores %d e %d adicionado\n", id1, id2);
        }
        else{
            printf("Largura de banda nao apropriada\n");
        }
    }
}

// Remove link e reseta custo para Infinito
void del_link(TipoRede *rede, int id1, int id2){
    int idx1 = id1 - 1;
    int idx2 = id2 - 1;

    if(rede->matriz_bandwidth[idx1][idx2] == 0){
        printf("Link nao existe\n"); // Adicionado \n para seguranca
        return;
    }
    rede->matriz_bandwidth[idx1][idx2] = 0;
    rede->matriz_bandwidth[idx2][idx1] = 0;
    calcular_custo_link(rede, idx1, idx2);
    calcular_custo_link(rede, idx2, idx1);
    printf("Link entre os roteadores %d e %d removido\n", id1, id2);
}

// Funcao principal de rastreamento (Pipeline: Dijkstra -> DFS -> Sort -> Print)
void trace_rota(TipoRede *rede, int id_origem, int id_destino) {
    if (id_origem == id_destino) {
        printf("Comunicacao direta\n");
        return;
    }

    int idx_origem = id_origem - 1;
    int idx_destino = id_destino - 1;
    int Z = rede->Z;

    // Auxiliares
    int *dist = (int*) malloc(Z * sizeof(int));
    int *pred = (int*) malloc(Z * sizeof(int));
    int *visitado = (int*) malloc(Z * sizeof(int));
    int *caminho_temp = (int*) malloc(Z * sizeof(int));

    // Fase 1: Descobrir o custo minimo
    dijkstra(rede, idx_origem, dist, pred, visitado);
    int custo_minimo = dist[idx_destino];

    if (custo_minimo != INT_MAX) {
        // Fase 2: Descobrir todos os caminhos com esse custo
        qtd_caminhos = 0;
        for(int i=0; i<Z; i++) visitado[i] = 0;

        dfs_busca_caminhos(rede, idx_origem, idx_destino,
                           custo_minimo, 0, caminho_temp, 0, visitado);

        // Fase 3: Ordenar e Imprimir
        ordenar_caminhos();

        for (int i = 0; i < qtd_caminhos; i++) {

            // Caso 1: Vizinhos (0 saltos intermediários)
            if (saltos_encontrados[i] == 0) {
                printf("Comunicacao direta\n");
            }
            // Caso 2: Rota Indireta (Imprime os intermediários)
            else {
                for (int j = 0; j < saltos_encontrados[i]; j++) {
                    printf("%d", caminhos_encontrados[i][j] + 1); // +1 para ID
                    if (j < saltos_encontrados[i] - 1) printf(" ");
                }
                printf("\n");
            }
        }
    }

    free(dist); free(pred); free(visitado); free(caminho_temp);
}

// Libera memoria das matrizes
void liberar_matriz(int **matriz, int Z) {
    if (matriz == NULL) return;
    for (int i = 0; i < Z; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

// Destroi o struct da rede
void liberar_rede(TipoRede *rede) {
    if (rede == NULL) return;
    liberar_matriz(rede->matriz_bandwidth, rede->Z);
    liberar_matriz(rede->matriz_custo, rede->Z);
    free(rede);
}

//################# MAIN ######################################

int main(void) {
    //VARIAVEIS DE APOIO
    TipoRede *roteadores;
    char comando[20];
    char comando_aux[10];
    roteadores = (TipoRede*) malloc(sizeof(TipoRede));
    scanf("%d", &roteadores->Z);
    roteadores->matriz_bandwidth = (int**) malloc(roteadores->Z * sizeof(int*));
    roteadores->matriz_custo = (int**) malloc(roteadores->Z * sizeof(int*));

    for (int i = 0; i < roteadores->Z; i++) {
        roteadores->matriz_bandwidth[i] = (int*) malloc(roteadores->Z * sizeof(int));
        roteadores->matriz_custo[i]     = (int*) malloc(roteadores->Z * sizeof(int));
    }
    //LEITURA DE INPUTS
    // Leitura da matriz
    for (int i = 0; i < roteadores->Z; i++) {
        for (int j = 0; j < roteadores->Z; j++) {
            scanf("%d", &roteadores->matriz_bandwidth[i][j]);
        }
    }

    // Leitura K e processamento inicial
    scanf("%d", &roteadores->K);
    preencher_matriz_custo_inicial(roteadores);

    // Loop de comandos
    strcpy(comando, "");
    while(strcmp(comando, "EXIT") != 0){
        scanf("%s", comando);

        if(strcmp(comando, "TRACE") == 0){
            int id1, id2;
            scanf("%d %d", &id1, &id2);
            trace_rota(roteadores, id1, id2);
        }
        else if(strcmp(comando, "LINK")== 0){
            scanf("%s", comando_aux);
            int id1, id2;

            if(strcmp(comando_aux, "ADD")== 0){
                int banda;
                scanf("%d %d %d", &id1, &id2, &banda);

                // Valida existencia (checando limites e delecao logica)
                int r1_existe = (id1 >= 1 && id1 <= roteadores->Z && roteadores->matriz_bandwidth[id1-1][id1-1] != -1);
                int r2_existe = (id2 >= 1 && id2 <= roteadores->Z && roteadores->matriz_bandwidth[id2-1][id2-1] != -1);

                if (!r1_existe && !r2_existe) printf("Roteadores %d e %d nao existem\n", id1, id2);
                else if (!r1_existe) printf("Roteador %d nao existe\n", id1);
                else if (!r2_existe) printf("Roteador %d nao existe\n", id2);
                else {
                    if(banda == 10 || banda == 100 || banda == 1000){
                        add_link(roteadores, id1, id2, banda);
                    } else {
                        printf("Largura de banda invalida\n");
                    }
                }
            }
            else if(strcmp(comando_aux, "DEL")== 0){
                scanf("%d %d", &id1, &id2);

                int r1_existe = (id1 >= 1 && id1 <= roteadores->Z && roteadores->matriz_bandwidth[id1-1][id1-1] != -1);
                int r2_existe = (id2 >= 1 && id2 <= roteadores->Z && roteadores->matriz_bandwidth[id2-1][id2-1] != -1);

                if (!r1_existe && !r2_existe) printf("Roteadores %d e %d nao existem\n", id1, id2);
                else if (!r1_existe) printf("Roteador %d nao existe\n", id1);
                else if (!r2_existe) printf("Roteador %d nao existe\n", id2);
                else {
                    del_link(roteadores, id1, id2);
                }
            }
        }
        else if(strcmp(comando, "ROUTER") == 0){
            scanf("%s", comando_aux);
            if(strcmp(comando_aux, "ADD") == 0){
                router_add(roteadores);
            }
            else if(strcmp(comando_aux, "DEL") == 0){
                int id;
                scanf("%d", &id);
                router_del(roteadores, id);
            }
        }
    }

    printf("Programa finalizado\n");
    //Libera memoria depois do EXIT
    liberar_rede(roteadores);

    return 0;
}
