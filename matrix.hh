#ifndef _MATRIX_HH_
#define _MATRIX_HH_		//[

const int SIZE = 5;
typedef double TYPE;
#define FORMAT "%f, "
/*
each coloumn specifies a task 
each row specifies an agent
*/

class Matrix {
  public:
    Matrix() {} 
    ~Matrix() {}
    int Initialize(const char *fileName);
    void Engine();
    TYPE GetMinimumCost();
  private:
    typedef enum StepTypes {
	None = 0,
	Step1 = 1,
	Step2,
	Step3,
	Step4,
	Step5,
	Step6,
	AllDone
    };
    typedef enum zeroTypes {
	NormalZero = 0,
	StarredZero = 1,
	PrimedZero = 2,
        StarredZeroMadePair=3, /* only used in step5 */
        PrimeZeroMadePair=4    /* only used in step5 */
    };
    /* transformation from StarredZero  => StarredZeroMadePair 
       imply that a piar has been made in step 5 (a,b) steps*/
    typedef enum cover {
	UnCovered = 0,
	Covered = 1
    };
  private: // steps for algorithm
    void doStep1( void );
    void doStep2( void );
    void doStep3( void );
    void doStep4( void );
    void doStep5( void );
    void doStep6( void );
  private: // utils
    int findStarZeroInSameCol(int col);
    int findPrimeZeroInSameRow(int row);
    int findUnCoveredZero(int* row, int* col);
    int starInRow(int row);
    TYPE findLowestCost();
    void PrintMatrix();
    void PrintMask();
  private:
    TYPE grid[SIZE + 1][SIZE + 1];	/* keep the cost */
    TYPE mask[SIZE + 1][SIZE + 1];
    /* keep prime, starred, cover/uncover */
    /* [row][0] [0][col] keep covering / uncovering information */
    StepTypes nextStep;
    int numRows;
    int numCols;
    int rowFromStep4;
    int colFromStep4;
    TYPE origGrid[SIZE + 1][SIZE + 1];	/* Final cost */
};

#endif				//]
