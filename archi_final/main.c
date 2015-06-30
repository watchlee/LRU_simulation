#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

enum{TEST_ERROR,TEST_PRINT,TEST_FUNCTION};
#define TEST TEST_FUNCTION
#define DEBUG TEST_PRINT 
#define INFINITE 999999999


int *Index;
int *Tag;
int *Vaild;


static int Addressing_bus;
int Sets;
int Associativity;
int Offset;

int** block_array;
static int total_miss_times = 0;
static int total_hit_times = 0;

/*-----------Memory Allocate----------------*/
void Allocate_memory(int Addressing_bus,int Sets,int Associativity,int Offset)
{
    //The number of blocks = Associativity * Sets
    Index = (int*)malloc(Associativity*Sets*sizeof(int));
    Tag = (int*)malloc(Associativity*Sets*sizeof(int));
    Vaild = (int*)malloc(Associativity*Sets*sizeof(int));
    block_array=(int**)malloc(Associativity*Sets*sizeof(void*));
    

    /*此處需要再做修改 需改成可以搭配n-way 的形式 目前只有direct mapping 模式 */
    int count;
    for(count = 0;count<Associativity*Sets;count++)
        block_array= (int*)malloc(Addressing_bus*sizeof(int*));
    int count2;
    for(count =0;count<Associativity*Sets;count++)
        block_array[count] = INFINITE;
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
#define TRIGGER 2
void Direct_Map(char** data_array,int total_data)
{
    FILE *result_file;
    result_file = fopen("result_file.txt","w");
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

    /*-----------取得Blocks的tag--------------------*/
    //int tag = Addressing_bus - index - Offset;  Can using Addressing_bus - index - Offset to replace this variable!
    

    /*Using index to do search and LRU algorithm*/

    /*Getting all data_array's elements index*/
    int index_to_dec[total_data];
    int tag_to_dec[total_data];
    int count;
    /*---------------初始化-----------------*/
    for(count = 0;count<total_data;count++)
    {
        index_to_dec[count] = 0;
        tag_to_dec[count] = 0;
    }
    for(count = 1;count<total_data;count++)
    {
        #if TRIGGER == 1
        for(loop = 0;loop<index;loop++)
        {
            if(data_array[count][loop]=='1')
            {
                index_to_dec[count]+=(int)(pow(2,(loop-2)));
                
            }
            
        }
        for(loop = index;loop<Addressing_bus;loop++)
        {
            if(data_array[count][loop]=='1')
            {
                tag_to_dec[count]+=(int)pow(2,(loop-2));
            }
        }
        #endif

        #if TRIGGER==2
        int times = index;
        int number=0;
        int start=Addressing_bus-1;
        while(times!=0)
        {
            if(data_array[count][start]=='1')
                index_to_dec[count]+=(int)(pow(2,(number)));
            number++;
            times--;
            start--;

        }

        number = 0;
        start = Addressing_bus-index-1;
        times = Addressing_bus-index-1;
        while(times!=0)
        {
            if(data_array[count][start]=='1')
                tag_to_dec[count]+=(int)(pow(2,(number)));
            number++;
            times--;
            start--;
        }
        #endif

    }
#if DEBUG==TEST_PRINT
    printf("Block_bits = %d\n",index);
    int out_loop,inner_loop;
    for(out_loop=1;out_loop<total_data;out_loop++)
    {
        for(inner_loop= 0;inner_loop<Addressing_bus;inner_loop++)
        {
            if(inner_loop==(Addressing_bus-index))
                printf(" ");
            printf("%c",data_array[out_loop][inner_loop]);
        }
        printf(" Index is %d and tag is %d\n",index_to_dec[out_loop],tag_to_dec[out_loop]);
        /*Recording index and tag data intp file*/
        fprintf(result_file,"Index is %d and tag is %d\n",index_to_dec[out_loop],tag_to_dec[out_loop]);
    }
    
#endif
    
    /*Direct Mapping memory
*   Using index to find where location having put tag into there.
*
*
* */
    for(count = 1;count<total_data;count++)
    {
        if(block_array[index_to_dec[count]]==INFINITE)
        {
            printf("Cool fault!\n");
            total_miss_times++;
            block_array[index_to_dec[count]] = tag_to_dec[count];
        }
        else
        {
            /*current data is not fits */
            if(block_array[index_to_dec[count]]!=tag_to_dec[count])
            {
                total_miss_times++;
                printf("miss happend! block index = %d, and data = %d our data=%d\t\n",index_to_dec[count],block_array[index_to_dec[count]],tag_to_dec[count]);

                fprintf(result_file,"miss happend! block index = %d, and data = %d our data=%d\t\n",index_to_dec[count],block_array[index_to_dec[count]],tag_to_dec[count]);

                block_array[index_to_dec[count]] = tag_to_dec[count];
            }
            else
            {

                printf("hit happend! block index = %d, and data = %d our data=%d\n",index_to_dec[count],block_array[index_to_dec[count]],tag_to_dec[count]);
                fprintf(result_file,"hit happend! block index = %d, and data = %d our data=%d\n",index_to_dec[count],block_array[index_to_dec[count]],tag_to_dec[count]);
                
                total_hit_times++;
            }
        }
    }
    printf("Total miss times = %d, and total hit times = %d\n",total_miss_times,total_hit_times); 

    fprintf(result_file,"Total miss times = %d, and total hit times = %d\n",total_miss_times,total_hit_times); 
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
    fclose(result_file);
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


