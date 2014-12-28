#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFSIZE 15

pid_t pid1;

int main(int argc, const char **argv) {
    int a = 8;  // 101
    int rc;
    char buff[BUFSIZE];

    char* bu = malloc(BUFSIZE*2*sizeof(char));

uint32_t  mask;
mask = (1 << 2) - 1;
uint32_t lastXbits = a & mask;
printf("last : %d\n",lastXbits);
        printf("%d\n",a);
        a = a >> 1;
        printf("%d\n",a);
        a = a << 2;
        printf("%d\n",a);

        printf("%d\n", __builtin_clz(a));
        while((rc = scanf("%s\n",buff)) != EOF){
            printf("%s\n",buff);
        }
        rewind(stdin);
        printf("coucou\n");
        while((bu = gets(buff))){
            printf("%s\n",bu);
        }
        printf("%f\n",pow(2,-1));
    return 0;
}
