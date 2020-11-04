#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>

void matriz(float *m, char arq[100], int linha, int coluna) {//nessa função faz a inserção de dados na matriz inicial
  int i, j;
  FILE *matriz_real;//cria um ponteiro de arquivo para abrir arquivos
  matriz_real = fopen(arq, "r");//abertura do arquivo para leitura
  #pragma omp parallel private(i, j) share(matriz_real, m, linha, coluna)
  {
    #pragma omp for
    {
      for(i = 0; i < linha; i++)
        for(j = 0;j < coluna; j++)
          fscanf(matriz_real,"%lf",&m[(coluna * i) + j]);////matriz inicial recebendo os dados
    }
  }
  fclose(matriz_real);//fechamento do arquivo
}

void multiplicar(int linha1, int coluna2, int linha2, float *matriz1, float *matriz2, float *matrizAux) {

  int i, j, k;
  #pragma omp parallel private(i, j, k) share(matriz1, matriz2, matrizAux, linha1, linha2, coluna2){
    #pragma omp for {
      for(i = 0; i < linha1; i++)
        for(j = 0; j < coluna2; j++){
          somaprod = 0;
          for(k = 0; k < linha2; k++) 
            somaprod += matriz1[i][k] * matriz2[k][j];

          matrizAux[i][j] = somaprod;
        }
    }
  }
}

float somaReducao(int linha, float *matriz) {
  float result = 0;
  #pragma omp parallel share(result, matriz, linha){
    #pragma omp for {
      for(int i = 0; i < linha; i++){
        result += matriz[i];
      }
    }
  }
}

void main (int argC, char *argV[]){

  int y = atof(argV[1]), w = atof(argV[2]), v = atof(argV[3]);

  float *A, *B, *C, *D, *auxAB, resultado;

  clock_t tempoInicial;//contadores de tempo
  clock_t tempoFinal;
  double tempoExecucao;

  char *arq_A = argV[4], *arq_B = argV[5], *arq_C = argV[6], *arq_D = argV[7];

  arq_A[strcspn(arq_A, "\n")] = 0;
  arq_B[strcspn(arq_B, "\n")] = 0;
  arq_C[strcspn(arq_C, "\n")] = 0;
  arq_D[strcspn(arq_D, "\n")] = 0;

  A = (float *) malloc(y * w * sizeof(float));
  B = (float *) malloc(w * v * sizeof(float));
  C = (float *) malloc(v * 1 * sizeof(float));
  D = (float *) malloc(y * 1 * sizeof(float));

  auxAB = (float *) malloc(y * v * sizeof(float));

  matriz(A, arq_A, y, w);
  matriz(B, arq_B, w, v);
  matriz(C, arq_C, v, 1);

  tempoInicial = clock();

  multiplicar(y, v, w, A, B, auxAB);
  multiplicar(y, 1, y, auxAB, C, D);

  resultado = somaReducao(y, D);
  
  tempoFinal = clock();

  tempoExecucao = (tempoFinal - tempoInicial) * 1000.0 / CLOCKS_PER_SEC;
  printf("Tempo para a conclusão da soma é de %lf\n", tempoExecucao);

}
