#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>

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
    char saida[40];
    registro *memoria;
    registro last;
    last.temp = 59;
    
    FILE *fp;
    FILE *reservatorio;
    FILE *out;
    fpos_t ultimo;

    memoria = (registro*)malloc(sizeof(registro)*TAM_MEM);
    srand(time(NULL));

    fp = fopen("teste.txt", "wb");
    
    iniciaContagem();
    geraArquivoIni(fp); 
    printf("Tempo para gerar arquivo teste: ");   
    terminaContagem();
    printf("\n");
    iniciaContagem();
    fclose(fp);
    fp=fopen("teste.txt", "rb");

    rewind(fp);
    fread(memoria, sizeof(registro), TAM_MEM, fp);
    
    int menor = 51;
    int pos_menor;
    registro k;
    sprintf(saida,"saida_0_arq_%d.txt", nSaida);

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
                if(memoria[i].temp == last.temp){
                    menor = memoria[i].temp;
                    pos_menor = i;
                    break;  
                }
                else if(memoria[i].temp < menor)
                {
                    menor = memoria[i].temp;
                    pos_menor = i;                    
                }
            }

            sprintf(saida,"saida_0_arq_%d.txt", nSaida);
            if(nSaida != prev_saida)
            {
                out = fopen(saida, "ab");
                prev_saida++;
            }
            fseek(out, 0, SEEK_END); 
            if(!primeiroElemento)
            {

                if(last.temp <= memoria[pos_menor].temp)
                {
                    fseek(out, 0, SEEK_END);
                    if(memoria[pos_menor].temp != 59){ 
                        fwrite(&memoria[pos_menor],sizeof(registro), 1, out);
                    }

                    last.temp = memoria[pos_menor].temp;
                    memoria[pos_menor].temp = 59;
                    if (ret == 1)
                    {
                        memcpy(&memoria[pos_menor], &k, sizeof(registro));
                    }
                }
                else
                {
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
                    }                
                    elementos_reservatorio++;     
                }            
            }
            else//é primeiro elemento
            {
                fseek(out, 0, SEEK_END); 
                fwrite(&memoria[pos_menor],sizeof(registro), 1, out);
                last = memoria[pos_menor];                
                memcpy(&memoria[pos_menor], &k, sizeof(registro)); 
                primeiroElemento=0;                   
            }
            menor = 51;      
        }
        fseek(out, 0, SEEK_END); 
        //for(j=0;j<TAM_MEM;j++)
        //{
            for(i=0;i<TAM_MEM;i++)
            {
                if(memoria[i].temp < menor)
                {
                    menor = memoria[i].temp;
                    pos_menor = i;
                }
            //}
            fseek(out, 0, SEEK_END); 
            if(memoria[pos_menor].temp != 59)
            {
                fwrite(&memoria[pos_menor],sizeof(registro), 1, out);
            }
            memoria[pos_menor].temp = 59;
            menor = 51;
        }
        fclose(reservatorio);
        reservatorio = fopen("reservatorio.txt","rb");
        fseek(reservatorio, 0, SEEK_END); 
        pos_stream = ftell(reservatorio);
        rewind(reservatorio);
        i=0;
        while(i<TAM_MEM)
        {
            fread(&memoria[i],sizeof(registro), 1, reservatorio);
            i++;
        }
        flag = 1;
        nSaida++;
        elementos_reservatorio =0;
        primeiroElemento=1;        
    }

    fclose(reservatorio);
    if(!reservatorioVazio(reservatorio))
    {
        fclose(out);
        sprintf(saida,"saida_0_arq_%d.txt",nSaida );
        out = fopen(saida, "wb");

        fseek(reservatorio, 0, SEEK_END); 
        pos_stream = ftell(reservatorio);
        i=0;
        rewind(reservatorio);
        while(!feof(reservatorio))
        {
            fread(&memoria[i],sizeof(registro), 1, reservatorio);
            i++;
        }

        //pos_stream = sizeof(registro)*TAM_MEM;
        
        for(i=0;i<TAM_MEM;i++)
        {
            if(memoria[i].temp < menor)
            {
                menor = memoria[i].temp;
                pos_menor = i;
            }
            
            fseek(out, 0, SEEK_END); 
            if(memoria[pos_menor].temp != 59)
            {
                fwrite(&memoria[pos_menor],sizeof(registro), 1, out);
            }
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
    printf("Tempo para executar algoritmo de Selecao Natural: ");
    terminaContagem();
    printf("\n>>%d arquivos gerados\n", nSaida);
    /*FIM SELEÇÃO NATURAL*/
    
    /*INICIO INTERCALAÇÃO*/printf("COMEÇOU A TRETA\n");
    int saidasGeradas = nSaida, cont = 0, rodadas = 0, controle = 0;
    int duration = (int) nSaida/2;
    nSaida = 1;
    i=0;
    
    FILE *fp1, *fp2;
    registro r1, r2;    
    
    for(i=1; i<=100; i++){
        int n = pow(2,i);
        if(saidasGeradas <= n){
            rodadas = i;
            printf("%d\n\n\n",rodadas);
            break;
        }
    }
    i=0;
    int duration2 = duration;
    while(controle < rodadas){
        if(saidasGeradas%2!=0){
            sprintf(saida,"saida_%d_arq_%d.txt", controle, saidasGeradas);
            char saida1[40];
            sprintf(saida1,"saida_%d_arq_1.txt", controle+1);
            rename(saida, saida1);
            nSaida =1;
        }else{
            nSaida = 1;
        }
        while(duration--){
            sprintf(saida,"saida_%d_arq_%d.txt", controle, nSaida);
            fp1 = fopen(saida,"rb");
            
            sprintf(saida,"saida_%d_arq_%d.txt", controle, ++nSaida);
            fp2 = fopen(saida, "rb");
            
            sprintf(saida,"saida_%d_arq_%d.txt", controle+1, ++i);
            out = fopen(saida, "ab");
            
            fread(&r1,sizeof(registro), 1,fp1);
            fread(&r2,sizeof(registro), 1,fp2);
            
            while(!feof(fp1) && !feof(fp2)){
                if(r1.temp <= r2.temp){
                    fwrite(&r1,sizeof(registro), 1, out);
                    fread(&r1, sizeof(registro),1, fp1);
                }
                else if(r2.temp < r1.temp){
                    fwrite(&r2,sizeof(registro), 1, out);
                    fread(&r2, sizeof(registro),1, fp2);
                }
            }
            if(!feof(fp2))
            {
                while((ret=fread(&r2, sizeof(registro),1, fp2))==1)
                    fwrite(&r2,sizeof(registro), 1, out);
            }
            if(!feof(fp1))
            {
                while((ret=fread(&r1, sizeof(registro),1, fp1))==1)
                    fwrite(&r1,sizeof(registro), 1, out);
            }


            sprintf(saida,"saida_%d_arq_%d.txt",controle, --nSaida);
            remove(saida);
            sprintf(saida,"saida_%d_arq_%d.txt", controle, ++nSaida);
            remove(saida);
            nSaida++;
            cont++;
        }
        saidasGeradas = cont;
        duration = (int) duration2/2;
        duration2 = duration;
        controle++;
        i=0;
        cont=0;
    }
    return 0;
}

int reservatorioVazio(FILE *reservatorio)
{ 
    int pos_stream;
    reservatorio = fopen("reservatorio.txt","rb");
    fseek(reservatorio, 0, SEEK_END); 
    if((pos_stream = ftell(reservatorio))==0)
    {
        return 1;
    }
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
    printf(" %lu.%03lums",msec_diff, usec_diff%1000);

}
