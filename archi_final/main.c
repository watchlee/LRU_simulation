#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

enum{TEST_ERROR,TEST_PRINT,TEST_FUNCTION};
#define TEST TEST_FUNCTION
#define DEBUG 0


int *Index;
int *Tag;
int *Vaild;


static int Addressing_bus;
int Sets;
int Associativity;
int Offset;

char** block_array;

/*-----------Memory Allocate----------------*/
void Allocate_memory(int Addressing_bus,int Sets,int Associativity,int Offset)
{
    //The number of blocks = Associativity * Sets
    Index = (int*)malloc(Associativity*Sets*sizeof(int));
    Tag = (int*)malloc(Associativity*Sets*sizeof(int));
    Vaild = (int*)malloc(Associativity*Sets*sizeof(int));
    block_array=(char**)malloc(Associativity*Sets*sizeof(void*));
    

    /*此處需要再做修改 需改成可以搭配n-way 的形式 目前只有direct mapping 模式 */
    int count;
    for(count = 0;count<Associativity*Sets;count++)
        block_array= (char*)malloc(Addressing_bus*sizeof(char*));
    int count2;
    for(count =0;count<Associativity*Sets;count++)
        block_array[count] = '0';
}

/*--------------Determine Momory---------------*/

/*--------------Algorithm---------------------*/
/*
 *              使用Branch and Bound 方法來解決此問題
 *
 * */

/*------------------------Addressing combination--------------------------*/

void combination_function(char address[],char temp_address[],int start,int end,int index,int r)
{
    if(index == r)
    {
        int count2;
        for(count2 = 0;count2<r;count2++)
            printf("%c ",temp_address[count2]);
        printf("\n");
        return;
    }
    int count;
    for(count = start;count<=end&&end-count+1>=r-index;count++)
    {
        temp_address[index] = address[count];
        combination_function(address,temp_address,count+1,end,index+1,r);
    }
}

void combination(char address[],int N,int K)
{
    char temp_address[K];
    combination_function(address,temp_address,0,N-1,0,K);
}

/*-----------------combination--------------------------*/


/*
 * tag | index | offset| = address
 *
 */
 
void Direct_Map(char** data_array,int total_data)
{

    int total_block = Sets*Associativity;
    /*設定初始值*/
    int loop;
    for(loop = 0;loop<total_block;loop++)
        Vaild[loop] = 0;
#if DEBUG==TEST_PRINT
    printf("Offset = %d\n",Offset);
#endif
    /*-----------取得Blocks的index------------------*/
    int index =0;
    while(total_block!=1)
    {
        total_block = total_block/2;
        index++;
    }
      
    
#if DEBUG == TEST_PRINT
    printf("Block_bits = %d\n",index);
    int out_loop,inner_loop;
    for(out_loop=1;out_loop<total_data;out_loop++)
    {
        for(inner_loop= 0;inner_loop<Addressing_bus;inner_loop++)
            printf("%c",data_array[out_loop][inner_loop]);
        printf("\n");
    }
    
#endif
    /*Using index to do search and LRU algorithm*/

    /*Getting all data_array's elements index*/
    int index_to_dec[total_data];
    int count;
    for(count = 0;count<total_data;count++)
    {
        index_to_dec[count] = 0;
    }
    for(count = 1;count<total_data;count++)
    {
        for(loop = 0;loop<index;loop++)
        {
            if(data_array[count][loop]=='1')
            {
                index_to_dec[count]+=pow(2,(loop-2));
            }
        }

    }
    /*Direct Mapping memory*/
/*    for(count = 1;count<total_data;count++)
    {
        if(strcmp(block_array[count][0],"0")==0)
        {
            printf("first using\n");
        }
    }
*/  
/*要做修正
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
    char address[Addressing_bus];
    int total = 0,total_data=0;
    /*------------read total_data------------------*/
    while(!feof(address_file))
    {
        fscanf(address_file,"%s",address);
        if(strncmp(address,".",1)!=0)
        {
            total++;
        }
        total_data++;
    }
    fclose(address_file);


    

    printf("Total data = %d and %d \n",total,total_data);
    /*first reading file is used to take how many data that I have to get!
    *Then, second reading is to store data into array.
    * */
    
    address_file = fopen(argc[2],"r");
    char *data_length,**data_array;
    data_array=(char**)malloc(total*sizeof(char*)+total*Addressing_bus*sizeof(char));
    int loop;
    for(loop = 0,data_length = (char*)(data_array+total);loop<total;loop++,data_length+=Addressing_bus)
        data_array[loop]=data_length;


    loop=0;
    while(!feof(address_file))
    {
        fscanf(address_file,"%s",address);
        if(strncmp(address,".",1)!=0)
        {
            
            /*當要複製資料時請勿使用 xxxx = address 這樣會造成. . .所有指標陣列都會指向最後address的值*/
            strcpy(data_array[loop],address);
            loop++;
        }
    }
    Direct_Map(data_array,total);
#if DEBUG==1
    /*印出時必須從第一筆開始印,因為第一筆是不相關的資料
     * */
    int inner_loop;
    int out_loop;
    for(out_loop = 1;out_loop<total;out_loop++)
    {
        for(inner_loop = 0;inner_loop<Addressing_bus;inner_loop++)
        {
            printf("%c",data_array[out_loop][inner_loop]);
        }
        printf("\n");
    }
#endif 


#if Test==1
    int read_times;
    for(read_times = 0;read_times<2;read_times++)
    {
        while(!feof(address_file))
        {
            
            fscanf(address_file,"%s",address);
            if(strncmp(address,".",1)!=0)
            {
            total++;      
            printf("%s\n",address);
            }
        }
        /*Read again file according to C(n,k)*/
        fclose(address_file);
        address_file = fopen(argc[2],"r");
        printf("read_times = %d , total = %d\n",read_times,total);
    }

    char file[32]={'0','0','0','1','0','0','0','1','1','1','1','1','1','0','0','1','0','1','1','0','1','1','1','1','1',
    '0','0','1','0','0','0','0'};
    char test[5]={'1','2','3','4','5'};
    Direct_Map(file);
    int count;
    int sum = 0;
    char file2[11] ={'0','0','0','0','0','0','0','0','0','0','1'};
    combination(test,5,3);
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
    
#if TEST==TEST_FUNCTION
    char str[5]={'a','b','c','d','e'};
#endif


/*-----------------------------------------------------------------------------------*/

    free(data_array);
    fclose(cache_file);
    fclose(address_file);
    return 0;
}


