#define _CRT_SECURE_NO_WARNINGS 

#include <iostream>
#include <fstream>
#include <cstdio>
#include <queue>

#include "Tools.h"
#include "ArgParser.h"
#include "FileContentFilter.h"
#include "LexemeParser.h"
#include "LexemeProcesser.h"
#include "WordProcesser.h"
#include "PhraseProcesser.h"

using namespace std;

int main(int argc, char * argv[]);

int showResults(Status* status);
Status* initStatus(int argc, char* argv[]);

int main(int argc, char* argv[]) {
	
	Status* status = initStatus(argc, argv);

	
	LexemeProcesser* lexemeProcesser;
	if (status->phraseLength > 1) {
		lexemeProcesser = new PhraseProcesser(status);
	}
	else {
		lexemeProcesser = new WordProcesser(status);
	}

	LexemeParser* lexemeParser = new LexemeParser(status, lexemeProcesser);
	FileContentFilter* fileContentFilter = new FileContentFilter(status, lexemeParser);
	
	fileContentFilter->scan();
	lexemeParser->sendToStatus();
	lexemeProcesser->sendToStatus();

	showResults(status);

	delete status;
	delete lexemeParser;
	delete lexemeProcesser;
	delete fileContentFilter;

	return 0;


}

struct cmp {
	bool operator() (pair<int, string> a, pair<int, string> b) {
		if (a.first < b.first) {
			return 1;
		}
		else if (a.first == b.first) {
			return a.second > b.second;
		}
		return 0;
	}
};

int showResults(Status* status) {
	FILE* outfile = NULL;
	outfile = fopen((status->outputFilePath).c_str(), "w+");
	fprintf(outfile, "characters: %d\n", status->charNum);
	fprintf(outfile, "words: %d\n", status->wordTotalNum);
	fprintf(outfile, "lines: %d", status->lineNum);


	cout << "characters: " << status->charNum << endl;
	cout << "words: " << status->wordTotalNum << endl;
	cout << "lines: " << status->lineNum;

	
	map<string, int>::iterator iter;
	priority_queue<pair<int, string>, vector<pair<int, string>>, cmp> que;
	iter = ((status->storeMap).strMap)->begin();

	while (iter != ((status->storeMap).strMap)->end())
	{
		que.push(make_pair(iter->second, iter->first));
		iter++;
	}

	for (unsigned int i = 0; (i < (unsigned int)(((status->storeMap).strMap)->size()))
								&& (i < (unsigned int)(status->topNum)); i++) {
		pair<int, string> strPair = que.top();
		que.pop();

		cout << endl << "<" << strPair.second << ">: " << strPair.first;
		fprintf(outfile, "\n<%s>: %d", strPair.second.c_str(), strPair.first);
	}
	fclose(outfile);
	
	return 0;
}

Status * initStatus(int argc, char * argv[])
{
	Status* status = new Status();
    
	ArgParser argParser(argc, argv, status);

	return status;
}
