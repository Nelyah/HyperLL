#include <stdio.h>
#include <math.h>
#include <inttypes.h>

uint64_t lastBits(int n, uint64_t val){
// returns the last n bits of val
// val needs to be unsigned int 64bits
    uint64_t mask;
    mask = ((uint64_t)(1) << n) - 1;
    uint64_t returnVal = val & mask;
    return returnVal;
}

int main(int argc, const char **argv) {
    uint64_t a  = pow(2,42)-1;
    uint64_t b  = pow(2,31);
//    a = a << ;
    
    printf("%" PRIu64 "\n",a);
    printf("clz %d\n",__builtin_clz(a));
    printf("%" PRIu64 "\n",b);

    printf("last %" PRIu64 "\n", lastBits(31,a));

    FILE* f = fopen("plot.txt","r");
    char  h[50],g[50],c[50],d[50],e[50];
    float t1, t2, t3, t4, t5;
    fscanf(f,"%f %f %f %f %f",&t1,&t2,&t3,&t4,&t5);
    printf("%f %f \n",t1,t2);
    fscanf(f,"%f %f %f %f %f",&t1,&t2,&t3,&t4,&t5);
    printf("%f %f \n",t1,t2);




    return 0;
}
