#include "BTreeIndex.h"

using namespace std;

void BTreeIndex::CreateIndexFile(const char *filename, int numberOfRecords, int m){
    BTreeFile.open(BTreeFileName, ios::in | ios::out | ios::binary);
    if (BTreeFile.is_open()) {
        cout << "Index file created successfully.\n";
    } else {
        cerr << "Failed to create or open the file.\n";
    }
    this->numberOfRecords = numberOfRecords;
    this->m = m;  //Opens the file for writing in binary mode
    ofstream outfile(filename, ios::binary);
    outfile.clear();
    int i = 1; // header , root at beginning
    for (int j = 0; j < numberOfRecords; ++j) { // for each row
        if (i == numberOfRecords) { // to handle column which has the links to the empty places
            i = -1;
        }
        outfile << -1 << "  " << i << "  ";// Write the leaf flag and next node index.
        for (int k = 0; k < numberOfRecords - 1; ++k) {
            outfile << "-1" << "  ";
        }
        i++;
        outfile << "\n";
    }
    ReadFile(filename);
    outfile.close();
}

void BTreeIndex::DisplayIndexFileContent(const char *filename){
    ifstream inputFile(filename, ios::in | ios::binary);
    vector<BTreeNode> Tree = ReadFile(filename);
    cout << "<-----------------------Header Row------------------------>\n ";
    cout << "First Empty Place: " << head << "\n";
    for (int i = 0; i < Tree.size(); ++i){
        cout << " Place: " << Tree[i].place << " | HasLeaf: " << Tree[i].isLeaf << " | Node: ";
        for (const auto &pair : Tree[i].node){
            cout << "(" << pair.first << ", " << pair.second << ") ";
        }
        cout << "\n";
    }
}

vector<BTreeNode> BTreeIndex::ReadFile(const char *filename){
    ifstream File(filename);
    vector<BTreeNode> Btree;
    string line;
    int i = 1;
    File >> head >> head; // Read the first two integers in the file and assign them to `head`.
    getline(File, line); //read header line to ignore it  // read second char and put it in head also
    while (getline(File, line)) // second char which has the empty place
    { // start from second line which is the root , first in data
        istringstream iss(line);
        BTreeNode Node;
        iss >> Node.isLeaf;
        Node.place = i;
        int key, value;
        while ((iss >> key >> value)){
            Node.node.emplace_back(key, value);
            // as each node has vector (m) of keys (pair) !!

        }
        Btree.push_back(Node);
        i++;
        // at the end vector will have all nodes in tha file
    }
    int count = 0;
    for (int i = 0; i < Btree.size(); i++){
        // node is vector has the keys
        for (int j = 0; j < Btree[i].node.size(); j++){
            // as long as pair!=-1 this mean it's a valid key
            if ((Btree[i].node[j].first != -1) && (Btree[i].node[j].second != -1) ){
                count++;
            }
        }
        //count keys for each node we have
        Btree[i].count = count;
        count = 0; // Reset the counter for the next node
    }
    //Add children nodes to non leaf node
    for (int j = (Btree.size() - 1); j >= 0; --j){
        if (Btree[j].isLeaf == 1){//internal node has children
            for (int k = 0; k < Btree[j].node.size(); ++k){
                if (Btree[j].node[k].second != -1){//means this key-value pair points to a valid child node
                    Btree[j].children.push_back(Btree[Btree[j].node[k].second - 1]);
                }
            }
        }
    }
    File.close();
    return Btree;
}

void BTreeIndex::SaveFile(const char *filename, vector<BTreeNode> bTree, int m){
    ofstream outFile(filename, ios::binary);
    outFile << -1 << "  " << head << "  ";
    for (int i = 0; i < (m * 2) - 1; ++i){
        outFile << -1 << "  ";
        // just filling the first row (header one)
    }
    outFile << "\n";
    for (const auto &node : bTree){
        // write first val which is type og node
        outFile << node.isLeaf << "  ";

        for (const auto &pair : node.node){
            outFile << pair.first << "  ";
            outFile << pair.second << "  ";
        }
        outFile << "\n";
    }

    outFile.close();
}
int BTreeIndex::Split(int i, vector<BTreeNode> bTree)
{
     int newRecordNumber = head - 1; // The new node will be created at the head - 1 index //subtract -1 because the index starts from 0  
    if (i == 0){        //if the index of the node that I want to split is RootNode then call SplitRoot
        return SplitRoot(bTree);
    }
    if (head == -1) {
    return -1; // No free nodes available, split cannot proceed.
    }

    head = bTree[head -1].node[0].first; // Update the head to the next free node. 
    vector<pair<int, int>> firstNode, secondNode;
    tie(firstNode, secondNode) = SplitNode(bTree[i].node); 
    int size = firstNode.size();
    int size2 = secondNode.size();

    bTree[i].isLeaf = 0; 
    bTree[i].count = size;
    for (int j = size; j < m; ++j) {
        firstNode.push_back(make_pair(-1,-1));
    }
    for (int j = 0; j < m; ++j) {
        bTree[i].node[j] = firstNode[j]; // Update the node with the first half of the keys.
    }
    //The last key belongs to the right half (secondNode), not to the left half. Since it has already been moved to the new node,
    // it should be removed from bTree[i].node.
    bTree[i].node.pop_back();  
    bTree[newRecordNumber].isLeaf = 0;
    bTree[newRecordNumber].count = size2;
    for (int j = size; j < m; ++j) {
        secondNode.push_back(make_pair(-1,-1));
    }
    for (int j = 0; j < m; ++j) {
        bTree[newRecordNumber].node[j] = secondNode[j];// Update the node with the second half of the keys.
    }
    SaveFile(BTreeFileName, bTree,m);
    return newRecordNumber;
   
}
bool BTreeIndex::SplitRoot(vector<BTreeNode> bTree)
{
    int firstNodeIdx= head; /*The index of the first child node will take the first empty node. This is taken from the head, which tracks the next free node. */ 
    int secondNodeIdx = bTree[firstNodeIdx-1].node[0].first; // The index of the second child node will take the second empty node.
    if (firstNodeIdx == -1) {
        return false;
    }
    if (secondNodeIdx == -1) {
        return false;
    }
   head = bTree[head].node[0].first; //update the head to the next free node
   vector<pair<int, int>> firstNode, secondNode, root;
   tie(firstNode, secondNode) = SplitNode(bTree[0].node);

   int size = firstNode.size();
    bTree[firstNodeIdx- 1].isLeaf = 0;
    bTree[firstNodeIdx - 1].count = size;
    for (int i = 0; i < size; ++i) {
        bTree[firstNodeIdx - 1].node[i] = firstNode[i];
    }

    int size2 = secondNode.size();
    bTree[secondNodeIdx - 1].isLeaf = 0;
    bTree[secondNodeIdx - 1].count = size2;
    for (int i = 0; i < size2; ++i) {
        bTree[secondNodeIdx - 1].node[i] = secondNode[i];
    }
    //create a new root node
    root.push_back(firstNode[size - 1]); // Push the largest key from the firstNode.
    root[0].second = firstNodeIdx;    // Set the pointer to the first child node.
    root.push_back(secondNode[size2 - 1]); // Push the largest key from the secondNode.
    root[1].second = secondNodeIdx;      // Set the pointer to the second child node.

    for (int i = root.size(); i < m; ++i) {
    root.push_back(make_pair(-1, -1)); // Fill remaining slots with empty pairs (-1, -1).
     }
    //mark the  2 node as non leaf node if it has children
    if (firstNodeIdx > 2 && secondNodeIdx > 3) {
    bTree[firstNodeIdx - 1].isLeaf = 1;
    bTree[secondNodeIdx - 1].isLeaf = 1;
     }

     bTree[0].node = root; // Update the root node in the tree.
    bTree[0].isLeaf = 1;  // Mark the root as an internal node.
    bTree[0].count = 2;   // Set the number of keys in the root to 2.
    SaveFile(BTreeFileName, bTree, m);
    return true;
}
//that splits the current node into two parts: firstNode (left half) and secondNode (right half).
pair<vector<pair<int, int>>, vector<pair<int, int>>> BTreeIndex::SplitNode(const vector<pair<int, int>> &originalNode){
    vector<pair<int, int>> firstNode, secondNode;

    auto middle = originalNode.begin() + originalNode.size() / 2;//The middle pointer is used to divide the node into two halves:

    for (auto it = originalNode.begin(); it != originalNode.end(); ++it) {
        if (distance(it, middle) > 0) { // calc the iterator comparing to the middle if it's greater than 0 then it's in the first half
            firstNode.push_back(*it); // Add the key-value pair to firstNode
        } else {
            secondNode.push_back(*it);// Add the key-value pair to secondNode
        }
    }

    return make_pair(firstNode, secondNode);
}
