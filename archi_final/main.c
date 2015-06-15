#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define TEST 0

int *Index;
int *Tag;
int *Vaild;
int Addressing_bus;
int Sets;
int Associativity;
int Offset;
/*-----------Memory Allocate----------------*/
void Allocate_memory(int Addressing_bus,int Sets,int Associativity,int Offset)
{
    Index = (int*)malloc(Associativity*Sets*sizeof(int));
    Tag = (int*)malloc(Associativity*Sets*sizeof(int));
    Vaild = (int*)malloc(Associativity*Sets*sizeof(int));

}

/*--------------Determine Momory---------------*/
void Direct_Map(char address[32])
{
    int total_block = Sets*Associativity;
    int size= 0;
    while(total_block!=1)
    {
        total_block = total_block/2;
        size++;
    }
    printf("size = %d",size);

/*าชื๖ะÞีý
    int index=0,bit = 1;
    int count = 30;
    for(count = 30;count>size;count--)
    {
        if(address[count]=='1')
        {
            index+=pow(2,(count-2));
        }

    }
*/
     printf("\nIndex = %d\n",index);
}

int main(int argv,char* argc[])
{

    FILE *cache_file,*address_file;
    cache_file = fopen(argc[1],"r");
    address_file = fopen(argc[2],"r");

    printf("%s %s\n",argc[1],argc[2]);
/*-------------------------------Read data-------------------------*/
    char buffer[30];
    char comma[30];
    int number[4];
    if(cache_file==NULL&&address_file==NULL)
    {
        printf("File dosen't exists!");
    }
    else
    {
        printf("Current input file %s \n",argc[1]);
        int index = 0;
        fscanf(cache_file,"%s%s%d",buffer,comma,&number[index]);
        while(!feof(cache_file))
        {
            index++;
            printf("%s %s %d\n",buffer,comma,number[index-1]);
            fscanf(cache_file,"%s%s%d",buffer,comma,&number[index]);

        }
    }
    /*-------------setting value--------------------*/
    Addressing_bus = number[0];
    Sets = number[1];
    Associativity = number[2];
    Offset = number[3];
    Allocate_memory(Addressing_bus,Sets,Associativity,Offset);
    /*-------------setting value--------------------*/



    char address[32];
    while(!feof(address_file))
    {
        fscanf(address_file,"%s",address);
        if(strncmp(address,".",1)!=0)
        {
            Direct_Map(address);
        }
    }

#if TEST == 1
    char file[32]={'0','0','0','1','0','0','0','1','1','1','1','1','1','0','0','1','0','1','1','0','1','1','1','1','1',
    '0','0','1','0','0','0','0'};
    Direct_Map(file);
    int count;
    int sum = 0;
    char file2[11] ={'0','0','0','0','0','0','0','0','0','0','1'};
    for(count = 0;count<11;count++)
    {
        if(file2[count]=='1')
        {
            sum+=pow(2,count);
            printf("2^%d ",count);
        }
    }
    Direct_Map(file);
    printf("Sum = %d\n",sum);
#endif

/*-----------------------------------------------------------------------------------*/
    fclose(cache_file);
    fclose(address_file);
    return 0;
}


