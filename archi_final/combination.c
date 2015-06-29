/*************************************************************************
	> File Name: combination.c
	> Author: watchlee
	> Mail: windvergil@gmail.com
	> Created Time: Sun Jun 28 21:52:53 2015
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <stdbool.h>


bool next_comb(int* comb, const int n, const int k) {
	int i = k - 1;
	const int e = n - k;
	do
    {
		comb[i]++;
    }
	while (comb[i] > e + i && i--);

	if (comb[0] > e)
		return 0;
	while (++i < k)
		comb[i] = comb[i - 1] + 1;
	return 1;
}
int main() {
	int n, k;
    scanf("%d%d",&n,&k);
	int* comb = (int*)malloc(n * sizeof(int));
    int i;
	for (int i = 0; i < k; i++)
		comb[i] = i;
	do {
		for (int i = 0; i < k; i++)
        {
            printf("%d",comb[i]);
            if(i+1<k)
            {
             printf(",");   
            }
            else
            {
                printf("\n");
            }
        }   
	} while (next_comb(comb, n, k));
	free(comb);
	return 0;
}
