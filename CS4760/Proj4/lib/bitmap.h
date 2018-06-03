#define SetBit(A,k)     ( A |= (1 << (k%32)) )
#define ClearBit(A,k)   ( A &= ~(1 << (k%32)) )
#define TestBit(A,k)    ( A & (1 << (k%32)) )
#define ClearAllBits(A) ( A = 0)
