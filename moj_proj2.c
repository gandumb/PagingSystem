#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ----------------------------------------- Timestamp Function ----------------------------------------- //

void getTime(FILE *outputFile){
    struct timespec sysTime;
    long milisec;

    int err = clock_gettime(CLOCK_REALTIME, &sysTime);
    if (err){
        perror("Error getting timestamp");
        return;
    }

    struct tm* timeStamp = localtime(&sysTime.tv_sec);
    if (timeStamp == NULL) {
        perror("Error getting localtime");
        return;
    }

    char timeString[sizeof("1900-01-01 23:59:59")];
    timeString[strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeStamp)] = '\0';

    fprintf(outputFile, "%s.%03li", timeString, milisec);
}

// ----------------------------------------- 4 Frame ----------------------------------------- //

void setMatrix4(int matrix[4][100]){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 100; j++){
            matrix[i][j] = -1;
        }
    }
}

void FIFO4(int memory[4][100], FILE *inputFile, FILE *outputFile){

    printf("Starting fifo...\n");

    int faultCounter = 0;

    int y = 1;

    int page;

    fscanf(inputFile, "%d ", &page);
    //printf("Processing page %d...\n", page);
    fprintf(outputFile, "Page Referenced: %d at ", page);
    getTime(outputFile);
    fprintf(outputFile, " - Page Fault Occured!\n");

    memory[0][y] = page;
    faultCounter++;


    while (fscanf(inputFile, "%d ", &page) == 1) {
        //printf("Processing page %d...\n", page);

        fprintf(outputFile, "Page Referenced: %d at ", page);
        getTime(outputFile);

        y++;

        for (int i = 0; i < 4; i++){
            memory[i][y] = memory[i][y-1];
        }

        int pageFault = 1; // set to 0 if no page fault
        for (int i = 0; i < 4; i++){
            if (memory[i][y] == page){
                pageFault = 0;
                break;
            }
        }

        if (pageFault){

            faultCounter++;

            memory[3][y] = memory[2][y];
            memory[2][y] = memory[1][y];
            memory[1][y] = memory[0][y];
            memory[0][y] = page;

            fprintf(outputFile, " - Page Fault Occured!");
        }
        fprintf(outputFile, "\n");
    }

    printf("Finished fifo\n");
    fprintf(outputFile, "\nTotal Page Faults: %d\n\n\nEnd of FIFO Simulation\n\n-----", faultCounter);
}

void LRU4(int memory[4][100], FILE *inputFile, FILE *outputFile){
    printf("Starting LRU...\n");

    int faultCounter = 0;

    int recUsed[4] = {-1, -1, -1, -1}; // 0 index = most recently used page, index 3 = oldest used page
    int recUsedSize = 4;

    int y = 1;

    //printAllInfo4(memory, removedPages, pageFaultOccur);

    int page;

    fscanf(inputFile, "%d ", &page);
    fprintf(outputFile, "Page Referenced: %d at ", page);
    getTime(outputFile);
    fprintf(outputFile, " - Page Fault Occured!\n");

    faultCounter++;

    recUsed[0] = page;
    memory[0][y] = page;

    while (fscanf(inputFile, "%d ", &page) == 1) {
        fprintf(outputFile, "Page Referenced: %d at ", page);
        getTime(outputFile);
        //printf("Processing page %c...\n", page);

        // Add to the history
        int newValFound = 1; // dup val if set to 0

        // Resume normal operations
        y++;

        for (int i = 0; i < 4; i++){
            memory[i][y] = memory[i][y-1];
        }

        int pageFault = 1; // set to 0 if no page fault
        for (int i = 0; i < 4; i++){
            if (memory[i][y] == page){
                pageFault = 0;
                break;
            }
        }

        if (pageFault){
            fprintf(outputFile, " - Page Fault Occured!");
            faultCounter++;

            for (int i = 0; i < 4; i++){
                if (memory[i][y] == recUsed[3]) {
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

        fprintf(outputFile, "\n");

    }

    printf("Finished LRU\n");
    fprintf(outputFile, "\nTotal Page Faults: %d\n\n\nEnd of LRU Simulation\n\n-----", faultCounter);
}

void SC4(int memory[4][100], FILE *inputFile, FILE *outputFile){
    printf("Starting SC...\n");

    int faultCounter = 0;

    int scBits[4] = {0, 0, 0, 0};

    int y = 1;

    //printAllInfo4(memory, removedPages, pageFaultOccur);

    int page;

    fscanf(inputFile, "%d ", &page);
    fprintf(outputFile, "Page Referenced: %d at ", page);
    getTime(outputFile);
    fprintf(outputFile, " - Page Fault Occured!\n");

    faultCounter++;
    //printf("Processing page %c...\n", page);

    memory[0][y] = page;

    //printAllInfo4(memory, removedPages, pageFaultOccur);

    while (fscanf(inputFile, "%d ", &page) == 1) {
        fprintf(outputFile, "Page Referenced: %d at ", page);
        getTime(outputFile);
        //printf("Processing page %c...\n", page);

        y++;

        for (int i = 0; i < 4; i++){
            memory[i][y] = memory[i][y-1];
        }

        int pageFault = 1; // set to 0 if no page fault
        for (int i = 0; i < 4; i++){
            if (memory[i][y] == page){
                pageFault = 0;
                break;
            }
        }

        if (pageFault){

            int xValExist = 1; // set to 0 if x value exists in the frame
            for (int i = 0; i < 4; i++){
                if (memory[i][y] == -1){
                    memory[i][y] = page;
                    xValExist = 0;
                    break;
                }
            }

            faultCounter++;
            fprintf(outputFile, " - Page Fault Occured!");

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
                    memory[0][y] = page;
                }
            }
        }
        
        //printAllInfo4(memory, removedPages, pageFaultOccur);
        fprintf(outputFile, "\n");
    }

    printf("Finished SC\n");
    fprintf(outputFile, "\nTotal Page Faults: %d\n\n\nEnd of SC Simulation\n\n-----", faultCounter);
}

// ----------------------------------------- 8 Frame ----------------------------------------- //

void setMatrix8(int matrix[8][100]){
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 100; j++){
            matrix[i][j] = -1;
        }
    }
}

void FIFO8(int memory[8][100], FILE *inputFile, FILE *outputFile){

    printf("Starting fifo...\n");

    int faultCounter = 0;

    int y = 1;

    int page;

    fscanf(inputFile, "%d ", &page);
    //printf("Processing page %d...\n", page);
    fprintf(outputFile, "Page Referenced: %d at ", page);
    getTime(outputFile);
    fprintf(outputFile, " - Page Fault Occured!\n");

    memory[0][y] = page;
    faultCounter++;


    while (fscanf(inputFile, "%d ", &page) == 1) {
        //printf("Processing page %d...\n", page);

        fprintf(outputFile, "Page Referenced: %d at ", page);
        getTime(outputFile);

        y++;

        for (int i = 0; i < 8; i++){
            memory[i][y] = memory[i][y-1];
        }

        int pageFault = 1; // set to 0 if no page fault
        for (int i = 0; i < 8; i++){
            if (memory[i][y] == page){
                pageFault = 0;
                break;
            }
        }

        if (pageFault){

            faultCounter++;

            // TODO
            memory[7][y] = memory[6][y];
            memory[6][y] = memory[5][y];
            memory[5][y] = memory[4][y];
            memory[4][y] = memory[3][y];
            memory[3][y] = memory[2][y];
            memory[2][y] = memory[1][y];
            memory[1][y] = memory[0][y];
            memory[0][y] = page;

            fprintf(outputFile, " - Page Fault Occured!");
        }
        fprintf(outputFile, "\n");
    }

    printf("Finished fifo\n");
    fprintf(outputFile, "\nTotal Page Faults: %d\n\n\nEnd of FIFO Simulation\n\n-----", faultCounter);
}

void LRU8(int memory[8][100], FILE *inputFile, FILE *outputFile){
    printf("Starting LRU...\n");

    int faultCounter = 0;

    int recUsed[8] = {-1, -1, -1, -1, -1, -1, -1, -1}; // 0 index = most recently used page, index 3 = oldest used page
    int recUsedSize = 8;

    int y = 1;

    //printAllInfo4(memory, removedPages, pageFaultOccur);

    int page;

    fscanf(inputFile, "%d ", &page);
    fprintf(outputFile, "Page Referenced: %d at ", page);
    getTime(outputFile);
    fprintf(outputFile, " - Page Fault Occured!\n");

    faultCounter++;

    recUsed[0] = page;
    memory[0][y] = page;

    while (fscanf(inputFile, "%d ", &page) == 1) {
        fprintf(outputFile, "Page Referenced: %d at ", page);
        getTime(outputFile);
        //printf("Processing page %c...\n", page);

        // Add to the history
        int newValFound = 1; // dup val if set to 0

        // Resume normal operations
        y++;

        for (int i = 0; i < 8; i++){
            memory[i][y] = memory[i][y-1];
        }

        int pageFault = 1; // set to 0 if no page fault
        for (int i = 0; i < 8; i++){
            if (memory[i][y] == page){
                pageFault = 0;
                break;
            }
        }

        if (pageFault){
            fprintf(outputFile, " - Page Fault Occured!");
            faultCounter++;

            for (int i = 0; i < 8; i++){
                if (memory[i][y] == recUsed[7]) {
                    //printf("Inserted Page = %c\n", page);

                    memory[i][y] = page;

                    break;
                }
            }
        }

        int dupPageDetected = 0;
        int dupIndex;
        int dupPage;

        for (int i = 0; i < 8; i++){
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
            recUsed[7] = recUsed[6];
            recUsed[6] = recUsed[5];
            recUsed[5] = recUsed[4];
            recUsed[4] = recUsed[3];
            recUsed[3] = recUsed[2];
            recUsed[2] = recUsed[1];
            recUsed[1] = recUsed[0];
            recUsed[0] = page;
        }

        fprintf(outputFile, "\n");

    }

    printf("Finished LRU\n");
    fprintf(outputFile, "\nTotal Page Faults: %d\n\n\nEnd of LRU Simulation\n\n-----", faultCounter);
}

void SC8(int memory[8][100], FILE *inputFile, FILE *outputFile){
    printf("Starting SC...\n");

    int faultCounter = 0;

    int scBits[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    int y = 1;

    //printAllInfo4(memory, removedPages, pageFaultOccur);

    int page;

    fscanf(inputFile, "%d ", &page);
    fprintf(outputFile, "Page Referenced: %d at ", page);
    getTime(outputFile);
    fprintf(outputFile, " - Page Fault Occured!\n");

    faultCounter++;
    //printf("Processing page %c...\n", page);

    memory[0][y] = page;

    //printAllInfo4(memory, removedPages, pageFaultOccur);

    while (fscanf(inputFile, "%d ", &page) == 1) {
        fprintf(outputFile, "Page Referenced: %d at ", page);
        getTime(outputFile);
        //printf("Processing page %c...\n", page);

        y++;

        for (int i = 0; i < 8; i++){
            memory[i][y] = memory[i][y-1];
        }

        int pageFault = 1; // set to 0 if no page fault
        for (int i = 0; i < 8; i++){
            if (memory[i][y] == page){
                pageFault = 0;
                break;
            }
        }

        if (pageFault){

            int xValExist = 1; // set to 0 if x value exists in the frame
            for (int i = 0; i < 8; i++){
                if (memory[i][y] == -1){
                    memory[i][y] = page;
                    xValExist = 0;
                    break;
                }
            }

            faultCounter++;
            fprintf(outputFile, " - Page Fault Occured!");

            if (xValExist){
                int dupVal = 1; // set to 0 if duplicate value found

                for (int i = 0; i < 8; i++) {
                    if (page == memory[i][y]){
                        scBits[i] = 1;
                        dupVal = 0;
                        break;
                    }
                }


                int allHaveSC = 1;

                if (dupVal) {
                    for(int i = 0; i < 8; i++){
                        if (scBits[i] == 0){

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
                    memory[0][y] = page;
                }
            }
        }
        
        //printAllInfo4(memory, removedPages, pageFaultOccur);
        fprintf(outputFile, "\n");
    }

    printf("Finished SC\n");
    fprintf(outputFile, "\nTotal Page Faults: %d\n\n\nEnd of SC Simulation\n\n-----", faultCounter);
}

// ----------------------------------------- Main ----------------------------------------- //

//Prompt user for page replacement algorithm (FIFO, LRU, Second Chance)
int main() {

    char type[100] = "";
    printf( "Which page replacement algorithm do you want to use: FIFO, LRU, Second Chance (SC) >>> ");
    scanf("%s", type);
    //printf("%s", type);

    char frameRequest[100] = "";
    printf( "Four (Enter a 4) or Eight (Enter an 8) page frames? >>> ");
    scanf("%s", frameRequest);
    //printf("%s", frameRequest);

    FILE *outputFile;

    if (strcmp("4", frameRequest) == 0) {
        outputFile = fopen("moj_proj2_output_4_frames.txt", "a+");
        
    }
    else if (strcmp("8", frameRequest) == 0) {
        outputFile = fopen("moj_proj2_output_8_frames.txt", "a+");
    }
    else {
        printf("Given page frame size is not 4 or 8...");
        return 0;
    }

    fprintf(outputFile, "Page Replace Algo: %s - Page Frame Size: %s - Timestamp: ", type, frameRequest);
    getTime(outputFile);

    int memory4[4][100];
    setMatrix4(memory4);

    int memory8[8][100];
    setMatrix8(memory8);

    FILE *inputFile = fopen("moj_proj2_input.txt", "w+");

    if (inputFile == NULL){
            printf("Error creating or opening file\n");
            return 0;
    }
    else {
        int randNum;
        srand(time(0));

        for (int i = 0; i < 100; i++) {
            randNum = rand() % (16);
            //printf("%d\n", randNum);
            fprintf(inputFile, "%d ", randNum);
        }
    }

    fclose(inputFile);

    inputFile = fopen("moj_proj2_input.txt", "r");

    fprintf(outputFile, "\n\n");

    // fclose(inputFile);
    // fclose(outputFile);
    // return 0;

    char page;

    if (strcmp("FIFO", type) == 0) {
        printf("Using FIFO Page Replacement Algorithm...\n");

        if (strcmp("4", frameRequest) == 0) {
            FIFO4(memory4, inputFile, outputFile);
        }
        else if (strcmp("8", frameRequest) == 0) {
            FIFO8(memory8, inputFile, outputFile);
        }
    }
    else if (strcmp("LRU", type) == 0) {
        printf("Using LRU Page Replacement Algorithm...\n");

        if (strcmp("4", frameRequest) == 0) {
            LRU4(memory4, inputFile, outputFile);
        }
        else if (strcmp("8", frameRequest) == 0) {
            LRU8(memory8, inputFile, outputFile);
        }
    }
    else if (strcmp("SC", type) == 0) {
        printf("Using SC Page Replacement Algorithm...\n");

        if (strcmp("4", frameRequest) == 0) {
            SC4(memory4, inputFile, outputFile);
        }
        else if (strcmp("8", frameRequest) == 0) {
            SC8(memory8, inputFile, outputFile);
        }
    }
    else {
        printf("The given page replacement algorithm is incorrect or not supported...\n");
    }

    fprintf(outputFile, "\n\n");
    
    fclose(inputFile);
    fclose(outputFile);

    return 0;
}