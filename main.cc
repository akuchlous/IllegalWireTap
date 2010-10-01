/* Problem : Illegal WireTaps 
   
   Synopsis : 
      1) Construct a matrix [26*26] 
           where Mij represent the cost of worker i doing the job j
      2) Apply cost minimization algorithm 
           (Hungarian/Munkres algorithm for the cost reduction)
*/

#include <stdio.h>
#include "matrix.hh"

/* main */
int main(int argc, char *argv[])
{
    if (argc != 2) {
	printf("Error:\n\t Usage : a.out <filename>\n");
	return -1;
    }
    Matrix m;
    if (m.Initialize(argv[1]) == -1) {
	printf("Error In initializing the Matrix\n");
	return -1;
    }
    m.Engine();
    printf("Minmum Cost = %f\n", m.GetMinimumCost());
}
