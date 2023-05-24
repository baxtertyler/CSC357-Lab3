#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define PAGESIZE 1024

typedef unsigned char byte;
typedef unsigned int uint;

typedef struct chunckhead {
    unsigned int size;
    unsigned int info;
    byte *next, *prev;
} chunkhead;

byte *mymalloc(uint);
void myfree(byte*);
void analyse();
void splitChunk(chunkhead* c, uint);

unsigned char myheap[1048576];
chunkhead* c0 = NULL;

int main() {
    return 0;
}

/*
    mymalloc function -> heart of the program. This
    will ensure the input chunk is of the right size
    (if not it will make it so that it is), splits the 
    chunk if there j
*/
byte *mymalloc(uint s) {
    chunkhead* cTemp = (chunkhead*)myheap;
    if(c0 == NULL) {
        c0 = (chunkhead*)myheap;
        c0->size = sizeof(myheap) - sizeof(chunkhead);
        c0->info = 0;
        c0->next = NULL; c0->prev = NULL;
        return (byte*)(c0+1);
    }
    if ((float)((s+sizeof(chunkhead)) % PAGESIZE) != 0) {
        s = (1 + ceil((float)((s+sizeof(chunkhead)) / PAGESIZE))) * PAGESIZE;
    }
    if(s < PAGESIZE) {
        s = PAGESIZE;
    }
    while (cTemp != NULL) {
        if (cTemp->size < s || cTemp->info == 1) {
            cTemp = ((chunkhead*)cTemp->next);
        } else { break; }
    }
    if (cTemp == NULL) {
        return NULL;
    }
    if (cTemp->size >= (s + PAGESIZE + sizeof(chunkhead))) {
        splitChunk(cTemp, s);
    }
    cTemp->info = 1;
    return (byte*)(cTemp+1);
}

/*
    frees the chunks that mymalloc stores. After, it 
    will combine the chunks so that there is a large 
    chunk of free memory rather than many chunks of 
    less memory
*/
void myfree(byte *addy) {
    chunkhead* cTemp = (chunkhead*)addy - 1;
    cTemp->info = 0;
    if (cTemp == c0) {
        c0 = NULL;
    }
    if (cTemp->prev != NULL && ((chunkhead*)cTemp->prev)->info == 0) {
        ((chunkhead*)cTemp->prev)->next = cTemp->next;
        if(cTemp->next != NULL) {
            ((chunkhead*)cTemp->next)->prev = cTemp->prev;
        }
        ((chunkhead*)cTemp->prev)->size += cTemp->size;
        ((chunkhead*)cTemp->prev)->size += sizeof(chunkhead);
        cTemp = ((chunkhead*)cTemp->prev);
    }
    if (cTemp->next != NULL && ((chunkhead*)cTemp->next)->info == 0) {
        cTemp->next += ((chunkhead*)cTemp->next)->size;
        cTemp->next += sizeof(chunkhead);
        cTemp->next = ((chunkhead*)cTemp->next)->next;
        if(cTemp->next != NULL) {
            ((chunkhead*)cTemp->next)->prev = (byte*)cTemp;
        }
    }
}

/*
    function to "see into" the memory
*/
void analyse() {
    chunkhead* cCurr = (chunkhead*)myheap;
    int cnt = 0;
    while (cCurr != NULL) {
        printf("Chunk #%d: \n", cnt++);
        printf("Size = %d bytes\n", cCurr->size);
        if (cCurr->info == 1) {
            printf("%s\n", "Occupied");
        } else {
            printf("%s\n", "Free");
        }
        printf("Next = %p\n", cCurr->next);
        printf("Prev = %p\n\n", cCurr->prev);
        cCurr = ((chunkhead*)cCurr->next);
    }
}


/*
    splits chunks 
*/
void splitChunk(chunkhead* c, uint s) {
    chunkhead* c1 = (chunkhead*)((byte*)c + s + sizeof(chunkhead));
    c1->size = c->size - (s + sizeof(chunkhead));
    c1->info = 0;
    c1->next = c->next;
    c1->prev = (byte*)c;
    if (c->next != NULL) {
        (((chunkhead*)c1->next)->prev) = (byte*)c1;
    }
    c->size = s;
    c->next = (byte*)c1;
}
