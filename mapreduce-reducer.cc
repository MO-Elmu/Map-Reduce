/**
 * File: mapreduce-reducer.cc
 * --------------------------
 * Presents the implementation of the MapReduceReducer class,
 * which is charged with the responsibility of collating all of the
 * intermediate files for a given hash number, sorting that collation,
 * grouping the sorted collation by key, and then pressing that result
 * through the reducer executable.
 *
 * See the documentation in mapreduce-reducer.h for more information.
 */

#include "ostreamlock.h"
#include "mapreduce-reducer.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include <dirent.h>
#include <stdlib.h>
#include <iostream>
#include "mr-names.h"
using namespace std;

MapReduceReducer::MapReduceReducer(const string& serverHost, unsigned short serverPort,
                                   const string& cwd, const string& executable, const string& outputPath) : 
  MapReduceWorker(serverHost, serverPort, cwd, executable, outputPath) {}

void MapReduceReducer::reduce() const {
   while(true){
   	string pattern;	
	if(!requestInput(pattern)) break;
        alertServerOfProgress("About to collate all intermediate files matching in \"" + pattern + "\".");
        string base = extractBase(pattern);
        string path = extractPath(pattern);
        string outputColl = outputPath + "/" + base;
        string output = outputPath + "/" + changeExtension(base, "mapped", "output");
        string outputSorted = outputPath + "/" + changeExtension(base, "mapped", "sorted");
        string outputGrouped = outputPath + "/" + changeExtension(base, "mapped", "grouped");
        fstream outfileColl (outputColl, ios::in|ios::out|ios::app);
        fstream outfileSorted (outputSorted, ios::in|ios::out|ios::app);
        collate(base, path, outfileColl);
        sortFile(outfileColl, outfileSorted);
        outfileColl.close();
        remove(outputColl.c_str());
        outfileSorted.close();
        string script = cwd + "/group-by-key.py";
        string command = "python ";
        command = command + script + " < "+ outputSorted + " > " + outputGrouped ;
        int status = system(command.c_str());
        if (status == -1) {
            status = WEXITSTATUS(status);
            cout << oslock << "Python command not executed and returned a status " 
                 << status << "." << endl << osunlock;
        }
        remove(outputSorted.c_str());
        bool success = processInput(outputGrouped, output);
        remove(outputGrouped.c_str());
        notifyServer(pattern, success);
   }         		
}
string MapReduceReducer::extractPath(const string& pattern) const {
  size_t pos = pattern.rfind('/');
  if (pos == string::npos) return pattern;
  return pattern.substr(0,pos);
}
void MapReduceReducer::collate(const string& base, const string& path, fstream& outfile) const{
   DIR *dir = opendir(path.c_str());
    if (dir == NULL) {
     cerr << "Directory named \"" << path << "\" could not be opened." << endl;
     exit(1);
    }   
    while (true) {
    struct dirent *ent = readdir(dir);
    if (ent == NULL) break;
    if (ent->d_name[0] == '.') continue; 
    string file(path);
    file += "/";
    file += ent->d_name;
    if(file.find(base) != string::npos){
       ifstream infile(file);
       string line;
       while(getline(infile,line))
           outfile << line << "\n";
       infile.close();
       outfile.clear();
       outfile.seekg(0, ios::beg);
    }
  }
  closedir(dir); 
}

void MapReduceReducer::sortFile(fstream& outfile, fstream& outfileSorted) const{
   vector<string> lines;
   for(string line; getline(outfile, line);) lines.push_back(line);
   sort(lines.begin(), lines.end()); 
   for(auto& line: lines) outfileSorted << line << "\n";
}

