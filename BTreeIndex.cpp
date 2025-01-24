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
        outfile << -1 << "  " << i << "  ";
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
    File >> head >> head; // read first char put it in head and then
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
        count = 0;
    }
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
