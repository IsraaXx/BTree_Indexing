#include "BTreeIndex.h"
using namespace std;

int main(){
    BTreeIndex bTreeIndex;
    cout << "Create Index File" << "\n";
    cout << "Enter the number of keys (m):";
    int m;
    cin >> m;
    bTreeIndex.CreateIndexFile("BTreeIndex.txt", m * 2, m);
        int choice, recordID, reference;
        do {
            cout << "\nB-Tree Index Menu:" << "\n";
            cout << "1. Insert New Record" << "\n";
            cout << "2. Delete Record" << "\n";
            cout << "3. Display Index File Content" << "\n";
            cout << "4. Search for a Record" << "\n";
            cout << "5. Exit" << "\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice){
                case 1: {
                    cout << "Enter RecordID and Reference: ";
                    cin >> recordID >> reference;
                 //   bTreeIndex.InsertNewRecordAtIndex(recordID, reference);
                   // int referenceValue = bTreeIndex.SearchARecord("BTreeIndex.txt", recordID);
//                    if(referenceValue == -1){
//                        bTreeIndex.InsertNewRecordAtIndex( recordID, reference);}
//                    else{
//                        cout << "Error: Can't insert RecordID already exits." << "\n";
//                    }
                    break;
                }


                case 3: {
                    bTreeIndex.DisplayIndexFileContent("BTreeIndex.txt");
                    break;
                }
                case 5: {
                    cout << "Exiting program." << "\n";
                    break;
                }

                default: {
                    cout << "Invalid choice. Please enter a valid option." << "\n";
                }
            }
        } while (choice != 5);

}

