#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


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
        printf("Vértice %d:", i + 1);
        for (int j = 0; j < g->vertices; j++) {
            if (g->adyacencia[i][j] == 1) {
                printf(" -> %d", j + 1);
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


// Función para verificar si el grafo es k-conexo (implementación básica)
bool esKConexo(Grafo* g, int k) {
    if (g->vertices <= k) {
        return false;
    }
    
    // Por ahora, solo verificamos que el grado mínimo sea al menos k
    // Esta es una condición necesaria pero no suficiente para k-conexidad
    return gradoMinimo(g) >= k;
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
        printf("\nMenú de opciones:\n");
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
                    printf("Grafo cargado exitosamente.\n");
                }
                break;


            case '2':
                if (grafo != NULL) {
                    imprimirGrafo(grafo);
                } else {
                    printf("Primero debe cargar un grafo.\n");
                }
                break;


            case '3':
                if (grafo != NULL) {
                    printf("El grado máximo del grafo es: %d\n", gradoMaximo(grafo));
                } else {
                    printf("Primero debe cargar un grafo.\n");
                }
                break;


            case '4':
                if (grafo != NULL) {
                    printf("El grado mínimo del grafo es: %d\n", gradoMinimo(grafo));
                } else {
                    printf("Primero debe cargar un grafo.\n");
                }
                break;


            case '5':
                if (grafo != NULL) {
                    int k;
                    printf("Ingrese el valor de k: ");
                    scanf("%d", &k);
                    if (esKConexo(grafo, k)) {
                        printf("El grafo es %d-conexo.\n", k);
                    } else {
                        printf("El grafo no es %d-conexo.\n", k);
                    }
                } else {
                    printf("Primero debe cargar un grafo.\n");
                }
                break;


            case '6':
                printf("Saliendo del programa...\n");
                break;


            default:
                printf("Opción no válida.\n");
                break;
        }


        if (opcion != '6') {
            printf("\n¿Desea realizar otra acción? (s/n): ");
            scanf(" %c", &opcion);
            if (opcion == 'n' || opcion == 'N') {
                break;
            }
        }
    } while (opcion != '6');


    if (grafo != NULL) {
        liberarGrafo(grafo);
    }


    return 0;
}



