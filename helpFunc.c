#include <stdio.h>
#include <string.h>

char upCase(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 'A';
    }
    return c;
}
int compareStr(char sentences[] , char word[], int lenWord) {
    for (int i=0; i<lenWord; i++) {
        if (upCase(sentences[i]) != upCase(word[i])) {
            return 0;
        }
    }
    return 1;//1 = true 
}