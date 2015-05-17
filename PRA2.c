#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

//using namespace std;

typedef struct {
long int id, temp, hora, minuto, segundo, dia, mes, ano;
}registro;


void iniciaContagem();
void terminaContagem();
void geraArquivoIni(FILE *fp);

struct timeval tv_ini, tv_fim;
unsigned long time_diff, sec_diff, usec_diff, msec_diff;
int rc;

int main()
{
    int i;
    int elementos_reservatorio = 0;
    std::string saida("saida_");
    registro *memoria;
    
    printf("string = %s\n", saida);return;
    
    FILE *fp;
    FILE *reservatorio;
    //FILE *saida;

    memoria = (registro*)malloc(sizeof(registro)*10);
    srand(time(NULL));

    fp = fopen("teste.txt", "wb");
    reservatorio = fopen("reservatorio.txt", "wb");
    
    iniciaContagem();
    geraArquivoIni(fp);    
    terminaContagem();
    
    fclose(fp);
    
    fp=fopen("teste.txt", "rb");
    for(i=0;i<50;i++)
    {
        registro k;
        fread(&k,sizeof(registro), 1, fp);
        printf("%d ºC %d h %d min %d seg -  %d / %d / %d\n", k.temp, k.hora, k.minuto, k.segundo, k.dia, k.mes, k.ano);
    }
    printf("\n\n\n");
    rewind(fp);
    fread(memoria, sizeof(registro), 10, fp);
    
    int menor = 51;
    int pos_menor;
    while(elementos_reservatorio < 10){
        for(i=0;i<10;i++)
        {
            if(memoria[i].temp < menor)
            {
                menor = memoria[i].temp;
                pos_menor = i;
            }
        }
        fwrite(&memoria[pos_menor], sizeof(registro), 1, reservatorio);
        elementos_reservatorio++;
        fread(&memoria[pos_menor],sizeof(registro), 1, fp);
    }

    return 0;
}

void geraArquivoIni(FILE *fp){
    int i = 0;
    for(i=0;i<50;i++)
        {
            registro j;
            j.id = i+1;
            j.temp = rand() % 40;
            j.hora = rand() % 23;
            j.minuto = rand() % 59;
            j.segundo = rand() % 59;
            j.dia = 1 + rand() % 30;
            j.mes = 1 + rand() % 12;
            j.ano = 1995 + rand() % 20;
            fwrite(&j, sizeof(registro), 1, fp);
        }
}

void iniciaContagem()
{
    rc = gettimeofday(&tv_ini, NULL);
        if (rc != 0) {
            perror("erro em gettimeofday()");
            exit(1);
        }
}

void terminaContagem()
{
     rc = gettimeofday(&tv_fim, NULL);
    if (rc != 0) {
        perror("erro em gettimeofday()");
        exit(1);
    }
    /* calcula a diferenca entre os tempos, em usec */
    time_diff = (1000000L*tv_fim.tv_sec + tv_fim.tv_usec) - (1000000L*tv_ini.tv_sec + tv_ini.tv_usec);
    /* converte para segundos + microsegundos (parte fracionária) */
    sec_diff = time_diff / 1000000L;
    usec_diff = time_diff % 1000000L;

    /* converte para msec */
    msec_diff = time_diff / 1000;
    printf("Tempo para gerar o programa 2: %lu.%03lu\n",msec_diff, usec_diff%1000);

}
