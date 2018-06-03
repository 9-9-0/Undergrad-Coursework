#include <stdio.h>
#include "../lib/bitmap.h"

void print_bits(int input);
int get_free_ind(int bitmap);

int bit_map;

int main()
{
  bit_map = 0;
  SetBit(bit_map, 2);
  print_bits(bit_map);

  SetBit(bit_map, 3);
  print_bits(bit_map);

  SetBit(bit_map, 18);
  print_bits(bit_map);

  ClearBit(bit_map, 3);
  print_bits(bit_map);

  int test2 = get_free_ind(bit_map);
  printf("First free index: %d\n", test2);

  SetBit(bit_map,0);
  test2 = get_free_ind(bit_map);
  printf("First free index: %d\n", test2);

  SetBit(bit_map,1);
  SetBit(bit_map,3);
  test2 = get_free_ind(bit_map);
  printf("First free index: %d\n", test2);

  int test = TestBit(bit_map, 3);
  printf("%d\n", test);

  test = TestBit(bit_map, 2);
  printf("%d\n", test);

  test = TestBit(bit_map, 18);
  printf("%d\n", test);

  ClearAllBits(bit_map);
  print_bits(bit_map);

  for (int i = 0; i < 19; i++) {
    SetBit(bit_map, i);
    //print_bits(bit_map);
  }

  printf("%d\n", bit_map); //All 19 child processes in use, value is 524287
}

int get_free_ind(int bitmap) {
  //Can swap this out for a mod operation.
  for (int i = 0; i < 19; i++) {
    if (!TestBit(bitmap, i)) {
      return i;
    }
  }
  return -1; //This should never return as at least 1 should exist.
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
