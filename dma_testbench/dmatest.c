#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

unsigned long tStart, tEnd;
unsigned long data;

unsigned long getTime(){
    struct timeval temp;

    gettimeofday(&temp, NULL);

    return temp.tv_sec * 1000 * 1000 + temp.tv_usec;
}

void report(char *msg, unsigned long data, unsigned long time, unsigned long dmaUsed){
    printf("%s\t%ld\t%ld\t%f\t%d\n", msg, data, time, data * 1.0 / time, dmaUsed);
    FILE *f = fopen("report.dat", "a");
    fprintf(f, "%s\t%ld\t%ld\t%f\t%d\n", msg, data, time, data * 1.0 / time, dmaUsed);
    fclose(f);
}

#define REPORT(f, timeStart, timeEnd, dataPtr, msg, dmaUsed) *timeStart = getTime(); *dataPtr = f; *timeEnd = getTime(); report(msg, *dataPtr, *timeEnd - *timeStart, dmaUsed);

void checkData(char *bufferIn, char *bufferOut, unsigned int elems){
    int i;

    if(!memcmp(bufferIn, bufferOut, elems*sizeof(char))){
        printf("DMA Ok!\n");
    }
    else{
        for(i=0;i<elems;i++)
            printf("%d\t%d\t%d\t%d\n", i, bufferIn[i], bufferOut[i], (i==0 ? 0 : bufferOut[i] - bufferOut[i-1]));
    }
}

unsigned long memCpy_ARM(char *bufferIn, char *bufferOut, unsigned long elems, int size){
    int i;

    for(i=0; i<elems; i++)
        bufferOut[i] = bufferIn[i];

    return elems * size;
}

unsigned long memCpy_DMA(char *bufferIn, char *bufferOut, unsigned long elems, int size, int dmaToUse){

#define FIFO_LEN 4000
#define DMA_NUM 1

    int fd[DMA_NUM];

    fd[0] = open("/dev/axidma", O_RDWR);
    printf("%d\n",fd[0]);


    unsigned long byteMoved = 0;
    unsigned long byteToMove = 0;

    int i;

    while(byteMoved!=size * elems){
        byteToMove = size * elems - byteMoved > FIFO_LEN ? FIFO_LEN : size * elems - byteMoved;

        for(i=0; i<dmaToUse; i++){
            write(fd[i], &bufferIn[byteMoved], byteToMove);
        }

        for(i=0; i<dmaToUse; i++)
            read(fd[i], &bufferOut[byteMoved], byteToMove);

        byteMoved += byteToMove;
    }

    close(fd[0]);


    return elems * size * dmaToUse;
}

int main(int argc, char **argv)
{
    char *bufferIn, *bufferOut_ARM, *bufferOut_DMA;

    if(argc!=3){
        printf("Usage: ./dmaBench DATA DMA_TO_USE\n");
        exit(0);
    }

    unsigned long DATA = atoi(argv[1]);
    unsigned int DMA_TO_USE = atoi(argv[2]);

    bufferIn = (char *) malloc(sizeof(char) * DATA);
    bufferOut_ARM = (char *) malloc(sizeof(char) * DATA);
    bufferOut_DMA = (char *) malloc(sizeof(char) * DATA);

    int i;

    for(i=0; i<DATA; i++){
        bufferIn[i] = i;
    }

    memset(bufferOut_ARM, 0, sizeof(char) * DATA);
    memset(bufferOut_DMA, 0, sizeof(char) * DATA);

    REPORT(memCpy_ARM(bufferIn, bufferOut_ARM, DATA, sizeof(char)), &tStart, &tEnd, &data, "ARM", 0);
    for(i=0; i<DMA_TO_USE; i++){
        REPORT(memCpy_DMA(bufferIn, bufferOut_DMA, DATA/(i+1), sizeof(char), (i+1)), &tStart, &tEnd, &data, "DMA", (i+1));
    }

    checkData(bufferIn, bufferOut_ARM, DATA);
    checkData(bufferIn, bufferOut_DMA, DATA);

    return 0;
}
