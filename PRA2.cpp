#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>


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
    long int pos_stream = 0;
    int i,j, nSaida=1, primeiroElemento=1,ret=1;
    int elementos_reservatorio = 0;
    char saida[15];
    registro *memoria;
    registro last;

    FILE *fp;
    FILE *reservatorio;
    FILE *out;
    fpos_t ultimo;

    memoria = (registro*)malloc(sizeof(registro)*10);
    srand(time(NULL));

    fp = fopen("teste.txt", "wb");
    
    iniciaContagem();
    geraArquivoIni(fp);    
    terminaContagem();
    
    fclose(fp);
    fp=fopen("teste.txt", "rb");
    while(!feof(fp))
    {
        registro k;
        fread(&k,sizeof(registro), 1, fp);
        printf("%d C %d h %d min %d seg -  %d / %d / %d\n", k.temp, k.hora, k.minuto, k.segundo, k.dia, k.mes, k.ano);
    }
    printf("\n\n\n");
    rewind(fp);
    fread(memoria, sizeof(registro), 10, fp);
    
    int menor = 51;
    int pos_menor;
    registro k;
    while(!feof(fp)){
        reservatorio = fopen("reservatorio.txt", "wb");
        fclose(reservatorio);
        reservatorio = fopen("reservatorio.txt", "ab");
        while(elementos_reservatorio < 10 && ((ret = fread(&k,sizeof(registro), 1, fp))==1)){
            for(i=0;i<10;i++)
            {
                if(memoria[i].temp < menor)
                {
                    menor = memoria[i].temp;
                    pos_menor = i;
                }
            }
            sprintf(saida,"saida_%d", nSaida);

            out = fopen(saida, "ab");
            //fgetpos(out, &ultimo);
            if(!primeiroElemento)
            {
                //fsetpos(out, &ultimo);
                //fread(&j,sizeof(registro), 1, out);
                if(last.temp <= memoria[pos_menor].temp)
                {
                    //printf("@@@%d > %d\n", memoria[pos_menor].temp, last.temp);
                    fwrite(&memoria[pos_menor],sizeof(registro), 1, out);
                    //printf("%d   ", memoria[pos_menor].temp);
                    last.temp = memoria[pos_menor].temp;
                    memcpy(&memoria[pos_menor], &k, sizeof(registro));
                    //fread(&memoria[pos_menor],sizeof(registro), 1, fp);        
                    //printf("   %d", memoria[pos_menor].temp);
                }
                else
                {
                    //printf("@@@%d < %d\n", memoria[pos_menor].temp, last.temp);
                    fwrite(&memoria[pos_menor],sizeof(registro), 1, reservatorio);
                    memcpy(&memoria[pos_menor], &k, sizeof(registro));
                    //fread(&memoria[pos_menor],sizeof(registro), 1, fp);   
                    elementos_reservatorio++;     
                }            
            }
            else
            {
                fwrite(&memoria[pos_menor],sizeof(registro), 1, out);
                last = memoria[pos_menor];                
                //fread(&memoria[pos_menor],sizeof(registro), 1, fp); 
                memcpy(&memoria[pos_menor], &k, sizeof(registro));
                primeiroElemento=0;
                //rewind(out);                     
            }
            menor = 51;
            //printf("elementos no reservatorio: %d\n", elementos_reservatorio);        
        }
        printf("\n\n\n");
        for(i=0;i<10;i++){
            printf("%d\n", memoria[i].temp);
        }
        fseek (reservatorio, 0, SEEK_END); 
        pos_stream = ftell(reservatorio);
        //printf(">>>>>>>>> pos_stream = %ld\n", pos_stream);
        for(j=0;j<(pos_stream/32);j++)
        {
            for(i=0;i<10;i++)
            {
                if(memoria[i].temp < menor)
                {
                    menor = memoria[i].temp;
                    pos_menor = i;
                }
            }
            fwrite(&memoria[pos_menor],sizeof(registro), 1, out);
            memoria[pos_menor].temp = 59;
            menor = 51;
        }
        fread(memoria,sizeof(registro), (pos_stream/32), reservatorio);
        nSaida++;
        elementos_reservatorio =0;
        fclose(reservatorio);
    }
    fclose(fp);
    fclose(out);
    fclose(reservatorio);
    out = fopen(saida, "rb");
    reservatorio = fopen("reservatorio.txt", "rb");
    rewind(out);
    fseek ( out ,0 , SEEK_SET );
    rewind(reservatorio);
    printf("\n>>SAIDAS: \n");

    for(i=1;i<=nSaida;i++)
    {
        printf("%d : \n", i);
        sprintf(saida,"saida_%d", i);
        out = fopen(saida, "rb");
        while((ret= fread(&k,sizeof(registro), 1, out))==1)
        {        
            printf("%d C %d h %d min %d seg -  %d / %d / %d\n", k.temp, k.hora, k.minuto, k.segundo, k.dia, k.mes, k.ano);
        }
        fclose(out);
    }
    printf("\n>>RESERVATORIO:\n");
    while((ret = fread(&k,sizeof(registro), 1, reservatorio))==1)
    {        
        printf("%d C %d h %d min %d seg -  %d / %d / %d\n", k.temp, k.hora, k.minuto, k.segundo, k.dia, k.mes, k.ano);
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
