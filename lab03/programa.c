#include "mpi.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

//nessa função faz a inserção de dados na matriz inicial
void matriz(float *m, char arq[100], int linha, int coluna) {

  int i, j;//contadores
  FILE *matriz_real;//é criado um ponteiro de FILE para abrir o arquivo

  matriz_real = fopen(arq, "r");//abertura do arquivo para leitura(r)

  for(i = 0; i < linha; i++)
    for(j = 0;j < coluna; j++)
      fscanf(matriz_real,"%f",&m[(coluna * i) + j]);//a matriz recebendo os dados do arquivo
  
  fclose(matriz_real);//fechando o arquivo
}

//função que faz a multiplição das matrizes
void multiplicar(int linha1, int coluna2, int linha2, float *matriz1, float *matriz2, float *matrizAux) {

  int i, j, k;//contadores

  //inicia a zona paralela identificando as variáveis compartilhadas e privadas
  {
    for(i = 0; i < linha1; i++){
      for(j = 0; j < coluna2; j++){
        matrizAux[(i * coluna2) + j] = 0;
          for(k = 0; k < linha2; k++) 
            matrizAux[(i * coluna2) + j] += matriz1[(i* linha2) + k] * matriz2[(k*coluna2) + j];
      }
    }
  }
}

//função que faz a soma pela redução da matriz D
float somaReducao(int linha, float *matriz) {

  float result = 0;//auxiliar

  //inicia zona paralela
      for(int i = 0; i < linha; i++)
        result += matriz[i];

  return result;
}

//função para salvar a matriz final no arqD.dat
void imprimir(float *resultado, int linhas, char arq[100]){

	int j;//contador
	FILE *matriz;//é criado um ponteiro de FILE para abrir o arquivo

	matriz = fopen(arq,"w");//abertura do arquivo para escrita(w)

	for(j=0; j < linhas; j++)
			fprintf(matriz,"%.2f\n",resultado[j]);//salvando dados no arqD.dat

	fclose(matriz);//fechamento do arquivo
}

void main (int argC, char *argV[]){

  //valores que o programa recebe
  int y = atof(argV[1]);
  int w = atof(argV[2]);
  int v = atof(argV[3]);

  //matrizes
  float *A, *B, *C, *D, *auxAB;

  float resultado = 0;

  //contadores de tempo
  clock_t tempoInicial;
  clock_t tempoFinal;
  double tempoExecucao;

  //recebe os path para os arquivos para extração e armazenamento dos dados
  char *arq_A = argV[4];
  char *arq_B = argV[5];
  char *arq_C = argV[6];
  char *arq_D = argV[7];

  //para tirar o "\n" das strings
  arq_A[strcspn(arq_A, "\n")] = 0;
  arq_B[strcspn(arq_B, "\n")] = 0;
  arq_C[strcspn(arq_C, "\n")] = 0;
  arq_D[strcspn(arq_D, "\n")] = 0;

  //Alocando as matrizes
  A = (float*) malloc(y * w * sizeof(float));
  B = (float*) malloc(w * v * sizeof(float));
  C = (float*) malloc(v * 1 * sizeof(float));
  D = (float*) malloc(y * 1 * sizeof(float));

  //alocando a matriz auxiliar
  auxAB = (float*) malloc(y * v * sizeof(float));

  //lendo os dados dos arquivos .dat
  matriz(A, arq_A, y, w);
  matriz(B, arq_B, w, v);
  matriz(C, arq_C, v, 1);

  tempoInicial = clock();//iniciando a contagem do tempo

  //chamada para a multiplicação das matrizes
  multiplicar(y, v, w, A, B, auxAB);
  multiplicar(y, 1, v, auxAB, C, D);

  //chamada para a soma pela redução da matriz D
  resultado = somaReducao(y, D);

  tempoFinal = clock();//finalização da contagem do tempo
  
  imprimir(D,y,arq_D);//escrita dos dados no arqD.dat

  //transformando o tempo em segundos
  tempoExecucao = (tempoFinal - tempoInicial) * 1000.0 / CLOCKS_PER_SEC;

  printf("Tempo para a conclusão da soma é de %lf, resultado %f\n", tempoExecucao,resultado);
}
