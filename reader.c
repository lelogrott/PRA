#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

typedef struct {
long int id, temp, hora, minuto, segundo, dia, mes, ano;
}registro;

int main(){
	char saida[15];
	registro k;
	FILE *reader;
	int ret =1, i,cont=0;
	reader = fopen("teste.txt", "rb");
	while(!feof(reader))
	{
		fread(&k,sizeof(registro), 1, reader);
        printf("ID = %d -> %d C %d h %d min %d seg -  %d / %d / %d\n", k.id,k.temp, k.hora, k.minuto, k.segundo, k.dia, k.mes, k.ano);
	}
	fclose(reader);
	for(i=1;i<=3;i++)
    {
        printf("%d : \n", i);
        sprintf(saida,"saida_%d", i);
        reader = fopen(saida, "rb");
        while((ret= fread(&k,sizeof(registro), 1, reader))==1)
        {   cont++;     
            printf("ID = %d -> %d C %d h %d min %d seg -  %d / %d / %d\n", k.id,k.temp, k.hora, k.minuto, k.segundo, k.dia, k.mes, k.ano);
        }
        fclose(reader);
        printf("%d", cont);
    }
	return 0;
}