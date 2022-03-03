#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setMatrix(char matrix[4][25]){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 25; j++){
            matrix[i][j] = 'x';
        }
    }
}

void printMatrix(char matrix[4][25]){
    //size is always the same

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 25; j++){
            printf("%c ", matrix[i][j]);
        }
        printf("\n");
    }
}

void printPageFaultOccur(char pageFaultOccur[25]){
    for (int i = 0; i < 25; i++){
        printf("%c ", pageFaultOccur[i]);
    }
    printf("\n");
}

void printAllInfo(char memory[4][25], char removedPages[4][25], char pageFaultOccur[25]){
    printf("Loaded Memory:\n");
    printMatrix(memory);
    printf("-----------------------\n");
    printf("Page Fault Occurences:\n");
    printPageFaultOccur(pageFaultOccur);
    printf("-----------------------\n");
    printf("Removed Pages:\n");
    printMatrix(removedPages);
    printf("-----------------------\n");
    printf("\n");
}

void FIFO(char memory[4][25], char removedPages[4][25], FILE *inputFile){
    char pageFaultOccur[25] = {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'};

    int y = 1;

    printAllInfo(memory, removedPages, pageFaultOccur);

    char page;

    fscanf(inputFile, " %c", &page);
    printf("Processing page %c...\n", page);

    memory[0][y] = page;
    pageFaultOccur[y] = 'F';

    printAllInfo(memory, removedPages, pageFaultOccur);

    while (fscanf(inputFile, " %c", &page) == 1) {
        printf("Processing page %c...\n", page);

        y++;

        for (int i = 0; i < 4; i++){
            memory[i][y] = memory[i][y-1];
            removedPages[i][y] = removedPages[i][y-1];
        }

        int pageFault = 1; // set to 0 if no page fault
        for (int i = 0; i < 4; i++){
            if (memory[i][y] == page){
                pageFault = 0;
                break;
            }
        }

        if (pageFault){
            pageFaultOccur[y] = 'F';

            removedPages[3][y] = removedPages[2][y];
            removedPages[2][y] = removedPages[1][y];
            removedPages[1][y] = removedPages[0][y];
            removedPages[0][y] = memory[3][y];

            memory[3][y] = memory[2][y];
            memory[2][y] = memory[1][y];
            memory[1][y] = memory[0][y];
            memory[0][y] = page;
        }

        printAllInfo(memory, removedPages, pageFaultOccur);
    }
}

void LRU(char memory[4][25], char removedPages[4][25], FILE *inputFile){
    char pageFaultOccur[25] = {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'};

    char recUsed[4] = {'x', 'x', 'x', 'x'}; // 0 index = most recently used page, index 3 = oldest used page
    int recUsedSize = 4;

    int y = 1;

    printAllInfo(memory, removedPages, pageFaultOccur);

    char page;

    fscanf(inputFile, " %c", &page);
    printf("Processing page %c...\n", page);

    pageFaultOccur[y] = 'F';
    recUsed[0] = page;

    memory[0][y] = page;

    printAllInfo(memory, removedPages, pageFaultOccur);

    while (fscanf(inputFile, " %c", &page) == 1) {
        printf("Processing page %c...\n", page);

        // Add to the history
        int newValFound = 1; // dup val if set to 0

        // Resume normal operations
        y++;

        for (int i = 0; i < 4; i++){
            memory[i][y] = memory[i][y-1];
            removedPages[i][y] = removedPages[i][y-1];
        }

        int pageFault = 1; // set to 0 if no page fault
        for (int i = 0; i < 4; i++){
            if (memory[i][y] == page){
                pageFault = 0;
                break;
            }
        }

        if (pageFault){
            pageFaultOccur[y] = 'F';

            for (int i = 0; i < 4; i++){
                if (memory[i][y] == recUsed[3]) {
                    removedPages[3][y] = removedPages[2][y];
                    removedPages[2][y] = removedPages[1][y];
                    removedPages[1][y] = removedPages[0][y];
                    removedPages[0][y] = memory[i][y];

                    //printf("Inserted Page = %c\n", page);

                    memory[i][y] = page;

                    break;
                }
            }
        }

        int dupPageDetected = 0;
        int dupIndex;
        int dupPage;

        for (int i = 0; i < 4; i++){
            if (recUsed[i] == page) {
                dupPageDetected = 1;
                dupIndex = i;
                dupPage = recUsed[i];
                //printf("Dup Index = %d\n", dupIndex);
                break;
            }
        }
        if (dupPageDetected){
            for (int i = dupIndex; i > 0; i--){
                recUsed[i] = recUsed[i-1];
                //printf("Swap = %c\n", recUsed[i-1]);
            }
            recUsed[0] = page;
        }
        else {
            recUsed[3] = recUsed[2];
            recUsed[2] = recUsed[1];
            recUsed[1] = recUsed[0];
            recUsed[0] = page;
        }

        //printf("Index 0 = %c\n", recUsed[0]);
        //printf("Index 1 = %c\n", recUsed[1]);
        //printf("Index 2 = %c\n", recUsed[2]);
        //printf("Index 3 = %c\n", recUsed[3]);

        printAllInfo(memory, removedPages, pageFaultOccur);
    }
}

void SC(char memory[4][25], char removedPages[4][25], FILE *inputFile){
    char pageFaultOccur[25] = {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'};

    int scBits[4] = {0, 0, 0, 0};

    int y = 1;

    printAllInfo(memory, removedPages, pageFaultOccur);

    char page;

    fscanf(inputFile, " %c", &page);
    printf("Processing page %c...\n", page);

    memory[0][y] = page;
    pageFaultOccur[y] = 'F';

    printAllInfo(memory, removedPages, pageFaultOccur);

    while (fscanf(inputFile, " %c", &page) == 1) {
        printf("Processing page %c...\n", page);

        y++;

        for (int i = 0; i < 4; i++){
            memory[i][y] = memory[i][y-1];
            removedPages[i][y] = removedPages[i][y-1];
        }

        int pageFault = 1; // set to 0 if no page fault
        for (int i = 0; i < 4; i++){
            if (memory[i][y] == page){
                pageFault = 0;
                break;
            }
        }

        if (pageFault){
            pageFaultOccur[y] = 'F';

            int xValExist = 1; // set to 0 if x value exists in the frame
            for (int i = 0; i < 4; i++){
                if (memory[i][y] == 'x'){
                    memory[i][y] = page;
                    xValExist = 0;
                    break;
                }
            }

            if (xValExist){
                int dupVal = 1; // set to 0 if duplicate value found

                for (int i = 0; i < 4; i++) {
                    if (page == memory[i][y]){
                        scBits[i] = 1;
                        dupVal = 0;
                        break;
                    }
                }


                int allHaveSC = 1;

                if (dupVal) {
                    for(int i = 0; i < 4; i++){
                        if (scBits[i] == 0){
                            removedPages[3][y] = removedPages[2][y];
                            removedPages[2][y] = removedPages[1][y];
                            removedPages[1][y] = removedPages[0][y];
                            removedPages[0][y] = memory[i][y];

                            memory[i][y] = page;
                            allHaveSC = 0;
                            break;
                        }
                        else {
                            scBits[i] = 0;
                        }
                    }
                }

                if (allHaveSC){
                    //printf("LALALALALALALALALALALLA");
                    removedPages[3][y] = removedPages[2][y];
                    removedPages[2][y] = removedPages[1][y];
                    removedPages[1][y] = removedPages[0][y];
                    removedPages[0][y] = memory[0][y];

                    memory[0][y] = page;
                }
            }
        }
        
        printAllInfo(memory, removedPages, pageFaultOccur);
    }
}

//Prompt user for page replacement algorithm (FIFO, LRU, Second Chance)
int main() {

    char type[100] = "";

   printf( "Which page replacement algorithm do you want to use: FIFO, LRU, Second Chance (SC) >>> ");
   scanf("%s", type);

    printf("%s\n", type);

    char memory[4][25];
    char removedPages[4][25];

    setMatrix(memory);
    setMatrix(removedPages);

    FILE *inputFile = fopen("referencePage.txt", "r");

    if (inputFile == NULL){
            printf("Error1\n");
            return 0;
    }

    int result;

    if (strcmp("FIFO", type) == 0) {
        printf("Using FIFO Page Replacement Algorithm...\n");

        FIFO(memory, removedPages, inputFile);
    }
    else if (strcmp("LRU", type) == 0) {
        printf("Using LRU Page Replacement Algorithm...\n");

        LRU(memory, removedPages, inputFile);
    }
    else if (strcmp("SC", type) == 0) {
        printf("Using SC Page Replacement Algorithm...\n");

        SC(memory, removedPages, inputFile);
    }
    else {
        printf("Bad input...\n");
    }

    fclose(inputFile);

    return 0;
}

//Read a number delimited by spaces from the reference page file

//Create a "memory" space of four pages of memory, with 4 pages of most recently removed from memory below it

//Apply the page replacement algorithm to the reference string data

//Display memory at each time a page is loaded or removed

//Show an indcator below the column where page faults occured

//Load the page into memory