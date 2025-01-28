# B-Tree Indexing System

## Overview
This project implements a **B-tree indexing system** in C++ to efficiently manage and manipulate a set of fixed-length records stored in a binary file. The B-tree is used as a secondary index, supporting operations like record insertion, deletion, search, and displaying the structure of the index file. 

The implementation is based on an assignment designed to reinforce concepts of data structures, file management, and algorithm design.

---

## Features
1. **B-tree Creation**: 
   - Create a binary file with `n` fixed-length records and initialize the B-tree structure.
2. **Record Insertion**:
   - Add new records to the B-tree index with support for node splitting when required.
3. **Record Deletion**:
   - Remove a record from the index and update the tree structure accordingly.
4. **Search**:
   - Search for a record in the B-tree and retrieve its reference from the data file.
5. **Display**:
   - Visualize the content of the index file, showing each node on a new line.

---

## How It Works
1. **File Structure**:
   - Each record consists of:
     - **m** descendants: m Record IDs and m references.
     - **1 integer** indicating whether the node is a leaf (0) or an internal node (1).
   - Node 0 always stores metadata:
     - The index of the first empty node at its second integer position.
     - Serves as a linked list of empty nodes.

2. **Node Indexing**:
   - The root node is always initialized at index 1.
   - Empty nodes form a linked list for efficient allocation during insertion.

---

## Functionality
The program is implemented using the following key functions:

### `void CreateIndexFileFile (Char* filename, int numberOfRecords, int m)`
- Initializes the binary file with `numberOfRecords` and configures the B-tree structure.
- Each empty node points to the next empty node, forming a linked list.

### `int InsertNewRecordAtIndex (Char* filename, int RecordID, int Reference)`
- Inserts a new record into the B-tree.
- Returns:
  - `-1` if there is no space to insert.
  - The index of the node where the record is inserted if successful.

### `void DeleteRecordFromIndex (Char* filename, int RecordID)`
- Deletes a record from the B-tree and updates the index structure.

### `void DisplayIndexFileContent (Char* filename)`
- Displays the content of the index file, showing each node on a new line.

### `int SearchARecord (Char* filename, int RecordID)`
- Searches for a record in the B-tree.
- Returns:
  - `-1` if the record does not exist.
  - The reference to the data file if the record is found.
