#ifndef BTREEINDEX_BTREEINDEX_H
#define BTREEINDEX_BTREEINDEX_H
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <sstream>
#include <algorithm>

using namespace std;

struct BTreeNode{
public:
    int isLeaf;
    int count = 0;
    int place;
    vector<pair<int, int>> node;
    vector<BTreeNode> children;
};

class BTreeIndex{
    const char *const BTreeFileName = "BTreeIndex.txt";
    int numberOfRecords;
    int m;
    int head{};
    fstream BTreeFile;

public:
    void CreateIndexFile(const char *filename, int numberOfRecords, int m);
    //int InsertNewRecordAtIndex(int RecordID, int Reference);
    void DisplayIndexFileContent(const char *filename);
    vector<BTreeNode> ReadFile(const char *filename);
    void SaveFile(const char *filename, vector<BTreeNode> bTree, int m);
};

#endif // BTREEINDEX_BTREEINDEX_H
