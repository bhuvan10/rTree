
# Rtree

R-tree is a data structure used for indexing spatial information in databases and computer systems. It is particularly designed for efficient querying and retrieval of objects based on their spatial relationships, such as proximity or containment. The "R" in R-tree stands for "rectangle," which refers to the bounding boxes used to represent the spatial extent of objects.

The R-tree organizes objects hierarchically into a tree structure, where each node represents a bounding box that encompasses its child nodes. This hierarchical organization enables efficient searching and pruning of large portions of the tree during query operations, reducing the search space and improving performance.

R-trees are widely used in various applications involving spatial data, such as geographic information systems (GIS), spatial databases, and spatial indexing in multidimensional data. They are effective for spatial range queries, nearest neighbor searches, and spatial join operations, making them an essential tool for managing and analyzing spatial data efficiently.




## Tech Stack

C Progeamming Language

## Instruction to Run the Program
- Put `rTree.c`, `small.txt` and `data2.txt` in a directory.
- `rTree.c`is the program's C file.
- `small.txt` contains the smaller dataset and `large.txt` contains larger dataset.
- Open a terminal window (for Mac/Linux) or command promt window (for Windows) in that directory.
- To run the program run the following command 
  -   for windows run `gcc -o DSA_assignment_group_11.exe DSA_assignment_group_11.c && DSA_assignment_group_11.exe filename.txt` in the command prompt.
  -   for Mac/Linux run `gcc -o DSA_assignment_group_11 DSA_assignment_group_11.c && ./DSA_assignment_group_11 filename.txt` in the termial.
-   `filename.txt` is the command line argument. Replace it with appropriate file name present in your system.
- Output will be printed on the screen.


![App Screenshot]([https://via.placeholder.com/468x300?text=App+Screenshot+Here](https://github.com/bhuvan10/rTree/blob/main/img.png))
