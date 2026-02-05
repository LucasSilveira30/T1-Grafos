#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>

#define MAX_VERTICES 100
#define INFINITO INT_MAX

typedef struct {
    int num_vertices;
    int num_arestas;
    char tipo; 
    bool valorado;
    int matriz_adj[MAX_VERTICES][MAX_VERTICES];
} Grafo;

void inicializarGrafo(Grafo *g);
void lerGrafoArquivo(Grafo *g, const char *nome_arquivo);
void salvarGrafoArquivo(Grafo *g, const char *nome_arquivo);
void criarGrafoUsuario(Grafo *g);
void imprimirMatrizAdjacencia(Grafo *g);
void salvarMatrizAdjacencia(Grafo *g, const char *nome_arquivo);
void calcularGrauVertices(Grafo *g);
void primMST(Grafo *g);
void dijkstra(Grafo *g, int origem);
void buscaLargura(Grafo *g, int inicio);
void buscaProfundidade(Grafo *g, int inicio);
void dfsUtil(Grafo *g, int vertice, bool visitado[]);
void exibirMenu();

int main() {
    Grafo g;
    int opcao;
    char nome_arquivo[100];
    int vertice_inicio;

    inicializarGrafo(&g);

    do {
        exibirMenu();
        scanf("%d", &opcao);
        getchar(); 

        switch(opcao) {
            case 1:
                printf("Digite o nome do arquivo: ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0';
                lerGrafoArquivo(&g, nome_arquivo);
                break;
            case 2:
                criarGrafoUsuario(&g);
                break;
            case 3:
                printf("Digite o nome do arquivo para salvar: ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0';
                salvarGrafoArquivo(&g, nome_arquivo);
                break;
            case 4:
                imprimirMatrizAdjacencia(&g);
                break;
            case 5:
                printf("Digite o nome do arquivo para salvar a matriz: ");
                fgets(nome_arquivo, sizeof(nome_arquivo), stdin);
                nome_arquivo[strcspn(nome_arquivo, "\n")] = '\0';
                salvarMatrizAdjacencia(&g, nome_arquivo);
                break;
            case 6:
                calcularGrauVertices(&g);
                break;
            case 7:
                primMST(&g);
                break;
            case 8:
                printf("Digite o vértice de origem: ");
                scanf("%d", &vertice_inicio);
                dijkstra(&g, vertice_inicio);
                break;
            case 9:
                printf("Digite o vértice de início: ");
                scanf("%d", &vertice_inicio);
                buscaLargura(&g, vertice_inicio);
                break;
            case 10:
                printf("Digite o vértice de início: ");
                scanf("%d", &vertice_inicio);
                buscaProfundidade(&g, vertice_inicio);
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while(opcao != 0);

    return 0;
}

void inicializarGrafo(Grafo *g) {
    g->num_vertices = 0;
    g->num_arestas = 0;
    g->tipo = 'G';
    g->valorado = false;
    for(int i = 0; i < MAX_VERTICES; i++) {
        for(int j = 0; j < MAX_VERTICES; j++) {
            g->matriz_adj[i][j] = 0;
        }
    }
}

void lerGrafoArquivo(Grafo *g, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if(arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    int valorado;
    fscanf(arquivo, "%d %d %c %d", &g->num_vertices, &g->num_arestas, &g->tipo, &valorado);
    g->valorado = (valorado == 1);

    for(int i = 0; i < g->num_vertices; i++) {
        for(int j = 0; j < g->num_vertices; j++) {
            g->matriz_adj[i][j] = 0;
        }
    }

    int vi, vj, peso = 1;
    for(int i = 0; i < g->num_arestas; i++) {
        if(g->valorado) {
            fscanf(arquivo, "%d %d %d", &vi, &vj, &peso);
        } else {
            fscanf(arquivo, "%d %d", &vi, &vj);
        }

        g->matriz_adj[vi][vj] = peso;
        if(g->tipo == 'G') {
            g->matriz_adj[vj][vi] = peso;
        }
    }

    fclose(arquivo);
    printf("Grafo carregado com sucesso!\n");
}

void salvarGrafoArquivo(Grafo *g, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "w");
    if(arquivo == NULL) {
        printf("Erro ao criar o arquivo!\n");
        return;
    }

    fprintf(arquivo, "%d %d %c %d\n", g->num_vertices, g->num_arestas, g->tipo, g->valorado ? 1 : 0);

    for(int i = 0; i < g->num_vertices; i++) {
        for(int j = 0; j < g->num_vertices; j++) {
            if(g->matriz_adj[i][j] != 0) {
                // Para grafos não-dirigidos, escrever apenas uma vez (i <= j)
                if(g->tipo == 'D' || i <= j) {
                    if(g->valorado) {
                        fprintf(arquivo, "%d %d %d\n", i, j, g->matriz_adj[i][j]);
                    } else {
                        fprintf(arquivo, "%d %d\n", i, j);
                    }
                }
            }
        }
    }

    fclose(arquivo);
    printf("Grafo salvo com sucesso no arquivo %s\n", nome_arquivo);
}

void criarGrafoUsuario(Grafo *g) {
    char tipo;
    int valorado;

    printf("Número de vértices: ");
    scanf("%d", &g->num_vertices);
    printf("Tipo (G para não-dirigido, D para dirigido): ");
    scanf(" %c", &tipo);
    printf("Valorado (1 para sim, 0 para não): ");
    scanf("%d", &valorado);

    g->tipo = tipo;
    g->valorado = (valorado == 1);
    g->num_arestas = 0;

    for(int i = 0; i < g->num_vertices; i++) {
        for(int j = 0; j < g->num_vertices; j++) {
            g->matriz_adj[i][j] = 0;
        }
    }

    printf("Adicionar arestas (digite -1 para parar):\n");
    int vi, vj, peso = 1;

    while(1) {
        printf("Origem: ");
        scanf("%d", &vi);
        if(vi == -1) break;
        printf("Destino: ");
        scanf("%d", &vj);
        if(g->valorado) {
            printf("Peso: ");
            scanf("%d", &peso);
        }

        g->matriz_adj[vi][vj] = peso;
        if(g->tipo == 'G') {
            g->matriz_adj[vj][vi] = peso;
        }
        g->num_arestas++;
    }
}

void imprimirMatrizAdjacencia(Grafo *g) {
    printf("\nMatriz de Adjacência:\n");
    for(int i = 0; i < g->num_vertices; i++) {
        for(int j = 0; j < g->num_vertices; j++) {
            printf("%d ", g->matriz_adj[i][j]);
        }
        printf("\n");
    }
}

void salvarMatrizAdjacencia(Grafo *g, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "w");
    if(arquivo == NULL) {
        printf("Erro ao criar o arquivo!\n");
        return;
    }

    for(int i = 0; i < g->num_vertices; i++) {
        for(int j = 0; j < g->num_vertices; j++) {
            fprintf(arquivo, "%d", g->matriz_adj[i][j]);
            if(j != g->num_vertices - 1) fprintf(arquivo, " ");
        }
        if(i != g->num_vertices - 1) fprintf(arquivo, "\n");
    }

    fclose(arquivo);
    printf("Matriz de adjacência salva no arquivo %s\n", nome_arquivo);
}

void calcularGrauVertices(Grafo *g) {
    printf("\nGrau dos vértices:\n");
    for(int i = 0; i < g->num_vertices; i++) {
        int grau = 0;
        for(int j = 0; j < g->num_vertices; j++) {
            if(g->matriz_adj[i][j] != 0) grau++;
        }
        if(g->tipo == 'D') {
            int grau_entrada = 0;
            for(int j = 0; j < g->num_vertices; j++) {
                if(g->matriz_adj[j][i] != 0) grau_entrada++;
            }
            printf("Vértice %d: Grau de saída = %d, Grau de entrada = %d\n", i, grau, grau_entrada);
        } else {
            printf("Vértice %d: Grau = %d\n", i, grau);
        }
    }
}

void primMST(Grafo *g) {
    if(g->tipo == 'D') {
        printf("O algoritmo de Prim não funciona com grafos dirigidos!\n");
        return;
    }
    if(!g->valorado) {
        printf("O grafo precisa ser valorado para encontrar a MST!\n");
        return;
    }

    int parent[g->num_vertices];
    int key[g->num_vertices];
    bool inMST[g->num_vertices];

    for(int i = 0; i < g->num_vertices; i++) {
        key[i] = INFINITO;
        inMST[i] = false;
    }

    key[0] = 0;
    parent[0] = -1;

    for(int count = 0; count < g->num_vertices - 1; count++) {
        int min = INFINITO, min_index;

        for(int v = 0; v < g->num_vertices; v++) {
            if(!inMST[v] && key[v] < min) {
                min = key[v];
                min_index = v;
            }
        }

        inMST[min_index] = true;

        for(int v = 0; v < g->num_vertices; v++) {
            if(g->matriz_adj[min_index][v] && !inMST[v] && g->matriz_adj[min_index][v] < key[v]) {
                parent[v] = min_index;
                key[v] = g->matriz_adj[min_index][v];
            }
        }
    }

    printf("\nÁrvore Geradora Mínima (Prim):\n");
    int total_weight = 0;
    for(int i = 1; i < g->num_vertices; i++) {
        printf("%d - %d \t%d\n", parent[i], i, g->matriz_adj[i][parent[i]]);
        total_weight += g->matriz_adj[i][parent[i]];
    }
    printf("Peso total: %d\n", total_weight);
}

void dijkstra(Grafo *g, int origem) {
    if(!g->valorado) {
        printf("O algoritmo de Dijkstra requer um grafo valorado!\n");
        return;
    }

    int dist[g->num_vertices];
    bool sptSet[g->num_vertices];

    for(int i = 0; i < g->num_vertices; i++) {
        dist[i] = INFINITO;
        sptSet[i] = false;
    }

    dist[origem] = 0;

    for(int count = 0; count < g->num_vertices - 1; count++) {
        int min = INFINITO, u;

        for(int v = 0; v < g->num_vertices; v++) {
            if(!sptSet[v] && dist[v] <= min) {
                min = dist[v];
                u = v;
            }
        }

        sptSet[u] = true;

        for(int v = 0; v < g->num_vertices; v++) {
            if(!sptSet[v] && g->matriz_adj[u][v] && dist[u] != INFINITO &&
               dist[u] + g->matriz_adj[u][v] < dist[v]) {
                dist[v] = dist[u] + g->matriz_adj[u][v];
            }
        }
    }

    printf("\nCaminhos mais curtos a partir do vértice %d:\n", origem);
    for(int i = 0; i < g->num_vertices; i++) {
        if(i != origem) {
            printf("Para %d: ", i);
            if(dist[i] == INFINITO) {
                printf("Inalcançável\n");
            } else {
                printf("Distância = %d\n", dist[i]);
            }
        }
    }
}

void buscaLargura(Grafo *g, int inicio) {
    bool visitado[g->num_vertices];
    for(int i = 0; i < g->num_vertices; i++) {
        visitado[i] = false;
    }

    int fila[g->num_vertices];
    int frente = 0, tras = 0;

    visitado[inicio] = true;
    fila[tras++] = inicio;

    printf("\nBusca em Largura a partir do vértice %d:\n", inicio);

    while(frente < tras) {
        int vertice = fila[frente++];
        printf("%d ", vertice);

        for(int i = 0; i < g->num_vertices; i++) {
            if(g->matriz_adj[vertice][i] != 0 && !visitado[i]) {
                visitado[i] = true;
                fila[tras++] = i;
            }
        }
    }
    printf("\n");
}

void buscaProfundidade(Grafo *g, int inicio) {
    bool visitado[g->num_vertices];
    for(int i = 0; i < g->num_vertices; i++) {
        visitado[i] = false;
    }

    printf("\nBusca em Profundidade a partir do vértice %d:\n", inicio);
    dfsUtil(g, inicio, visitado);
    printf("\n");
}

void dfsUtil(Grafo *g, int vertice, bool visitado[]) {
    visitado[vertice] = true;
    printf("%d ", vertice);

    for(int i = 0; i < g->num_vertices; i++) {
        if(g->matriz_adj[vertice][i] != 0 && !visitado[i]) {
            dfsUtil(g, i, visitado);
        }
    }
}

void exibirMenu() {
    printf("\n=== MENU ===\n");
    printf("1. Ler grafo de arquivo\n");
    printf("2. Criar grafo manualmente\n");
    printf("3. Salvar grafo em arquivo\n");
    printf("4. Exibir matriz de adjacência\n");
    printf("5. Salvar matriz de adjacência em arquivo\n");
    printf("6. Calcular grau dos vértices\n");
    printf("7. Encontrar Árvore Geradora Mínima (Prim)\n");
    printf("8. Calcular caminhos mais curtos (Dijkstra)\n");
    printf("9. Busca em Largura (BFS)\n");
    printf("10. Busca em Profundidade (DFS)\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
}
