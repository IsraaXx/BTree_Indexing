#include "BTreeIndex.h"

using namespace std;

void BTreeIndex::CreateIndexFile(const char *filename, int numberOfRecords, int m){
    BTreeFile.open(BTreeFileName, ios::in | ios::out | ios::binary);
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
        cout << " Place: " << Tree[i].place << " | nodeType: " << Tree[i].nodeType << " | Node: ";
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
        iss >> Node.nodeType;
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
        if (Btree[j].nodeType == 1){//internal node has children
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
        outFile << node.nodeType << "  ";

        for (const auto &pair : node.node){
            outFile << pair.first << "  ";
            outFile << pair.second << "  ";
        }
        outFile << "\n";
    }

    outFile.close();
}

int BTreeIndex::InsertNewRecordAtIndex(int RecordID, int Reference){
    vector<BTreeNode> bTree = ReadFile(BTreeFileName);//Reads the B-tree from the file into memory
    if (bTree[0].count == 0){ //Handles the case where the tree is empty
        head = bTree[0].node[0].first; //makes the head with the next empty place
        bTree[0].node[0].first = RecordID; //Sets the first record in the root node
        bTree[0].node[0].second = Reference;
        bTree[0].nodeType = 0;  //make it leaf node as it the first node in the tree
        bTree[0].count++; // count this key
        SaveFile(BTreeFileName, bTree,m); //Saves the updated B-tree and exits.
        return 1;                         //insertion was successful
    }
    stack<int> visited; //to track visited nodes during traversal, starting from the root
    int i = 0;
    bool found = false;       // traverse as long as you have internal nodes
    while (bTree[i].nodeType){  //to find the appropriate node (i) to insert the new record
        visited.push(i);     //Pushes visited internal nodes onto the stack
        found = false;       //This allows us to backtrack and update parent nodes later if needed
        for (int j = 0; j < bTree[i].node.size(); ++j) {
            if (bTree[i].node[j].first >= RecordID){ //If RecordID is less than or equal to a key, moves to the corresponding child node
                i = bTree[i].node[j].second - 1;     // update index with the child node
                found = true;
                break;
            }
        }
        // means each key we found was smaller than the key we want to insert
        // so its position will be the reference of the last node
        if (!found){ // to get the last pair , Updates [i] to the index of the last child
            i = bTree[i].node[bTree[i].count -1].second -1;
        }
    }
    // we found the appropriate node to insert at
    bTree[i].node.push_back(make_pair(RecordID,Reference));
    //nodes are sorted based on the RecordID
    sort(bTree[i].node.begin(), bTree[i].node.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        if (a.first != -1 && b.first != -1) { //custom comparator to sort pairs based on RecordID
            return a.first < b.first;
        }
        return a.first != -1;
    });

    bTree[i].count++;           //reflect the addition of a new record
    int newFromSplitIndex = -1; //Checks if the node exceeds the maximum allowed number of entries (m)
    if (bTree[i].count > m){    //If so, the node is split, and SplitNode
        newFromSplitIndex = Split(i, bTree);
    }else{
        bTree[i].node.pop_back();
        SaveFile(BTreeFileName, bTree, m);
    }
    if(i == 0){
        return 1;
    }
    //After the insertion, the path from the root to the inserted leaf node is traversed
    // in reverse order (using the visited stack), and any necessary updates are made to the parent nodes
    while (!visited.empty()) {
        int lastVisitedIndex = visited.top();
        visited.pop();
        newFromSplitIndex = updateAfterInsert(lastVisitedIndex, newFromSplitIndex);
    }
    return -1;
}

int BTreeIndex::Split(int i, vector<BTreeNode> bTree){
    int newRecordNumber = head - 1; // The new node will be created at the head - 1 index //subtract -1 because the index starts from 0
    if (i == 0){        //if the index of the node that I want to split is RootNode then call SplitRoot
        return SplitRoot(bTree);
    }
    if (head == -1) {
        return -1; // No free nodes available, split cannot proceed.
    }

    head = bTree[head -1].node[0].first; // Update the head to the next free node.
    vector<pair<int, int>> firstNode, secondNode;
    //The tie function is used to unpack the two vectors returned
    tie(firstNode, secondNode) = SplitNode(bTree[i].node);
    int size = firstNode.size();
    int size2 = secondNode.size();

    bTree[i].nodeType = 0;
    bTree[i].count = size;
    for (int j = size; j < m; ++j) { //the remaining entries are filled with (-1, -1)
        firstNode.push_back(make_pair(-1,-1));
    }
    for (int j = 0; j < m; ++j) { //Copy Data Back to the Node
        bTree[i].node[j] = firstNode[j]; // Update the node with the first half of the keys.
    }
    //The last key belongs to the right half (secondNode), not to the left half. Since it has already been moved to the new node,
    // it should be removed from bTree[i].node.
    bTree[i].node.pop_back();
    bTree[newRecordNumber].nodeType = 0;
    bTree[newRecordNumber].count = size2;
    for (int j = size; j < m; ++j) { //the remaining entries are filled with (-1, -1)
        secondNode.push_back(make_pair(-1,-1));
    }
    for (int j = 0; j < m; ++j) {
        bTree[newRecordNumber].node[j] = secondNode[j];// Update the node with the second half of the keys.
    }
    SaveFile(BTreeFileName, bTree,m);
    return newRecordNumber;

}
bool BTreeIndex::SplitRoot(vector<BTreeNode> bTree){
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
    bTree[firstNodeIdx- 1].nodeType = 0;
    bTree[firstNodeIdx - 1].count = size;
    for (int i = 0; i < size; ++i) {
        bTree[firstNodeIdx - 1].node[i] = firstNode[i];
    }

    int size2 = secondNode.size();
    bTree[secondNodeIdx - 1].nodeType = 0;
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
    //If both child nodes have indices greater than thresholds (indicating children exist)
    //mark the  2 node as non leaf node if it has children
    if (firstNodeIdx > 2 && secondNodeIdx > 3) {
        bTree[firstNodeIdx - 1].nodeType = 1;
        bTree[secondNodeIdx - 1].nodeType = 1;
    }

    bTree[0].node = root; // Update the root node in the tree.
    bTree[0].nodeType = 1;  // Mark the root as an internal node.
    bTree[0].count = 2;   // Set the number of keys in the root to 2
    //The largest key from firstNode. //The largest key from secondNode.
    SaveFile(BTreeFileName, bTree, m);
    return true;
}
//that splits the current node into two parts: firstNode (left half) and secondNode (right half).
pair<vector<pair<int, int>>, vector<pair<int, int>>> BTreeIndex::SplitNode(const vector<pair<int, int>> &originalNode){
    vector<pair<int, int>> firstNode, secondNode;

    auto middle = originalNode.begin() + originalNode.size() / 2;//The middle pointer is used to divide the node into two halves:
    //Iterates through all elements
    for (auto it = originalNode.begin(); it != originalNode.end(); ++it) {
        if (distance(it, middle) > 0) { // calc the iterator comparing to the middle if it's greater than 0 then it's in the first half // how far (it) is from middle
            firstNode.push_back(*it); // Add the key-value pair to firstNode
        } else {
            secondNode.push_back(*it);// Add the key-value pair to secondNode
        }
    }

    return make_pair(firstNode, secondNode);
}
void BTreeIndex::DeleteCase2(const char *filename, vector<BTreeNode> &bTree, BTreeNode &find, int RecordID, int &count, int &temp)
{
    for (int i = 0; i < find.node.size(); ++i)
    { // case 2
        if (find.node[i].first == RecordID)
        {
            find.node[i].first = -1;
            find.node[i].second = -1;
        }
        sort(find.node.begin(), find.node.end(), [](const std::pair<int, int> &a, const std::pair<int, int> &b)
        {
            if (a.first != -1 && b.first != -1) {
                return a.first < b.first;
            }
            return a.first != -1; });
    }
    temp = find.node[count - 2].first;
    bTree[(find.place - 1)].node = find.node;
    for (auto &i : bTree)
    {
        for (auto &k : i.node)
        {
            if (k.first == RecordID)
            {
                k.first = temp;
            }
        }
    }
    find.count--;
    SaveFile(filename, bTree, m);
}

void BTreeIndex::DeleteCase1(const char *filename, vector<BTreeNode> &bTree, BTreeNode &find, int RecordID){
    for (int i = 0; i < find.node.size(); ++i)
    { // case 1
        if (find.node[i].first == RecordID)
        {
            find.node[i].first = -1;
            find.node[i].second = -1;
        }
        sort(find.node.begin(), find.node.end(), [](const std::pair<int, int> &a, const std::pair<int, int> &b)
        {
            if (a.first != -1 && b.first != -1) {
                return a.first < b.first;
            }
            return a.first != -1; });
    }
    find.count--;
    bTree[(find.place - 1)].node = find.node;
    SaveFile(filename, bTree, m);
}
void BTreeIndex::DeleteRecordFromIndex(const char *filename, int RecordID, int m)
{
    vector<BTreeNode> bTree;
    bTree = ReadFile(filename);
    int balance = m / 2;
    int count = 0;
    int temp;
    BTreeNode find;
    for (auto &i : bTree)
    {
        if (i.nodeType == 0)
        {
            for (int k = 0; k < i.node.size(); ++k)
            {
                if (i.node[k].first == RecordID)
                {
                    find = i;
                    break;
                }
            }
        }
    }
    if (find.place == 1)
    { // if the node is the root
        DeleteCase1(filename,bTree,find,RecordID);
        bTree = ReadFile(filename);
        if (bTree[0].count == 0){
            bTree[0].node[0].first = 2;
            head = 1;
            bTree[0].nodeType =  -1;
        }
        SaveFile(filename, bTree, m);
        return;
    }
    for (auto &i : find.node)
    { // get the node balance
        if (i.first != -1)
        {
            count++;
        }
    }
    if ((count - 1) >= balance)
    {
        if (find.node[count - 1].first == RecordID)
        {
            DeleteCase2(filename, bTree, find, RecordID, count, temp);
        }
        else
        {
            DeleteCase1(filename,bTree,find, RecordID);
        }
    }
    else
    {
        ////////// case 3 or 4
        BTreeNode parent;
        BTreeNode siblings;
        bool flag = 0;
        for (size_t i = 0; i < bTree.size(); i++)
        {
            if (bTree[i].nodeType == 1)
            {
                for (size_t j = 0; j < bTree[i].node.size(); j++)
                {
                    if (bTree[i].node[j].second == find.place)
                    {
                        parent = bTree[i];
                        break;
                    }
                }
            }
        }

        for (size_t i = 0; i < parent.children.size(); i++)
        {
            int ct = 0;
            for (size_t j = 0; j < parent.children[i].node.size(); j++)
            {
                if (parent.children[i].node[j].first != -1)
                {
                    ct++;
                }
            }
            parent.children[i].count = ct;
        }

        for (size_t i = 0; i < parent.children.size() ; i++)
        {
            if (parent.children[i].place == find.place){
                break;
            }
            if ((parent.children[i].count > balance) && (parent.children[i +1].place == find.place))
            {
                siblings = parent.children[i];
                flag = 1;
                break;
            }

        }
        if (flag)
        {
            // case 3
            auto tmp = siblings.node[siblings.count - 1];/// the big one in the left
            DeleteCase2(filename, bTree, siblings, siblings.node[siblings.count - 1].first, siblings.count, temp);
            find.node[find.count] = tmp;
            find.count++;
            sort(find.node.begin(), find.node.end(), [](const std::pair<int, int> &a, const std::pair<int, int> &b)
            {
                if (a.first != -1 && b.first != -1) {
                    return a.first < b.first;
                }
                return a.first != -1; });
            if(find.node[find.count -1].first == RecordID){
                DeleteCase2(filename, bTree, find, RecordID, find.count, temp);
            }else{
                DeleteCase1(filename, bTree,find, RecordID);
            }

        }
        else
        {
            if(find.node[find.count -1].first == RecordID){
                DeleteCase2(filename, bTree, find, RecordID, find.count, temp);
            }else{
                DeleteCase1(filename, bTree,find, RecordID);
            }
            bTree = ReadFile(filename);
            int headTree = head;
            head = find.place;
            pair<int, int> temp = make_pair(bTree[find.place -1].node[0].first,bTree[find.place -1].node[0].second);
            for (int i = 0; i < parent.children.size(); ++i) {
                if (parent.node[i].second == find.place){
                    parent.node[i].second = -1;
                    parent.node[i].first = -1;
                    sort(parent.node.begin(), parent.node.end(), [](const std::pair<int, int> &a, const std::pair<int, int> &b)
                    {
                        if (a.first != -1 && b.first != -1) {
                            return a.first < b.first;
                        }
                        return a.first != -1; });
                    break;
                }
            }
            bTree[parent.place -1] = parent;
            int pl = find.place;
            find = bTree[pl -1];
            find.nodeType = -1;
            find.node[0].first = headTree;
            find.node[0].second = -1;
            bTree[pl -1] = find;
            SaveFile(filename, bTree,m);
            InsertNewRecordAtIndex(temp.first, temp.second);

        }
    }
}

