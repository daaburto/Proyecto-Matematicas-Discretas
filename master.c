#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define RESET   "\033[0m"   
#define ROJO    "\033[31m"  
#define VERDE   "\033[32m"  
#define MORADO  "\033[0;35m"


// Estructura para representar un grafo
typedef struct {
    int vertices;
    int **adyacencia;
} Grafo;


// Función para crear un grafo desde un archivo
Grafo* crearGrafoDesdeArchivo(const char* nombreArchivo) {
    FILE* archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("Error al abrir el archivo\n");
        return NULL;
    }


    Grafo* g = (Grafo*)malloc(sizeof(Grafo));
    
    // Leer número de vértices
    fscanf(archivo, "%d\n", &g->vertices);
    
    // Asignar memoria para la matriz de adyacencia
    g->adyacencia = (int**)malloc(g->vertices * sizeof(int*));
    for (int i = 0; i < g->vertices; i++) {
        g->adyacencia[i] = (int*)calloc(g->vertices, sizeof(int));
    }


    // Buffer para leer cada línea
    char linea[256];
    int vertice;
    
    // Leer cada línea del archivo
    while (fgets(linea, sizeof(linea), archivo)) {
        char* token = strtok(linea, ":"); // Separar el número del vértice
        if (token != NULL) {
            vertice = atoi(token) - 1; // Convertir a base 0
            
            // Obtener los vértices adyacentes
            token = strtok(NULL, ",");
            while (token != NULL) {
                int adyacente = atoi(token) - 1; // Convertir a base 0
                if (adyacente >= 0 && adyacente < g->vertices) {
                    g->adyacencia[vertice][adyacente] = 1;
                    g->adyacencia[adyacente][vertice] = 1;
                }
                token = strtok(NULL, ",");
            }
        }
    }


    fclose(archivo);
    return g;
}


// Función para imprimir el grafo
void imprimirGrafo(Grafo* g) {
    printf("\nRepresentación del grafo:\n");
    for (int i = 0; i < g->vertices; i++) {
        printf("Vértice %d: ", i + 1);
        for (int j = 0; j < g->vertices; j++) {
            if (g->adyacencia[i][j] == 1) {
                printf("%d ", j + 1);
            }
        }
        printf("\n");
    }
}


// Función para calcular el grado de un vértice
int calcularGrado(Grafo* g, int vertice) {
    int grado = 0;
    for (int i = 0; i < g->vertices; i++) {
        if (g->adyacencia[vertice][i] == 1) {
            grado++;
        }
    }
    return grado;
}


// Función para encontrar el grado máximo
int gradoMaximo(Grafo* g) {
    int max = 0;
    for (int i = 0; i < g->vertices; i++) {
        int grado = calcularGrado(g, i);
        if (grado > max) {
            max = grado;
        }
    }
    return max;
}


// Función para encontrar el grado mínimo
int gradoMinimo(Grafo* g) {
    int min = g->vertices;
    for (int i = 0; i < g->vertices; i++) {
        int grado = calcularGrado(g, i);
        if (grado < min) {
            min = grado;
        }
    }
    return min;
}


// Función para verificar si un grafo es conexo usando BFS
bool esConexo(Grafo* g, bool* verticesEliminados) {
    if (g->vertices <= 0) return false;
    
    // Array para marcar vértices visitados
    bool* visitado = (bool*)calloc(g->vertices, sizeof(bool));
    if (!visitado) return false;
    
    // Cola para BFS
    int* cola = (int*)malloc(g->vertices * sizeof(int));
    if (!cola) {
        free(visitado);
        return false;
    }
    
    // Encontrar el primer vértice no eliminado para empezar BFS
    int inicio = -1;
    for (int i = 0; i < g->vertices; i++) {
        if (!verticesEliminados[i]) {
            inicio = i;
            break;
        }
    }
    
    if (inicio == -1) {
        free(visitado);
        free(cola);
        return false;
    }
    
    // Inicializar BFS
    int frente = 0, final = 0;
    cola[final++] = inicio;
    visitado[inicio] = true;
    
    // Realizar BFS
    while (frente < final) {
        int actual = cola[frente++];
        
        for (int i = 0; i < g->vertices; i++) {
            if (g->adyacencia[actual][i] && !verticesEliminados[i] && !visitado[i]) {
                visitado[i] = true;
                cola[final++] = i;
            }
        }
    }
    
    // Verificar si todos los vértices no eliminados fueron visitados
    bool esConexo = true;
    for (int i = 0; i < g->vertices; i++) {
        if (!verticesEliminados[i] && !visitado[i]) {
            esConexo = false;
            break;
        }
    }
    
    free(visitado);
    free(cola);
    return esConexo;
}


// Función recursiva para generar todas las combinaciones posibles de k-1 vértices
bool verificarCombinaciones(Grafo* g, bool* verticesEliminados, int k, int inicio, int count) {
    // Si ya seleccionamos k-1 vértices, verificar si el grafo resultante es conexo
    if (count == k - 1) {
        return esConexo(g, verticesEliminados);
    }
    
    // Si no hay suficientes vértices restantes para alcanzar k-1
    if (g->vertices - inicio < k - 1 - count) {
        return true;
    }
    
    // Para cada vértice restante desde 'inicio'
    for (int i = inicio; i < g->vertices; i++) {
        // Marcar vértice como eliminado
        verticesEliminados[i] = true;
        
        // Verificar recursivamente con este vértice eliminado
        if (!verificarCombinaciones(g, verticesEliminados, k, i + 1, count + 1)) {
            return false;  // Si encontramos una combinación que desconecta el grafo
        }
        
        // Desmarcar vértice (backtracking)
        verticesEliminados[i] = false;
    }
    
    return true;
}


// Función principal para verificar si el grafo es k-conexo
bool esKConexo(Grafo* g, int k) {
    // Verificar condiciones básicas
    if (g == NULL || k < 1 || g->vertices <= k) {
        return false;
    }
    
    // Si k es 1, solo necesitamos verificar si el grafo es conexo
    if (k == 1) {
        bool* verticesEliminados = (bool*)calloc(g->vertices, sizeof(bool));
        if (!verticesEliminados) return false;
        
        bool resultado = esConexo(g, verticesEliminados);
        free(verticesEliminados);
        return resultado;
    }
    
    // Array para marcar vértices eliminados
    bool* verticesEliminados = (bool*)calloc(g->vertices, sizeof(bool));
    if (!verticesEliminados) return false;
    
    // Verificar todas las posibles combinaciones de k-1 vértices
    bool resultado = verificarCombinaciones(g, verticesEliminados, k, 0, 0);
    
    free(verticesEliminados);
    return resultado;
}

// Función para liberar la memoria del grafo
void liberarGrafo(Grafo* g) {
    for (int i = 0; i < g->vertices; i++) {
        free(g->adyacencia[i]);
    }
    free(g->adyacencia);
    free(g);
}


int main() {
    Grafo* grafo = NULL;
    char nombreArchivo[256];
    char opcion;
    
    do {
        printf("\n       Menú de opciones\n");
        printf("1. Cargar grafo desde archivo\n");
        printf("2. Mostrar grafo\n");
        printf("3. Calcular grado máximo\n");
        printf("4. Calcular grado mínimo\n");
        printf("5. Verificar k-conexidad\n");
        printf("6. Salir\n");
        printf("Seleccione una opción: ");
        scanf(" %c", &opcion);


        switch(opcion) {
            case '1':
                printf("Ingrese el nombre del archivo: ");
                scanf("%s", nombreArchivo);
                if (grafo != NULL) {
                    liberarGrafo(grafo);
                }
                grafo = crearGrafoDesdeArchivo(nombreArchivo);
                if (grafo != NULL) {
                    printf(VERDE "\nGrafo cargado exitosamente.\n" RESET);
                }
                break;


            case '2':
                if (grafo != NULL) {
                    imprimirGrafo(grafo);
                } else {
                    printf(ROJO "\nPrimero debe cargar un grafo.\n" RESET);
                }
                break;


            case '3':
                if (grafo != NULL) {
                    printf(VERDE "\nEl grado máximo del grafo es: %d\n"RESET, gradoMaximo(grafo));
                } else {
                    printf(ROJO "\nPrimero debe cargar un grafo.\n" RESET);
                }
                break;


            case '4':
                if (grafo != NULL) {
                    printf(VERDE "\nEl grado mínimo del grafo es: %d\n"RESET, gradoMinimo(grafo));
                } else {
                    printf(ROJO "\nPrimero debe cargar un grafo.\n" RESET);
                }
                break;


            case '5':
                if (grafo != NULL) {
                    int k;
                    printf("Ingrese el valor de k: ");
                    scanf("%d", &k);
                    if (esKConexo(grafo, k)) {
                        printf(VERDE"\nEl grafo es %d-conexo.\n"RESET, k);
                    } else {
                        printf(ROJO "\nEl grafo no es %d-conexo.\n"RESET, k);
                    }
                } else {
                    printf(ROJO "\nPrimero debe cargar un grafo.\n" RESET);
                }
                break;


            case '6':
                printf(MORADO "Saliendo del programa...\n");
                break;


            default:
                printf(ROJO"\nOpción no válida.\n"RESET);
                break;
        }


        if (opcion != '6') {
            printf("\n¿Desea realizar otra acción? (" VERDE"s"RESET"/"ROJO"n"RESET"): ");
            scanf(" %c", &opcion);
            if (opcion == 'n' || opcion == 'N') {
                printf(MORADO "\nSaliendo del programa...\n");
                break;
            }
        }

    } while (opcion != '6');


    if (grafo != NULL) {
        liberarGrafo(grafo);
    }


    return 0;
}



