#include <stdio.h>

int main() {

  #pragma omp parallel
  printf("hello World\n");
  printf("Done\n");
}
