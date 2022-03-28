/*
*  File:        ptree-private.h
*  Description: Private functions for the PTree class
*  Date:        2022-03-06 03:30
*
*               DECLARE YOUR PTREE PRIVATE MEMBER FUNCTIONS HERE
*/

#ifndef _PTREE_PRIVATE_H_
#define _PTREE_PRIVATE_H_

/////////////////////////////////////////////////
// DEFINE YOUR PRIVATE MEMBER FUNCTIONS HERE
//
// Just write the function signatures.
//
// Example:
//
// Node* MyHelperFunction(int arg_a, bool arg_b);
//
/////////////////////////////////////////////////

double averageHueInArea(PNG& im, unsigned int x, unsigned int y, pair<unsigned int, unsigned int> ul);
double averageSatInArea(PNG& im, unsigned int x, unsigned int y, pair<unsigned int, unsigned int> ul);
double averageLumInArea(PNG& im, unsigned int x, unsigned int y, pair<unsigned int, unsigned int> ul);
double averageAlpInArea(PNG& im, unsigned int x, unsigned int y, pair<unsigned int, unsigned int> ul);
void renderHelper(PNG& im, Node *tree) const;
int sizePTree(Node *tree) const;
int amountNumLeaves(Node *tree) const;
void destroyTree(Node *tree);
Node* copyTree(Node *tree);
bool shouldPrune(Node *tree, double tolerance, Node *subroot);
void pruneTheTree(double tolerance, Node *&subroot);
void renderPlace(PNG& im, Node *node) const;
void flipHorizontalHelper(Node *&subroot);
void flipVerticalHelper(Node *&subroot);



#endif