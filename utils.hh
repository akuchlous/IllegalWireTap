#ifndef _UTILS_H_
#define _UTILS_H_

#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <strings.h>

const int TOTAL_AGENT = 5;
#define FORMAT "%f, "
#define EVEN_AGENT(x) !(x&0x1)
#define ODD_AGENT(x)  (x&0x1)

typedef double TYPE;
int Primes[27] = {
    0, 0, 1, 1, 0, 1, 0, 1, 0, 0,	/*  0 --  9 */
    0, 1, 0, 1, 0, 0, 0, 1, 0, 1,	/* 10 -- 19 */
    0, 0, 0, 1, 0, 0, 0		        /* 20 -- 26 */
};

int PrimeFactor[27][3] = {
    {0, 0, 0},			/* = 0 */
    {0, 0, 0},			/* = 1 */
    {2, 0, 0},
    {3, 0, 0},
    {2, 0, 0},
    {5, 0, 0},
    {2, 3, 0},
    {7, 0, 0},
    {2, 0, 0},
    {3, 0, 0},
    {2, 5, 0},			/* = 10 */
    {11, 0, 0},
    {2, 3, 0},
    {13, 0, 0},
    {2, 7, 0},
    {3, 5, 0},
    {2, 0, 0},
    {17, 0, 0},
    {2, 3, 0},
    {19, 0, 0},
    {2, 5, 0},			/* = 20 */
    {3, 7, 0},
    {2, 11, 0},
    {23, 0, 0},
    {2, 3, 0},
    {5, 0, 0},
    {2, 13, 0}			/* = 26 */
};

/* Caclulate number of consonant and vowels for a string */
int numOfVowelsAndConsonants(const char *string,
			     size_t * numVowel, size_t * numConsonant)
{
    size_t localVowel = 0;
    size_t localConsonant = 0;
    if (!string) {
	return -1;
    }
    size_t len = strlen(string);
    for (size_t index = 0; index < len - 1; index++) {
	if (string[index] == ' ')
	    break;
	switch (string[index]) {
	case 'A':
	case 'a':
	case 'E':
	case 'e':
	case 'I':
	case 'i':
	case 'O':
	case 'o':
	case 'U':
	case 'u':
	    localVowel++;
	    break;
	default:
	    localConsonant++;
	    break;
	}
    }
    *numVowel = localVowel;
    *numConsonant = localConsonant;
    return 0;
}

int fillPrimeFactor(int number, int *array)
{
    /* initialize to 0 */
    for (int i = 0; i < 27; i++) {
	array[i] = 0;
    }
    for (int factor = 2; factor <= 26; factor++) {
	if (Primes[factor] && (number % factor == 0)) {
	    array[factor] = 1;
	}
    }
    return 0;
}

/* Cost for an agent to do this particular task */
TYPE costOfTask(int agentNumber, size_t numOfVowels,
		size_t numOfConsonants, int *primeFactorsOfWireTap)
{
    TYPE cost = 0;
    cost = (TYPE) (1 + EVEN_AGENT(agentNumber) * (numOfVowels * 1.5) +
		   ODD_AGENT(agentNumber) * 1 * numOfConsonants);

    for (int numFactor = 0; numFactor < 3; numFactor++) {
	int factor = PrimeFactor[agentNumber][numFactor];
	if (!factor) {
	    continue;
	}
	cost += (2 * primeFactorsOfWireTap[factor]);
    }
    return cost;
}

/* calculate cost of  task for each agent*/
int fillCostOfTaskForEachAgent(FILE * fp, TYPE * agentCostRow)
{
    char *string = NULL;
    size_t len = 0;
    ssize_t read;
    read = getline(&string, &len, fp);
    if (read == -1) {
	printf("Error in reading  wireTap names\n");
	return -1;
    }
    size_t vowels = 0;
    size_t consonants = 0;
    bzero(agentCostRow, (TOTAL_AGENT + 1) * sizeof(TYPE));
    numOfVowelsAndConsonants(string, &vowels, &consonants);

    int primeFactorsOfWireTap[27];
    fillPrimeFactor(strlen(string) - 1, primeFactorsOfWireTap);

    for (int agentNumber = 1; agentNumber <= TOTAL_AGENT; agentNumber++) {
	agentCostRow[agentNumber] =
	    costOfTask(agentNumber, vowels, consonants,
		       primeFactorsOfWireTap);
    }
    free(string);
    return 0;
}

#endif				//]
