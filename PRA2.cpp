#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#define TAM_MEM 100000
#define TAM_ARQ 1000000


typedef struct {
long long int id, temp, hora, minuto, segundo, dia, mes, ano;
}registro;


void iniciaContagem();
void terminaContagem();
void geraArquivoIni(FILE *fp);
int reservatorioVazio(FILE *reservatorio);

struct timeval tv_ini, tv_fim;
unsigned long time_diff, sec_diff, usec_diff, msec_diff;
int rc;

int main()
{
    long int pos_stream = 0, pos_fp, flag = 1;
    int i,j, nSaida=1, primeiroElemento=1,ret=1,prev_saida=1;
    int elementos_reservatorio = 0;
    char saida[15];
    registro *memoria;
    registro last;

    FILE *fp;
    FILE *reservatorio;
    FILE *out;
    fpos_t ultimo;

    memoria = (registro*)malloc(sizeof(registro)*TAM_MEM);
    srand(time(NULL));

    fp = fopen("teste.txt", "wb");
    
    iniciaContagem();
    geraArquivoIni(fp);    
    terminaContagem();
    iniciaContagem();
    fclose(fp);
    fp=fopen("teste.txt", "rb");
/*    while(!feof(fp))
    {
        registro k;
        fread(&k,sizeof(registro), 1, fp);
        printf("%d C %d h %d min %d seg -  %d / %d / %d\n", k.temp, k.hora, k.minuto, k.segundo, k.dia, k.mes, k.ano);
    }
    printf("\n\n\n");*/
    rewind(fp);
    fread(memoria, sizeof(registro), TAM_MEM, fp);
    
    int menor = 51;
    int pos_menor;
    registro k;
    sprintf(saida,"saida_%d.txt", nSaida);

    out = fopen(saida, "ab");
    while(!feof(fp)){
        reservatorio = fopen("reservatorio.txt", "wb");
        fclose(reservatorio);
        reservatorio = fopen("reservatorio.txt", "ab");
        while(elementos_reservatorio < TAM_MEM && (ret==1)){
            if(flag==1){
                ret = fread(&k,sizeof(registro), 1, fp);
            }else{ 
                ret = flag;
            }
            for(i=0;i<TAM_MEM;i++)
            {
                if(memoria[i].temp < menor)
                {
                    menor = memoria[i].temp;
                    pos_menor = i;                    
                }
            }
            //printf("MENOR: %d - POSICAO: %d\n\n", memoria[pos_menor].temp, pos_menor);

            sprintf(saida,"saida_%d.txt", nSaida);
            if(nSaida != prev_saida)
            {
                out = fopen(saida, "ab");
                prev_saida++;
            }
            fseek(out, 0, SEEK_END); 
            //fgetpos(out, &ultimo);
            if(!primeiroElemento)
            {
                //fsetpos(out, &ultimo);
                //fread(&j,sizeof(registro), 1, out);
                if(last.temp <= memoria[pos_menor].temp)
                {
                    //printf("@@@%d > %d\n", memoria[pos_menor].temp, last.temp);
                    fseek(out, 0, SEEK_END);
                    if(memoria[pos_menor].temp != 59){ 
                        fwrite(&memoria[pos_menor],sizeof(registro), 1, out);
                       // printf("Entrou MENOR: %d\n", memoria[pos_menor].temp);
                    }
                    //printf("%d   ", memoria[pos_menor].temp);
                    last.temp = memoria[pos_menor].temp;
                    memoria[pos_menor].temp = 59;
                    if (ret == 1)
                    {
                        memcpy(&memoria[pos_menor], &k, sizeof(registro));
                    }
                    //fread(&memoria[pos_menor],sizeof(registro), 1, fp);        
                    //printf("   %d", memoria[pos_menor].temp);
                }
                else
                {
                    //printf("@@@ENTROU RES\n\n");
                    fwrite(&memoria[pos_menor],sizeof(registro), 1, reservatorio);
                    memoria[pos_menor].temp = 59;
                    if(ret ==1 && (elementos_reservatorio < TAM_MEM-1))
                    {
                        memcpy(&memoria[pos_menor], &k, sizeof(registro));    
                        flag = 1;
                    }
                    else
                    {    
                        flag = 0;                   
                        /*pos_fp = ftell(fp);                       
                        printf("POS_FP ANTES: %ld\n",pos_fp);
                        fseek(fp, -64, SEEK_CUR);
                        pos_fp = ftell(fp);            
                        printf("POS_FP DEPOIS: %ld\n",pos_fp);        
                        //fwrite(&k,sizeof(registro), 1, fp);
                        */
                    }                
                    //fread(&memoria[pos_menor],sizeof(registro), 1, fp);   
                    elementos_reservatorio++;     
                }            
            }
            else//é primeiro elemento
            {
                //printf("FIRST:%d\n", memoria[pos_menor].temp);
                fseek(out, 0, SEEK_END); 
                fwrite(&memoria[pos_menor],sizeof(registro), 1, out);
                last = memoria[pos_menor];                
                //fread(&memoria[pos_menor],sizeof(registro), 1, fp); 
                memcpy(&memoria[pos_menor], &k, sizeof(registro));            //conferir com mais calma
                primeiroElemento=0;
                //rewind(out);                     
            }
            menor = 51;
            
            //printf("elementos no reservatorio: %d\n", elementos_reservatorio);        
        }
        //printf("\n\n\n");
        /*for(i=0;i<5;i++){
            printf("%d\n", memoria[i].temp);
        }*/
        fseek(out, 0, SEEK_END); 
        //pos_stream = ftell(reservatorio);
        pos_stream = sizeof(registro)*TAM_MEM;
        //printf(">>>>>>>>> pos_stream = %ld\n", pos_stream);
        for(j=0;j<TAM_MEM;j++)
        {
            for(i=0;i<TAM_MEM;i++)
            {
                if(memoria[i].temp < menor)
                {
                    menor = memoria[i].temp;
                    pos_menor = i;
                }
            }
            fseek(out, 0, SEEK_END); 
            if(memoria[pos_menor].temp != 59)
            {
                fwrite(&memoria[pos_menor],sizeof(registro), 1, out);
            }
            //printf("SOBRAS: %d\n\n", memoria[pos_menor].temp);
            //printf("J == %d", j);
            memoria[pos_menor].temp = 59;
            menor = 51;
        }
        fclose(reservatorio);
        reservatorio = fopen("reservatorio.txt","rb");
        fseek(reservatorio, 0, SEEK_END); 
        pos_stream = ftell(reservatorio);
        //printf("@@@>>>>>>>>> pos_stream = %ld\n", pos_stream);
        rewind(reservatorio);
        i=0;
        //printf("BOTANDO NA MEMORIAAAA: \n");
        while(i<(pos_stream/64))
        {
            fread(&memoria[i],sizeof(registro), 1, reservatorio);
            //printf("%d\n",memoria[i].temp);
            i++;
        }
        /*
        while(!feof(reservatorio))
	    {
		    fread(&k,sizeof(registro), 1, reservatorio);
            printf("ID = %d -> %d C %d h %d min %d seg -  %d / %d / %d\n", k.id,k.temp, k.hora, k.minuto, k.segundo, k.dia, k.mes, k.ano);
	    }
        */
        //for(i=0;i<TAM_MEM;i++)
        //    printf("@@@SOBRAS: %d\n\n", memoria[i].temp);
        //fread(memoria,sizeof(registro), pos_stream/32, reservatorio);

        //fclose(reservatorio);
        flag = 1;
        nSaida++;
        elementos_reservatorio =0;
        primeiroElemento=1;        
    }

    fclose(reservatorio);
    if(!reservatorioVazio(reservatorio))
    {
        //printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>lol\n");
        fclose(out);
        sprintf(saida,"saida_%d.txt", nSaida);
        out = fopen(saida, "wb");

        fseek(reservatorio, 0, SEEK_END); 
        pos_stream = ftell(reservatorio);
        //printf(">>>>>>>>> pos_stream = %ld\n", pos_stream);
        //fread(memoria,sizeof(registro), pos_stream/32, reservatorio);
        i=0;
        rewind(reservatorio);
        while(!feof(reservatorio))
        {
            fread(&memoria[i],sizeof(registro), 1, reservatorio);
            i++;
        }
        for(i=0;i<TAM_MEM;i++)
           // printf("@@@SOBRAS: %d\n\n", memoria[i].temp);

        pos_stream = sizeof(registro)*TAM_MEM;
        //printf(">>>>>>>>> pos_stream = %ld\n", pos_stream);
        for(j=0;j<(pos_stream/64);j++)
        {
            for(i=0;i<TAM_MEM;i++)
            {
                if(memoria[i].temp < menor)
                {
                    menor = memoria[i].temp;
                    pos_menor = i;
                }
            }
            fseek(out, 0, SEEK_END); 
            if(memoria[pos_menor].temp != 59)
            {
                fwrite(&memoria[pos_menor],sizeof(registro), 1, out);
            }
           // printf("SOBRAS: %d\n\n", memoria[pos_menor].temp);
           // printf("J == %d", j);
            memoria[pos_menor].temp = 59;
            menor = 51;
        }
        
    }

    fclose(fp);
    fclose(out);
    fclose(reservatorio);
    out = fopen(saida, "rb");
    reservatorio = fopen("reservatorio.txt", "rb");
    rewind(out);
    fseek ( out ,0 , SEEK_SET );
    rewind(reservatorio);
    //printf("\n>>SAIDAS: \n");
    /*
    
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
    */
    terminaContagem();
    
    return 0;
}

int reservatorioVazio(FILE *reservatorio)
{ 
    int pos_stream;
    reservatorio = fopen("reservatorio.txt","rb");
    fseek(reservatorio, 0, SEEK_END); 
    if((pos_stream = ftell(reservatorio))==0)
    {printf("POS_STREAM: %d\n\n", pos_stream);
        return 1;
    }printf("POS_STREAM: %d\n\n", pos_stream);
    return 0;
}

void geraArquivoIni(FILE *fp){
    int i = 0;
    for(i=0;i<TAM_ARQ;i++)
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
    printf("Tempo para gerar o programa: %lu.%03lu\n",msec_diff, usec_diff%1000);

}
