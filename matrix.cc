#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "matrix.hh"
#include "utils.hh"

#define TRACE_FLOW		//   printf("Exit %s\n", __func__)
#define ROW_COVER(x) mask[x][0]
#define COL_COVER(x) mask[0][x]
typedef double TYPE;

int Matrix::Initialize(const char *fileName)
{
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) {
	printf("Error: could not open the file\n");
	return -1;
    }
    bzero(grid, (SIZE + 1) * (SIZE + 1) * sizeof(TYPE));
    bzero(mask, (SIZE + 1) * (SIZE + 1) * sizeof(TYPE));
    srandom(time(NULL));
    for (int wireTapNo = 1; wireTapNo <= TOTAL_AGENT; wireTapNo++) {
	fillCostOfTaskForEachAgent(fp, grid[wireTapNo]);
    }
    memcpy(origGrid, grid, (SIZE + 1) * (SIZE + 1) * sizeof(TYPE));
    fclose(fp);
    nextStep = Step1;
    numRows = SIZE;
    numCols = SIZE;
    rowFromStep4 = -1;
    colFromStep4 = -1;
    TRACE_FLOW;
    // PrintMatrix();
    return 0;
}

void Matrix::PrintMatrix()
{
    printf(" New Matrix\n");
    for (int i = 1; i <= SIZE; i++) {
	printf("\t   {  ");
	for (int j = 1; j <= SIZE; j++) {
	    printf(FORMAT, grid[i][j]);
	}
	printf("}\n");
    }
}

void Matrix::PrintMask()
{
    printf(" Mask \n");
    for (int i = 0; i <= SIZE; i++) {
	printf("\t");
	for (int j = 0; j <= SIZE; j++) {
	    if (i == 0 && j == 0) {
		printf("   ");
		// do nuthin
	    } else if (i == 0 || j == 0) {
		printf("%s  ", mask[i][j] ? "C" : "U");
		if (j == 0)
		    printf("|");
	    } else {
		printf("%s  ",
		       mask[i][j] ? mask[i][j] ==
		       StarredZero ? "S" : "P" : "N");
	    }
	    if (j == 0) {
		printf("  ");
	    }
	}
	if (i == 0) {
	    printf("\n");
	    printf("\t    ---------------");
	}
	printf("\n");
    }
}

/*
Step 1:  For each row of the matrix, find the smallest element.
         Subtract it from every element in its row.  Go to Step 2.
*/
void Matrix::doStep1(void)
{

    for (int i = 1; i <= numRows; i++) {
	TYPE minRowVal = grid[i][1];
	for (int j = 1; j <= numCols; j++) {
	    minRowVal = (grid[i][j] < minRowVal) ? grid[i][j] : minRowVal;
	}
	for (int j = 1; j <= numCols; j++) {
	    grid[i][j] -= minRowVal;
	}
    }
    TRACE_FLOW;
    // PrintMatrix();
    nextStep = Step2;
    return;
}

/* 
Step 2:  Find a zero (Z) in the resulting matrix.  
         If there is no starred zero in its row or column, star Z. 
         Repeat for each element in the matrix. Go to Step 3.
*/
void Matrix::doStep2(void)
{
    for (int row = 1; row <= numRows; row++) {
	for (int col = 1; col <= numCols; col++) {
	    if ((grid[row][col] == 0) &&
		(ROW_COVER(row) == NormalZero) &&
		(COL_COVER(col) == NormalZero)) {
		mask[row][col] = StarredZero;
		COL_COVER(col) = Covered;
		ROW_COVER(row) = Covered;
	    }
	}
    }
    // PrintMask();
    // printf("%s reset\n", __func__);
    for (int row = 0; row <= numRows; row++) {
	for (int col = 0; col <= numCols; col++) {
	    if (row == 0 || col == 0)
		mask[row][col] = UnCovered;
	}
    }
    // PrintMask();
    nextStep = Step3;
    TRACE_FLOW;
    return;
}

/* 
Step 3:  
    a) Cover each column containing a starred zero.  
    b) If K columns are covered, the starred zeros describe a 
       complete set of unique assignments.  
       In this case, 
           Go to DONE, 
       otherwise, 
           Go to Step 4. 
*/
void Matrix::doStep3(void)
{
    for (int row = 1; row <= numRows; row++) {
	for (int col = 1; col <= numCols; col++) {
	    if (mask[row][col] == StarredZero)
		COL_COVER(col) = Covered;
	}
    }
    int colCovered = 0;
    for (int col = 1; col <= numCols; col++) {
	if (COL_COVER(col) == Covered) {
	    ++colCovered;
	}
    }
    // PrintMask();
    nextStep = (colCovered == numCols) ? AllDone : Step4;
    // printf("colms covered = %d\n", colCovered);
    TRACE_FLOW;
    return;
}

/*
Find a noncovered zero and prime it.  findUnCoveredZero
B
*/
int Matrix::findUnCoveredZero(int *row, int *col)
{
    int foundUncovered = -1;
    *row = -1;
    *col = -1;
    for (int r = 1; r <= numRows; r++) {
	for (int c = 1; c <= numCols; c++) {
	    if ((ROW_COVER(r) == Covered) ||
		(COL_COVER(c) == Covered) || (mask[r][c] == StarredZero)) {
		continue;
	    }
	    if (grid[r][c] == 0) {
		*row = r;
		*col = c;
		// printf("Uncovered[%d][%d]\n", *row, *col);
		foundUncovered = 0;
		return foundUncovered;
	    }
	}
    }
    return foundUncovered;
}

/* find star in the same row */
int Matrix::starInRow(int row)
{
    int retVal = 0;
    for (int col = 1; col <= numCols; col++) {
	if (mask[row][col] == StarredZero) {
	    retVal = col;
	    break;
	}
    }
    // printf("%s row= %d\n", __func__, retVal);
    return retVal;
}

/*  
   find lowest cost in uncovered matrix
*/
TYPE Matrix::findLowestCost()
{
    TYPE lowestCost = 0;
    int row = 0;
    int colm = 0;
    for (int r = 1; r <= numRows; r++) {
	for (int c = 1; c <= numCols; c++) {
	    if (ROW_COVER(r) == Covered || (COL_COVER(c) == Covered)) {
		continue;
	    }
	    if (lowestCost == 0) {
		lowestCost = grid[r][c];
		row = r;
		colm = c;
	    }
	    if (lowestCost > grid[r][c]) {
		lowestCost = grid[r][c];
		row = r;
		colm = c;
	    }
	}
    }
    // printf("%s grid[%d][%d]=%d\n", __func__, row, colm, lowestCost);
    return lowestCost;
};

/* 
Step 4:
   a) Find a noncovered zero and Prime it.  
   b) If there is no starred zero in the row containing this primed zero, Go to Step 5.  
   c) Otherwise, cover this row and uncover the column containing the starred zero. 
   d) Continue in this manner until there are no uncovered zeros left. 
   e) Save the smallest uncovered value and Go to Step 6.
*/
void Matrix::doStep4(void)
{
    /* find a uncovered zero */
    int row = 0;
    int col = 0;
    // PrintMatrix();
    while (nextStep == Step4) {
	if (-1 == findUnCoveredZero(&row, &col)) {
	    nextStep = Step6;
	    continue;
	}
	/* Prime it */
	mask[row][col] = PrimedZero;
	/* Flip Covered/Uncovered */
	int coloumn = 0;
	if ((coloumn = starInRow(row))) {
	    ROW_COVER(row) = Covered;
	    COL_COVER(coloumn) = UnCovered;
	    // printf("after each iteration doStep4\n");
	    // PrintMask();
	} else {
	    rowFromStep4 = row;
	    colFromStep4 = col;
	    nextStep = Step5;
	    continue;
	}
    }
    // PrintMask();
    TRACE_FLOW;
    return;
}

/*
Step 5:    
      Construct a series of alternating primed and starred zeros as follows.  
      a) Let Z0 represent the uncovered primed zero found in Step 4.  
      b) Let Z1 denote the starred zero in the column of Z0 (if any). 
      c) Let Z2 denote the primed zero in the row of Z1 (there will always be one).  
      d) Continue until the series terminates at a primed zero that has no starred zero in its column.  
      e) 
         i)   Unstar each starred zero of the series
         ii)  star each primed zero of the series 
         iii) erase all primes and uncover every line in the matrix.  
      f) Return to Step 3. 
*/
int Matrix::findStarZeroInSameCol(int col)
{
    int starRowVal = 0;
    for (int row = 1; row <= numRows; row++) {
	if (mask[row][col] == StarredZero) {
	    starRowVal = row;
	    break;
	}
    }
    // printf("%s starRowVal = %d\n", __func__, starRowVal);
    return starRowVal;
}

int Matrix::findPrimeZeroInSameRow(int row)
{
    int primeColVal = 0;
    for (int col = 1; col <= numCols; col++) {
	if (mask[row][col] == PrimedZero) {
	    primeColVal = col;
	    break;
	}
    }
    // printf("%s PrimeColVal = %d\n", __func__, primeColVal);
    return primeColVal;
}

void Matrix::doStep5(void)
{
    /* primed zero loation : rowFromStep4, colFromStep4 */
    assert(rowFromStep4 != -1);
    assert(colFromStep4 != -1);

    int startRow = rowFromStep4;
    int startCol = colFromStep4;
    int endRow = -1;

    // printf("rowFromStep4=%d colFromStep4=%d\n", rowFromStep4, colFromStep4);
    rowFromStep4 = -1;		/* remove the information */
    colFromStep4 = -1;		/* ..................... */
    /* 5 a - d */
    mask[startRow][startCol] = PrimeZeroMadePair;
    while ((endRow = findStarZeroInSameCol(startCol))) {
	/* make pair */
	mask[endRow][startCol] = StarredZeroMadePair;
	startRow = endRow;
	startCol = findPrimeZeroInSameRow(startRow);
	assert(startCol != 0);
	mask[startRow][startCol] = PrimeZeroMadePair;
    }
    for (int row = 1; row <= numRows; row++) {
	for (int col = 1; col <= numCols; col++) {
	    if (mask[row][col] == StarredZeroMadePair) {
		/* i)   Unstar each starred zero of the series */
		mask[row][col] = NormalZero;
	    } else if (mask[row][col] == PrimeZeroMadePair) {
		/* ii)  star each primed zero of the series  */
		mask[row][col] = StarredZero;
	    }
	}
    }
    for (int row = 0; row <= numRows; row++) {
	for (int col = 0; col <= numCols; col++) {
	    if (row == 0 || col == 0) {
		mask[row][col] = UnCovered;
	    }
	}
    }
    nextStep = Step3;
    // PrintMask();
    TRACE_FLOW;
    return;
}

/*
Step 6:
   1) cost = find lowest cost of the node in cost matrix
   2) add "cost" every element of each covered row
   3) subtract "cost" from every element of each uncovered column.
      ==> add zero
*/
void Matrix::doStep6(void)
{
    TYPE lowCost = findLowestCost();
    /* addition / subtraction */
    for (int r = 1; r <= numRows; r++) {
	for (int c = 1; c <= numCols; c++) {
	    if (Covered == ROW_COVER(r)) {
		grid[r][c] += lowCost;
	    }
	    if (UnCovered == COL_COVER(c)) {
		grid[r][c] -= lowCost;
	    }
	}
    }
    TRACE_FLOW;
    // PrintMatrix();
    nextStep = Step4;
    return;
}

void Matrix::Engine()
{
    while (nextStep != AllDone) {
	switch (nextStep) {
	case None:
	case Step1:
	    doStep1();
	    break;
	case Step2:
	    doStep2();
	    break;
	case Step3:
	    doStep3();
	    break;
	case Step4:
	    doStep4();
	    break;
	case Step5:
	    doStep5();
	    break;
	case Step6:
	    doStep6();
	    break;
	default:
	    break;
	}
    }
    TRACE_FLOW;
    // PrintMatrix();
    // PrintMask();
}

TYPE Matrix::GetMinimumCost()
{
    TYPE totalCost = 0;
    for (int i = 0; i <= SIZE; i++) {
	for (int j = 0; j <= SIZE; j++) {
	    totalCost += (mask[i][j] == StarredZero) ? origGrid[i][j] : 0;
	}
    }
    return totalCost;
}
