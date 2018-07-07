/**
 * File: mapreduce-mapper.cc
 * -------------------------
 * Presents the implementation of the MapReduceMapper class,
 * which is charged with the responsibility of pressing through
 * a supplied input file through the provided executable and then
 * splaying that output into a large number of intermediate files
 * such that all keys that hash to the same value appear in the same
 * intermediate.
 */

#include "mapreduce-mapper.h"
#include "mr-names.h"
#include "string-utils.h"
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <limits>
#include "ostreamlock.h"
using namespace std;

MapReduceMapper::MapReduceMapper(const string& serverHost, unsigned short serverPort,
                                 const string& cwd, const string& executable,
                                 const string& outputPath,
                                 const size_t numHashCodes) :
  MapReduceWorker(serverHost, serverPort, cwd, executable, outputPath), nHashCodes(numHashCodes) {}

void MapReduceMapper::map() const {
  while (true) {
    string name;
    string line;
    if (!requestInput(name)) break;
    alertServerOfProgress("About to process \"" + name + "\".");
    string base = extractBase(name);
    string output = outputPath + "/" + changeExtension(base, "input", "mapped");
    bool success = processInput(name, output);
    ifstream infile (output);
    vector<ofstream> mapFileStreams;
    for(size_t i = 0; i<nHashCodes; i++){
	ofstream out;
	mapFileStreams.push_back(move(out));
    }
    
    while(infile >> line ) {
                size_t hashValue = hash<string>()(line);
                size_t belongTo = hashValue % nHashCodes;
                const string nameExtension = numberToString(belongTo, 5) + ".mapped";
                string output = outputPath + "/" + changeExtension(base, "input", nameExtension);
                if(!mapFileStreams[belongTo].is_open()) mapFileStreams[belongTo].open(output, ios::app);
                if (!line.empty()) mapFileStreams[belongTo] << line << " 1"<< "\n";
                infile.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    for(ofstream& stream : mapFileStreams) stream.close();
    infile.close();
    remove(output.c_str());
    notifyServer(name, success);
  }
  alertServerOfProgress("Server says no more input chunks, so shutting down.");
}
