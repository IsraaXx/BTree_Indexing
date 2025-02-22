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
                    
                    int referenceValue = bTreeIndex.SearchARecord("BTreeIndex.txt", recordID);
                    if(referenceValue == -1){
                       bTreeIndex.InsertNewRecordAtIndex( recordID, reference);}
                   else{
                       cout << " You can't insert this RecordID already exists." << "\n";
                   }
                    break;
                }
                 case 2:{
                    cout << "Enter RecordID: ";
                    cin >> recordID ;
                    bTreeIndex.DeleteRecordFromIndex("BTreeIndex.txt",recordID,m);
                    break;
                }

                case 3: {
                    bTreeIndex.DisplayIndexFileContent("BTreeIndex.txt");
                    break;
                }
                case 4:
                {
                    cout << "Enter RecordID to search: ";
                    cin >> recordID;
                    int referenceValue = bTreeIndex.SearchARecord("BTreeIndex.txt", recordID);
                    if (referenceValue == -1)
                    {
                        cout << "Record not found in the index." << endl;
                    }
                    else
                    {
                        cout << "Record found at reference: " << referenceValue << endl;
                    }
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
//test case for insert
//m=5
//(3, 12)
// (7, 24)
// (10, 48)
// (14, 72)
// (24, 60)
//(19,84)
//(30, 96)
//(15,132)
//(1,120)
//(5,132)
//(2 ,144)
// (8, 156)
// (9, 168)
// (6, 180)
// (11, 192)
// (12, 204)
// (17, 216)
// (18, 228)
//(32,240)

//test case for delete
//10
//9
//8
