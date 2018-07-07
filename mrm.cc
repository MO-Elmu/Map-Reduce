/**
 * File: mrm.cc
 * ------------
 * Provides the entry point into the worker that's
 * operating as a mapper.  Inspect the documentation
 * of the MapReduceMapper class for more information
 * as to how this works.
 */

#include "mapreduce-mapper.h"
static const int kExpectedArgumentCount = 7;
int main(int argc, char *argv[]) {
  char * pEnd;
  if (argc != kExpectedArgumentCount) return 1;
  MapReduceMapper mrm(/* serverHost = */ argv[1],
                      /* serverPort = */ atoi(argv[2]),
                      /* cwd = */ argv[3],
                      /* mapper = */ argv[4],
                      /* outputPath = */ argv[5],
	              /* numHashCodes = */ strtoul(argv[6],&pEnd,10));
  mrm.map();
  return 0;
}
