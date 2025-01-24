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
    int nodeType;
    int count = 0;
    int place;
    vector<pair<int, int>> node;
    vector<BTreeNode> children;
};

class BTreeIndex{
    const char *const BTreeFileName = "BTreeIndex.txt";
    int numberOfRecords;
    int m;
    int head{}; //The head is used to manage the linked list of empty nodes
    fstream BTreeFile;
    void DeleteCase2(const char *filename, vector<BTreeNode> &bTree, BTreeNode &find, int RecordID, int &count, int &temp);
    void DeleteCase1(const char *filename, vector<BTreeNode> &bTree, BTreeNode &find, int RecordID);

public:
    void CreateIndexFile(const char *filename, int numberOfRecords, int m);
    int InsertNewRecordAtIndex(int RecordID, int Reference);
    void DisplayIndexFileContent(const char *filename);
    vector<BTreeNode> ReadFile(const char *filename);
    void SaveFile(const char *filename, vector<BTreeNode> bTree, int m);
    void DeleteRecordFromIndex(const char *filename, int RecordID, int m);

/////////////////////////////Functions used in insert////////////////////////////////////////////
    int Split(int i,vector<BTreeNode> bTree);
    bool SplitRoot(vector<BTreeNode> bTree);
    pair<vector<pair<int, int>>, vector<pair<int, int>>> SplitNode(const vector<pair<int, int>>& originalNode);
    //int updateAfterInsert(int parentRecordNumber, int newChildRecordNumber);

};

#endif // BTREEINDEX_BTREEINDEX_H
