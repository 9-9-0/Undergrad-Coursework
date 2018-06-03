#include "../lib/oss/oss_funct.h"

void print_bits(int input);

int main() {
  uint test_bitmap[8];

  for (int i = 0; i < 8; i++) {
    test_bitmap[i] = 0;
  }

  for (int i = 0; i < 8; i++) {
    print_bits(test_bitmap[i]);
  }
  fprintf(stderr, "\n");

  SetBit(test_bitmap, 65);

  for (int i = 0; i < 8; i++) {
    print_bits(test_bitmap[i]);
  }
  fprintf(stderr, "\n");

  fprintf(stderr, "Testing bit 65: %d\n", TestBit(test_bitmap, 65));

  SetBit(test_bitmap, 0);

  for (int i = 0; i < 8; i++) {
    print_bits(test_bitmap[i]);
  }
  fprintf(stderr, "\n");

  ClearBit(test_bitmap, 65);

  for (int i = 0; i < 8; i++) {
    print_bits(test_bitmap[i]);
  }
  fprintf(stderr, "\n");

  fprintf(stderr, "Testing bit 65: %d\n", TestBit(test_bitmap, 65));
  
  return 0;
}

void print_bits(int input) {
  printf("%-10d : ", input);
  for (int i=31 ; i>=0 ; i--)
  {
    if(input & 1<< i) // num & 1 << position
      printf("1 ");
    else
      printf("0 ");
  }
  printf("\n");
}
