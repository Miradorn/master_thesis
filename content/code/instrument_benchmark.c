#include <stdio.h>

int intermediate = 0;

int inc(int input) {
  return input + 1;
}

void add(int input) {
  // Instrumentation Point
  intermediate += input;
}

int main() {
  for(int i = 0; i < 100000000; i++) {
    int result = inc(i);
    if(result % 100 == 0) {
      add(result);
    }
  }
  printf("%i", intermediate);
  return 0;
}
