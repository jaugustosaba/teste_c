#include <stdio.h>
#include <stdlib.h>

#define LINHAS  3
#define COLUNAS 5
#define COUNT   5


int **cria_slot() {
    int** slot = malloc (sizeof(int *) * LINHAS);
    for (int i=0; i < LINHAS; i++)
        slot[i] = malloc (sizeof (int) * COLUNAS);
  
    for (int i=0; i < LINHAS; i++) {
        printf("Insira os valores da linha %d:\n", (i + 1));
        for (int j=0; j < COLUNAS; j++)
            scanf("%d", &slot[i][j]);
    }

    return slot;
}


int main(void) {
    int premio[LINHAS][COLUNAS] =  {
        {1,0,0,0,1},
        {0,1,0,1,0},
        {0,0,1,0,0},
    };

    int count_prize;
    do {
        count_prize = 0;
        int** slot = cria_slot();

        int comp = slot[0][0];
        for (int i=0; i < LINHAS; i++) {
            for (int j=0; j < COLUNAS; j++) {
                if (comp == slot[i][j] && premio[i][j]) 
                    count_prize++;
            }
        }

        for (int i=0; i < LINHAS; i++)
            free (slot[i]);
        free(slot);
    } while (count_prize != COUNT);

    printf ("Ganhou!\n");
    return 0;
}