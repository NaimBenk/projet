#include <stdio.h>

int main(){
    for(int x=1; x<11; x++){
        for(int y=1; y<11; y++){
            int d = y+(x+y)*(x+y+1)/2;
            printf("%d ", d);
        }
        printf("\n");
    }
    return 0;
}