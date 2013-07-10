/*****************************************************************************
 *
 * memclim.c: Memory Climates.
 *
 * Copyright Aldor.org 1990-2007.
 *
 ****************************************************************************/

#include "memclim.h"

#define  USE_MEMORY_CLIMATE

#define MAX_MEMCLIM	1000
#define UNKNOWN_MEMCLIM	0

MemoryClimate	memoryClimate	= 0;

int		memclimL        = MAX_MEMCLIM; /* Ultimate limit */
int		memclimC 	= 1;           /* Current count  */

MemoryClimate	memclimClimateV   [MAX_MEMCLIM] = { UNKNOWN_MEMCLIM };
char *		memclimStringV    [MAX_MEMCLIM] = { "Unknown" };
int		memclimCountRegsV [MAX_MEMCLIM] = { 1 };

int		memclimHistogramV1[MAX_MEMCLIM];
int		memclimHistogramV2[MAX_MEMCLIM];

void
registerMemoryClimate(MemoryClimate clim, char *climString)
{
	printf("Registering \"%s\" to have climate = %d\n", climString, clim);
	if (memclimC >= memclimL) {
		fprintf(stderr, "Too many memory climates %d = \"%s\"\n",
			clim, climString);
	}
	else {
		int i;
		for (i = 0; i < memclimC; i++) {
			if (memclimClimateV[i] == clim) {
				memclimCountRegsV[i]++;
				break;
			}
		}
		if (i == memclimC) {
			memclimClimateV[i]   = clim;
			memclimStringV[i]    = climString;
			memclimCountRegsV[i] = 1;
			memclimC++;
		}
	}
}

/* You can make it smaller only. */
void            
limitNumberOfMemoryClimates(int n)
{
	if (n < memclimL) memclimL = n;
}

MemoryClimate
setMemoryClimate(MemoryClimate clim)
{
	MemoryClimate oc = memoryClimate;
	printf("Setting memory climate to be %d\n", clim);
	memoryClimate = clim;
	return oc;
}

int
numberOfMemoryClimates()
{
	return memclimC;
}

MemoryClimate
getMemoryClimate()
{
	return memoryClimate;
}

char *
getMemoryClimateString(MemoryClimate clim)
{
	int i;
	for (i = 0; i < memclimC-1; i++)
		if (memclimClimateV[i] == clim) return memclimStringV[i];
	return "Unregistered";
}

int
getMemoryClimateIndex(MemoryClimate clim)
{
	int i;
	for (i = 0; i < memclimC-1; i++)
		if (memclimClimateV[i] == clim) return i;
	return -1;
}

MemoryClimate
getMemoryClimateOfIndex(int ix)
{
	return memclimClimateV[ix];
}

void
initMemoryClimateHistogram()
{
	int i;
	for (i = 0; i < MAX_MEMCLIM; i++) {
		memclimHistogramV1[i] = 0;
		memclimHistogramV2[i] = 0;
	}
}

void
incrMemoryClimateHistogram(MemoryClimate clim, int n1, int n2)
{
	printf("<%d,%d>", n1, n2);
	memclimHistogramV1[clim] += n1;
	memclimHistogramV2[clim] += n2;
}

void
finiMemoryClimateHistogram()
{
}

void
showMemoryClimateHistogram(FILE *outf)
{
	int i;

	fprintf(outf, "================= Memory Histogram ===============\n");
	for (i = 0; i < memclimC; i++)
		fprintf(outf, "%20s: %d / %d\n", 
			memclimStringV[i],
			memclimHistogramV1[i],
			memclimHistogramV2[i]
		);
	fprintf(outf, "==================================================\n");
}
