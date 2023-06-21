#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <stdbool.h>
// variable to store the count of keys in the leaf nodes after creating the tree
long long num = 0;
// Maximum number of keys/children in a node
#define M 4
// Minimum number of keys/children in a node
#define m 2
// Macro to find maximum and minimum of two numbers
#define MIN(a, b) ((a) <= (b) ? (a) : (b))
#define MAX(a, b) ((a) >= (b) ? (a) : (b))
typedef struct node node;
// Struct for storing rectangle points in 2D plane
typedef struct {
    long long int x1;
    long long  int x2;
    long long int y1;
    long long int y2;
} rectangle;
// Struct for storing nodes in Rtree
struct node {
    rectangle MBR;
    node *keys[M];
    int numKeys;
    node *parent;
};
// Struct for storing Rtree
typedef struct {
    node *root;
} rTree;

// Function declarations
node * createNode();
rTree *createTree();
bool isLeaf(node * Node);
void preOrderTraversal(node* curNode, int level);
long long int area(rectangle rect);
long long int increasedArea(rectangle parent, rectangle child);
node *ChooseLeaf(node *root, rectangle child);
node** quadraticSplit(node *Node,node * newkey);
int * pickSeeds(node *keys[]);
int* pickNext(node *newNode0, node *newNode1 ,node *keys[]);
void insertKey(rTree *root, rectangle rect);
void updateMBR(node *node);

// Function to create an empty Rtree
rTree *createTree() {
    rTree *rtree = (rTree *)malloc(sizeof(rTree));
    rtree->root = NULL;
    return rtree;
}
// Function to create an empty node
node *createNode() {
    node *n = (node *)malloc(sizeof(node));
    n->numKeys = 0;
    n->parent = NULL;
    return n;
}

// Function to calculate MBR of two rectangles
rectangle calculateMBR(rectangle a, rectangle b) {
    rectangle rec=  {
                MIN(a.x1, b.x1),
                MAX(a.x2, b.x2),
                MIN(a.y1, b.y1),
                MAX(a.y2, b.y2)};
    return rec;
}


// Function to updates MBR of all Parent Nodes
void updateMBR(node * Node){
    // Returning if node is NULL
    if (Node == NULL) return;
    
    // Creating a new rectangle to store the updated MBR
    rectangle newMBR=Node->keys[0]->MBR;
    
    // Loop to calculate MBR of keys in the node.
    for(int i=1;i<Node->numKeys;i++){
        newMBR.x1 = MIN(newMBR.x1, Node->keys[i]->MBR.x1);
        newMBR.x2 = MAX(newMBR.x2, Node->keys[i]->MBR.x2);
        newMBR.y1 = MIN(newMBR.y1, Node->keys[i]->MBR.y1);
        newMBR.y2 = MAX(newMBR.y2, Node->keys[i]->MBR.y2);
    }
    // setting updated MBR as key's MBR
    Node->MBR=newMBR;
    // Recursively calling updateMBR to update MBR of tree starting from Node uptill Root node
    updateMBR(Node->parent);
}


// Returns if the passed Node is the leaf Node or not 
bool isLeaf(node * Node) {
    // A node is a leaf node if number of keys in every child of the node is 0
    for( int i = 0; i< Node->numKeys; i++){
        if (Node->keys[i]->numKeys != 0){
            return false;
        }
    }
    return true;
}

// Calculates the area of the passed rectangle
long long int area(rectangle rect) {
    return ((rect.x2 - rect.x1) * (rect.y2 - rect.y1));
}

/*
        Increased area function 
    return the area increased for two rectangles
*/
long long int increasedArea(rectangle parent, rectangle child){
    int x1 = MIN(parent.x1, child.x1);
    int x2 = MAX(parent.x2, child.x2);
    int y1 = MIN(parent.y1, child.y1);
    int y2 = MAX(parent.y2, child.y2);
    return (area((rectangle){x1, x2, y1, y2}) - area(parent));
}


// Function for preorder traversal of the Rtree
void preOrderTraversal(node* curNode, int level) {
    // Prints the MBR of the entire root node (all its children).
    if(curNode==NULL||curNode->numKeys==0){
        return;
    }
    //if the node is the root Node 
    if(level == 0) {
        printf("\nMBR of Root Node: [%lld,%lld,%lld,%lld]", curNode->MBR.x1,curNode->MBR.y1,curNode->MBR.x2,curNode->MBR.y2);
    }
    // else if it's not the leaf Node print its MBR
    else if(!isLeaf(curNode)){
        printf("\nMBR of Internal Node: [%lld,%lld,%lld,%lld]", curNode->MBR.x1,curNode->MBR.y1,curNode->MBR.x2,curNode->MBR.y2);
    }

    // For each call, prints the DFS depth as well as all children associated with the current node.
    if(isLeaf(curNode)){
        num+=curNode->numKeys;
        printf("\nDFS Level: %d \tExternal Node with %d Keys\n", level, curNode->numKeys);
    }
    else{
        printf("\nDFS Level: %d \tInternal Node with %d Keys\n", level, curNode->numKeys);
    }
    for(int i = 0; i < curNode->numKeys; i++) {
        // if the point is a 2D point just print 2 coordinates
        if( curNode->keys[i]->MBR.x1== curNode->keys[i]->MBR.x2&& curNode->keys[i]->MBR.y1== curNode->keys[i]->MBR.y2)
            {   
                printf("[%lld,%lld]", curNode->keys[i]->MBR.x1,curNode->keys[i]->MBR.y1);
            }
        // else the point is a rectangle  print bottom left and top right coordinates of the rectangle
        else            
            printf("[%lld,%lld,%lld,%lld]", curNode->keys[i]->MBR.x1,curNode->keys[i]->MBR.y1,curNode->keys[i]->MBR.x2,curNode->keys[i]->MBR.y2);     
        if(i != curNode->numKeys - 1) printf(" , ");
    } 
    printf("\n\n");

    // Recursive DFS call to visit children of the current node.
    for(int i = 0; i < curNode->numKeys; i++) {
        preOrderTraversal( curNode->keys[i] , level + 1);
    }
}

/*
                   Choose Leaf Algorithm
        returns the leaf Node from the tree which is most optimal for insertion of the new key
        such that area increased will be the Minimum for the correspoding leaf Node
        if area increased is same in two nodes tie is broken by choosing node with less area             
*/
node *ChooseLeaf(node *root,rectangle rect){
    node *N=root;
    if(isLeaf(N))    // Checks if root is leaf
        return N;
    long long int minIncrease=increasedArea(N->keys[0]->MBR,rect);;
    node *bestnode=N->keys[0];
    // Finds the node whose area will be minimum on insert new key with rect as dimensions
    for(int i=1;i<N->numKeys;i++ ){                     
        long long int increaseTemp=increasedArea(N->keys[i]->MBR,rect);
        // If the current key causes area incease to be less than minincrease than current key becomes best node
        if(increaseTemp<minIncrease){
            bestnode=N->keys[i];
            minIncrease=increaseTemp;
        }
        // If both have same area increase then choose best node with which has area minimum of the two
        else if(increaseTemp==minIncrease){
            if(area(N->keys[i]->MBR)<area(bestnode->MBR)) {
                bestnode=N->keys[i];
                minIncrease=increaseTemp;
            }
        }
    }
    // Recursive call to get the leaf present in the children of bestnode
    return ChooseLeaf(bestnode,rect);
}

/*
       Adjust Tree Algorithm 
       Handles Splitted Nodes 
            (i))    If Splitted nodes has one of them as root then new root is created
            (ii))   If Parent of one of the splitted nodes have less than M keys then other node is inserted as newkey to the parent
            (iii)   If Parent laready has M keys than split parent and propogate spit upwards by calling AdjustNode recursively            
*/
void  AdjustTree(node * L, node * LL, rTree * tree){
    if(L==NULL)
    return;
    node *parent =L->parent;
    //Updating MBR of parent and its parent as L's MBR is updated
    updateMBR(parent);
    // Parent NULL signifies root node being Splitted
   if(parent==NULL){
      // Creating new root node
        node *newroot=createNode();
      // Storing splitted nodes in new root
        newroot->keys[0]=L;
        newroot->keys[1]=LL;
        newroot->numKeys=2;
      // Changing tree root to new root
        tree->root=newroot;
      // Setting parent of splitted nodes as root
        L->parent=newroot;
        LL->parent=newroot;
      // Updating MBR of new root
        updateMBR(newroot);
    }
    // If parent has keys less than M than insert in parent
    else if(parent->numKeys<M)
    {  
        // New splitted node is inserted into the parent as new key/entry
        parent->keys[parent->numKeys++]=LL;
        // Setting parent of new splitted node
        LL->parent=parent;
        // Updating MBR of parent and its parents
        updateMBR(parent);
    }
    // If parent has M keys than need to split parent node and propogate it upwards
    else{
        // Parent node is splitted with LL as key in one of the two splitted nodes
        node **splitnodes=quadraticSplit(parent,LL);
        // Splitnodes are propogated upwards
        AdjustTree(splitnodes[0],splitnodes[1],tree);     
        // MBR of splitnodes is updated recursively till they reach root    
        updateMBR(splitnodes[0]);
        updateMBR(splitnodes[1]);
    }     
}


/*
    Insert Key Algorithm           
*/
void insertKey(rTree *tree,rectangle rect){
    // set root variable as Tree's root node
    node *root=tree->root;
    /*  Case when tree is empty or root node is NULL
        Crete new node and set it as tree's root node if this is the case
    */
    if(root==NULL) {
        root=createNode();
        tree->root=root;
    }
    // Creating an empty node and setting its MBR as rect struct (Node to be inserted in the Rtree) 
    node *newKey=createNode();
    newKey->MBR=rect;
    // Calling choose leaf to select the leaf node in which new node will be inserted
    node *leaf=ChooseLeaf(root,rect);
    /*  Case if number of keys/children in the leaf node is less than the M (Maximum number of keys/children in a node)
        Directly insert new key in the leaf node and set it's parent as leaf node. Increment numKeys of the leaf node.
    */
    if(leaf->numKeys<M) {
        int index=leaf->numKeys;
        leaf->keys[index]=newKey;
        leaf->numKeys++;
        newKey->parent=leaf;
        updateMBR(leaf);
    }
    /*  Case if number of keys/children in the leaf node is equal to or greater than M (Maximum number of keys/children in a node)
        Call quadratic split on leaf node which will split the leaf node in two nodes L, LL of appropriate size.
        Call adjust tree on splitted nodes L and LL which will adjsut these nodes in the tree
    */
    else {
        node **splitnodes=quadraticSplit(leaf,newKey);       
        AdjustTree(splitnodes[0],splitnodes[1],tree);       
    }
}

/*
                                Pick Seeds Algorithm
        if  i & j are same then cotinue as same Key can't be inserted in both Nodes
        calculates waste area = MBR of 2 Keys at index i & j subtractes area of both Keys
        if the Waste area is > maximum Wasre area two nodes are stored in the array to be returned
        finally returns the two keys to be inserted in the Node  & newNode1
                
*/
int * pickSeeds(node *keys[]){
    // Creating seeds array to store intials keys of new nodes.
    int *seeds = (int *)malloc(sizeof(int)*2);
    long long wasteAreaMax = 0;
    // initializing elements of seeds array
    seeds[0]=0;
    seeds[1]=1;
    for (int i = 0; i < M+1; i++) {
        for (int j = 0; j < M+1; j++) {
            if (i==j){
                continue;
            }
            rectangle max = calculateMBR(keys[i]->MBR,keys[j]->MBR);
            long long wasteArea = llabs((max.x2 - max.x1) * (max.y2 - max.y1)) -
                                llabs((keys[i]->MBR.x2 - keys[i]->MBR.x1) * (keys[i]->MBR.y2 - keys[i]->MBR.y1)) -
                                llabs((keys[j]->MBR.x2 - keys[j]->MBR.x1) * (keys[j]->MBR.y2 - keys[j]->MBR.y1));

            if (llabs(wasteArea) > wasteAreaMax)
            {   wasteAreaMax = llabs(wasteArea);
                seeds[0] = i;
                seeds[1] = j;
            }
        }
    }
    return seeds;
}

/*
                    Quadratic cost algorithm
        Accepts the full Node and a New Node which has M+1th key
        Splits the Nodes into 2 Nodes in a optimal way such that MBR of both Nodes are the mimimum possible
        (i)     Creates a Keys array with M+1 entries  
        (ii)    Clears the keys of Node so that new keys can be inserted & newNode1 is created & number of keys is set to 0
        (iii)   Calls pick Seeds algorithm to get 2 keys initalizing each Node
        (iv)    While keys left > 0 pick Next algorithm is called to get the next key to be inserted & in which Node
        (v)     Checks if some Node will be left out with < m keys if yes then its inserted in that Node
                        else it is inserted according to the Node reutrned by the pick Next algorithm
        (vi)    Updates the MBR of the Nodes before next iteration
        (vii)   Finally sets the parent Node for all the keys in both nodes                
*/
node ** quadraticSplit(node *Node,node * newkey) {
    // keysLeft
    int keysLeft = M+1;
    node *keys[keysLeft];
    keys[0]=newkey;
    for(int i=1;i<keysLeft;i++){
        keys[i]= Node->keys[i-1];
    }

    int * seeds = pickSeeds(keys);

    for (int i = 0; i<Node->numKeys; i++){
        Node->keys[i] = NULL;
    }
    Node->numKeys = 0;
    Node->keys[0] = keys[seeds[0]];
    Node->numKeys++;
    node *newNode1 = createNode();
    newNode1->keys[0] = keys[seeds[1]];
    newNode1->numKeys++;
    keys[seeds[0]] = NULL;
    keys[seeds[1]] = NULL;

    //update MBR
    Node->MBR =Node->keys[0]->MBR;
    newNode1->MBR =newNode1->keys[0]->MBR;
    
    keysLeft -= 2;
    int i,j;
    i=j=1;
    while (keysLeft > 0)
    { 
        int *nextKeyGroupAndPosition = pickNext(Node, newNode1, keys);
        int a = nextKeyGroupAndPosition[0];
        int b = nextKeyGroupAndPosition[1];

        if ( (nextKeyGroupAndPosition[0] == 0 && keysLeft>(m-newNode1->numKeys)) || keysLeft<=(m-Node->numKeys) ) 
        {
            Node->keys[i] = keys[nextKeyGroupAndPosition[1]];
            //update MBR of both nodes after each iteration            
            rectangle newMBR0 =  calculateMBR(Node->MBR,keys[nextKeyGroupAndPosition[1]]->MBR);
            Node->MBR =newMBR0;
            Node->numKeys=++i; 
        }
        else
        {  
            newNode1->keys[j] = keys[nextKeyGroupAndPosition[1]];
            newNode1->keys[j]->parent = newNode1;
            rectangle newMBR1 = calculateMBR(newNode1->MBR,keys[nextKeyGroupAndPosition[1]]->MBR);
            newNode1->MBR =newMBR1;
            newNode1->numKeys=++j;      
        }
        // set key in orignal node to null so it's removed in next iteration
        keys[nextKeyGroupAndPosition[1]] = NULL;
        keysLeft--;
    }
    // return split nodes
    node ** splitNodes = (node **)malloc(sizeof(node *)*2);
    splitNodes[0]=Node;
    splitNodes[1]=newNode1;
    //set parent for all the child nodes
    for (int i = 0; i<splitNodes[0]->numKeys; i++){
        splitNodes[0]->keys[i]->parent = splitNodes[0];
    }
    for (int i = 0; i<splitNodes[1]->numKeys; i++){
        splitNodes[1]->keys[i]->parent = splitNodes[1];
    }
    return splitNodes;
}
/*
                        Pick Next Algorithm 
            returns the next key which has maximum d1 -d2
    where d1 is waste area for new MBR of Node 1 & current key in consideration - Node1 MBR 
    where d2 is waste area for new MBR of Node 2 & current key in consideration - Node2 MBR
    stores the key for which maximum of d1-d2 and the corresponding Node between Node1 & NOde2 where it should be inserted
    Returns the key and the coresp Node in which it should be inserted
                
*/

int* pickNext(node *newNode0, node *newNode1 ,node *keys[])
{
    int wasteAreaMax = 0;
    int * nextKeyGroupAndPosition = (int *)malloc(sizeof(int)*2);
    //iterating over all keys
    for (int i = 0; i < M+1; i++)
    {
            // if the node is NULL it is already selected
            if (keys[i] == NULL )
                continue;
            rectangle newMBR = calculateMBR(newNode0->MBR,keys[i]->MBR);
            int wasteArea1 = (newMBR.x2 - newMBR.x1) * (newMBR.y2 - newMBR.y1) -
                            (newNode0->MBR.x2 - newNode0->MBR.x1) * (newNode0->MBR.y2 - newNode0->MBR.y1);

            rectangle newMBR1 = calculateMBR(newNode1->MBR, keys[i]->MBR );

            int wasteArea2 = (newMBR1.x2 - newMBR1.x1) * (newMBR1.y2 - newMBR1.y1) -
                             (newNode1->MBR.x2 - newNode1->MBR.x1) * (newNode1->MBR.y2 - newNode1->MBR.y1);

            if (llabs(wasteArea2 - wasteArea1) >= wasteAreaMax)
            {   wasteAreaMax = llabs(wasteArea2 - wasteArea1);
                nextKeyGroupAndPosition[0] = (wasteArea2 - wasteArea1 <= 0) ? 1 : 0;
                nextKeyGroupAndPosition[1] = i;
            }
    }
    return nextKeyGroupAndPosition;
}


int main(int  argc, char ** argv){
    rTree *tree = createTree();
    FILE *fp;

    // Reading file name from command line argument.
    if(argc==1){
        // Default file name when the command line argument is not provided.
        printf("No file name input from command line.. \n Using default File Name large.txt \n");
        fp = fopen("data2.txt", "r");    
    }
    else{
        fp = fopen(argv[1], "r");
    }
    
    // Exiting the program if file does not exist.
    if (fp == NULL){ 
        printf("Error occured while reading the file");
        return 0;
    }
    // variable to store data points while reading the file
    long long x, y;
    // counter variable for the while loop.
    long long k = 0;

    // Reading the file and add the entries read from file one by one to the Rtree.
    while (fscanf(fp, "%lld %lld\n", &x, &y) == 2)
    {
        // Creating rectangle struct from the data read from the file.
        rectangle rect = {x, x, y, y};
        // Inserting rectangle struct to the Rtree.
        insertKey(tree,rect);
        // Incrementing counter for while loop
        k++;
    };
    
    printf("\nStarting New Traversal..\nRectangle format [x1,y1,x2,y2] 2D point format [x1,y1]\n");
    // Preorder traversal of Rtree.
    preOrderTraversal(tree->root,0);
    printf("Count of keys in leaf node : %lld", num);
    return 0;
}

