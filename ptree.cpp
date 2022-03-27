kkl/*
*  File:        ptree.cpp
*  Description: Implementation of a partitioning tree class for CPSC 221 PA3
*  Date:        2022-03-03 01:53
*
*               ADD YOUR PRIVATE FUNCTION IMPLEMENTATIONS TO THE BOTTOM OF THIS FILE
*/

#include "ptree.h"
#include "hue_utils.h" // useful functions for calculating hue averages

using namespace cs221util;
using namespace std;

// The following definition may be convenient, but is not necessary to use
typedef pair<unsigned int, unsigned int> pairUI;

/////////////////////////////////
// PTree private member functions
/////////////////////////////////

/*
*  Destroys all dynamically allocated memory associated with the current PTree object.
*  You may want to add a recursive helper function for this!
*  POST: all nodes allocated into the heap have been released.
*/
void PTree::Clear() {
  destroyTree(root->A);
  destroyTree(root->B);
  root->A = NULL;
  root->B = NULL;
  
 
  
}

/*
*  Copies the parameter other PTree into the current PTree.
*  Does not free any memory. Should be called by copy constructor and operator=.
*  You may want a recursive helper function for this!
*  PARAM: other - the PTree which will be copied
*  PRE:   There is no dynamic memory associated with this PTree.
*  POST:  This PTree is a physically separate copy of the other PTree.
*/
void PTree::Copy(const PTree& other) {
  root = copyTree(other.root);
  
  
}

/*
*  Private helper function for the constructor. Recursively builds the tree
*  according to the specification of the constructor.
*  You *may* change this if you like, but we have provided here what we
*  believe will be sufficient to use as-is.
*  PARAM:  im - full reference image used for construction
*  PARAM:  ul - upper-left image coordinate of the currently building Node's image region
*  PARAM:  w - width of the currently building Node's image region
*  PARAM:  h - height of the currently building Node's image region
*  RETURN: pointer to the fully constructed Node
*/
Node* PTree::BuildNode(PNG& im, pair<unsigned int, unsigned int> ul, unsigned int w, unsigned int h) {


if (w == 1 && h==1) {

 //cout<<ul.first;
  //cout<<ul.second;
  //cout<< "reached base"<<endl;
  //cout<<" ";
  //cout<<ul.second<<endl;
  Node *baseNode = new Node();
  baseNode->upperleft = ul;
  baseNode->width = 1;
  baseNode->height = 1;
  baseNode->avg = *(im.getPixel(ul.first, ul.second));
  baseNode->A = NULL;
  baseNode->B = NULL;
  //cout<<"reached before return of base"<<endl;
  return baseNode;
 } else {
  Node *generalNode = new Node();
  generalNode->upperleft = ul;
  generalNode->width = w;
  generalNode->height = h;
  generalNode->avg.h = averageHueInArea(im, w, h, ul);
  generalNode->avg.s = averageSatInArea(im, w, h, ul);
  generalNode->avg.l = averageLumInArea(im, w, h, ul);
  generalNode->avg.a = averageAlpInArea(im, w, h, ul);
  //cout<<"reached after general node assignments"<<endl;
  


  if (h <= w) {
    if (w%2 == 0) {
      //cout<<w;
      //cout<<h;
      //cout<<"reached before even width children assignment"<<endl;
      pair<unsigned int, unsigned int> pair1 = make_pair(ul.first + w/2, ul.second);
      generalNode->A = BuildNode(im, ul, w/2, h);
      generalNode->B = BuildNode(im, pair1, w/2, h); 
      //cout<<"reached after even width children assignment"<<endl; 
    
    
    } else {
      //cout<<w;
      //cout<<h;
      //cout<<"reached before odd width children assignment"<<endl;
       pair<unsigned int, unsigned int> pair1 = make_pair(ul.first + w/2, ul.second);
       generalNode->A = BuildNode(im, ul, w/2, h);
       generalNode->B = BuildNode(im, pair1, (w/2) + 1, h); 
       //cout<<"reached after odd width children assignment"<<endl;
    }
  } else  {
    if (h%2 == 0) {
      //cout<<w;
      //cout<<h;
      //cout<<"reached before even height children assignment"<<endl;
      pair<unsigned int, unsigned int> pair1 = make_pair(ul.first, ul.second + h/2);
      generalNode->A = BuildNode(im, ul, w, h/2);
      generalNode->B = BuildNode(im, pair1, w, h/2); 
      //cout<<"reached after even height children assignment"<<endl;  
    } else {
      //cout<<w;
      //cout<<h;
      //cout<<"reached before odd height children assignment"<<endl;  
      pair<unsigned int, unsigned int> pair1 = make_pair(ul.first, ul.second + h/2);
       generalNode->A = BuildNode(im, ul, w, h/2);
       generalNode->B = BuildNode(im, pair1, w, (h/2) + 1); 
       //cout<<"reached after odd height children assignment"<<endl;  
    }

  }
  //cout<<"reached before general node return"<<endl;  
  return generalNode;
}
}

////////////////////////////////
// PTree public member functions
////////////////////////////////

/*
*  Constructor that builds the PTree using the provided PNG.
*
*  The PTree represents the sub-image (actually the entire image) from (0,0) to (w-1, h-1) where
*  w-1 and h-1 are the largest valid image coordinates of the original PNG.
*  Each node corresponds to a rectangle of pixels in the original PNG, represented by
*  an (x,y) pair for the upper-left corner of the rectangle, and two unsigned integers for the
*  number of pixels on the width and height dimensions of the rectangular sub-image region the
*  node defines.
*
*  A node's two children correspond to a partition of the node's rectangular region into two
*  equal (or approximately equal) size regions which are either tiled horizontally or vertically.
*
*  If the rectangular region of a node is taller than it is wide, then its two children will divide
*  the region into vertically-tiled sub-regions of equal height:
*  +-------+
*  |   A   |
*  |       |
*  +-------+
*  |   B   |
*  |       |
*  +-------+
*
*  If the rectangular region of a node is wider than it is tall, OR if the region is exactly square,
*  then its two children will divide the region into horizontally-tiled sub-regions of equal width:
*  +-------+-------+
*  |   A   |   B   |
*  |       |       |
*  +-------+-------+
*
*  If any region cannot be divided exactly evenly (e.g. a horizontal division of odd width), then
*  child B will receive the larger half of the two subregions.
*
*  When the tree is fully constructed, each leaf corresponds to a single pixel in the PNG image.
* 
*  For the average colour, this MUST be computed separately over the node's rectangular region.
*  Do NOT simply compute this as a weighted average of the children's averages.
*  The functions defined in hue_utils.h and implemented in hue_utils.cpp will be very useful.
*  Computing the average over many overlapping rectangular regions sounds like it will be
*  inefficient, but as an exercise in theory, think about the asymptotic upper bound on the
*  number of times any given pixel is included in an average calculation.
* 
*  PARAM: im - reference image which will provide pixel data for the constructed tree's leaves
*  POST:  The newly constructed tree contains the PNG's pixel data in each leaf node.
*/
PTree::PTree(PNG& im) {
  pair<unsigned int, unsigned int> pair1 = make_pair(0,0);
  root = BuildNode(im, pair1, im.width(), im.height());
  
}

/*
*  Copy constructor
*  Builds a new tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  This tree is constructed as a physically separate copy of other tree.
*/
PTree::PTree(const PTree& other) {
  Copy(other);
  
}

/*
*  Assignment operator
*  Rebuilds this tree as a copy of another tree.
*
*  PARAM: other - an existing PTree to be copied
*  POST:  If other is a physically different tree in memory, all pre-existing dynamic
*           memory in this tree is deallocated and this tree is reconstructed as a
*           physically separate copy of other tree.
*         Otherwise, there is no change to this tree.
*/
PTree& PTree::operator=(const PTree& other) {
  if (this != &other) {
    Clear();
    Copy(other);
  }

   return *this;
}

/*
*  Destructor
*  Deallocates all dynamic memory associated with the tree and destroys this PTree object.
*/
PTree::~PTree() {
  Clear();

}

/*
*  Traverses the tree and puts the leaf nodes' color data into the nodes'
*  defined image regions on the output PNG.
*  For non-pruned trees, each leaf node corresponds to a single pixel that will be coloured.
*  For pruned trees, each leaf node may cover a larger rectangular region that will be
*  entirely coloured using the node's average colour attribute.
*
*  You may want to add a recursive helper function for this!
*
*  RETURN: A PNG image of appropriate dimensions and coloured using the tree's leaf node colour data
*/
PNG PTree::Render() const {

  PNG *toBeReturned = new PNG(root->width, root->height);
  renderHelper(*toBeReturned, root);

 
  return *toBeReturned;
}

/*
*  Trims subtrees as high as possible in the tree. A subtree is pruned
*  (its children are cleared/deallocated) if ALL of its leaves have colour
*  within tolerance of the subtree root's average colour.
*  Pruning criteria should be evaluated on the original tree, and never on a pruned
*  tree (i.e. we expect that Prune would be called on any tree at most once).
*  When processing a subtree, you should determine if the subtree should be pruned,
*  and prune it if possible before determining if it has subtrees that can be pruned.
* 
*  You may want to add (a) recursive helper function(s) for this!
*
*  PRE:  This tree has not been previously pruned (and is not copied/assigned from a tree that has been pruned)
*  POST: Any subtrees (as close to the root as possible) whose leaves all have colour
*        within tolerance from the subtree's root colour will have their children deallocated;
*        Each pruned subtree's root becomes a leaf node.
*/
void PTree::Prune(double tolerance) {
pruneTheTree(tolerance, root);
  
}

/*
*  Returns the total number of nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::Size() const {

  return sizePTree(root);
}

/*
*  Returns the total number of leaf nodes in the tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*/
int PTree::NumLeaves() const {
  // replace the line below with your implementation
  return amountNumLeaves(root);
}

/*
*  Rearranges the nodes in the tree, such that a rendered PNG will be flipped horizontally
*  (i.e. mirrored over a vertical axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped horizontally.
*/
void PTree::FlipHorizontal() {
  flipHorizontalHelper(root);
  
  
}

/*
*  Like the function above, rearranges the nodes in the tree, such that a rendered PNG
*  will be flipped vertically (i.e. mirrored over a horizontal axis).
*  This can be achieved by manipulation of the nodes' member attribute(s).
*  Note that this may possibly be executed on a pruned tree.
*  This function should run in time linearly proportional to the size of the tree.
*
*  You may want to add a recursive helper function for this!
*
*  POST: Tree has been modified so that a rendered PNG will be flipped vertically.
*/
void PTree::FlipVertical() {
  flipVerticalHelper(root);
  
}

/*
    *  Provides access to the root of the tree.
    *  Dangerous in practice! This is only used for testing.
    */
Node* PTree::GetRoot() {
  return root;
}

//////////////////////////////////////////////
// PERSONALLY DEFINED PRIVATE MEMBER FUNCTIONS
//////////////////////////////////////////////

double PTree::averageHueInArea(PNG& im, unsigned int x, unsigned int y, pair<unsigned int, unsigned int> ul) {
double xComp = 0;
double yComp = 0;
int total = x*y; 
for (unsigned int j = 0; j < x; j++) {
    for (unsigned int z = 0; z < y; z++) {
      HSLAPixel *pix = im.getPixel(j + ul.first, z + ul.second);
      xComp += Deg2X(pix->h);
      yComp += Deg2Y(pix->h);
    }
}
double finalxComp =  xComp/total;
double finalyComp = yComp/total;
return XY2Deg(finalxComp, finalyComp);
}


double PTree::averageSatInArea(PNG& im, unsigned int x, unsigned int y, pair<unsigned int, unsigned int> ul) {
  double place = 0; 
  int total = x*y;
  for (unsigned int j = 0; j < x; j++) {
    for (unsigned int z = 0; z < y; z++) {
      HSLAPixel *pix = im.getPixel(j + ul.first, z+ ul.second);
      place += pix->s; 
    }
  }
  return place/total;

}

double PTree::averageLumInArea(PNG& im, unsigned int x, unsigned int y,  pair<unsigned int, unsigned int> ul) {
  double place = 0; 
  int total = x*y;
  for (unsigned int j = 0; j < x; j++) {
    for (unsigned int z = 0; z < y; z++) {
      HSLAPixel *pix = im.getPixel(j + ul.first, z + ul.second);
      place += pix->l; 
    }
  }
  return place/total;

}

double PTree::averageAlpInArea(PNG& im, unsigned int x, unsigned int y,  pair<unsigned int, unsigned int> ul) {
  double place = 0; 
  int total = x*y;
  for (unsigned int j = 0; j < x; j++) {
    for (unsigned int z = 0; z < y; z++) {
      HSLAPixel *pix = im.getPixel(j + ul.first, z + ul.second);
      place += pix->a; 
    }
  }
  return place/total;

}

 void PTree::renderHelper(PNG& im, Node *node) const {

   if (node->A == NULL && node->B == NULL) {
     if (node->width != 1 || node->height != 1) {
       renderPlace(im, node);
     } else {
     HSLAPixel *toBeAltered = im.getPixel(node->upperleft.first, node->upperleft.second);
     *toBeAltered = node->avg;
     }
   } else {
     renderHelper(im, node->B);
     renderHelper(im, node->A);

   }
  
}

void PTree::renderPlace(PNG& im, Node *node) const {
  for (unsigned int j = 0; j < node->width; j++) {
    for (unsigned int z = 0; z < node->height; z++) {
      HSLAPixel *toBeAltered = im.getPixel(j + node->upperleft.first, z + node->upperleft.second);
      *toBeAltered = node->avg;
    }
  }
}

int PTree::sizePTree(Node *tree) const {

  if (tree != NULL) {

    int ret1 = sizePTree(tree->A);
    int ret2 = sizePTree(tree->B);

    return 1 + ret1 + ret2;
  } else {
    return 0;
  }
}

int PTree::amountNumLeaves(Node *tree) const {
  if (tree == NULL) {
    return 0;
  } else if (tree->A == NULL && tree->B == NULL) {
    return 1;
  } else {
    return amountNumLeaves(tree->A) + amountNumLeaves(tree->B);
  }


}

void PTree::destroyTree(Node *&tree) {

  if (tree == NULL) {
        return;
  }

    destroyTree(tree->A);
    destroyTree(tree->B);
    delete tree;
}

Node* PTree::copyTree(Node *tree) {
  
    if (tree == NULL) {
        return NULL;
    } else {


    Node* curr = new Node();
    curr->upperleft = tree->upperleft;
    curr->width = tree->width;
    curr->height = tree->height;
    curr->avg = tree->avg;
    curr->A = copyTree(tree->A);
    curr->B = copyTree(tree->B);
    return curr;
    }

}

bool PTree::shouldPrune(Node *tree, double tolerance, Node *subroot) {
  if (tree->A == NULL && tree->B == NULL && subroot != tree) {
    if (subroot->avg.dist(tree->avg) <= tolerance) {
      return true;
    } else {
    return false;
    }
  } else {
    return shouldPrune(tree->A, tolerance, subroot) && shouldPrune(tree->B, tolerance, subroot);
  }
}

void PTree::pruneTheTree(double tolerance, Node *&subroot) {
  if (subroot->A == NULL && subroot->B == NULL) {
    return;
  } else if (shouldPrune(subroot, tolerance, subroot)) {
    destroyTree(subroot->A);
    destroyTree(subroot->B);
    subroot->A = NULL;
    subroot->B = NULL;
  } else {
  pruneTheTree(tolerance, subroot->A);
  pruneTheTree(tolerance, subroot->B);
    }
  }

  void PTree::flipHorizontalHelper(Node *&subroot) {
    if (subroot->A == NULL && subroot->B == NULL) {
      return;

    } else {

      if(subroot->width >= subroot->height) {
        unsigned int addFactor;
        if (subroot->width%2 == 0) {
          addFactor = subroot->width/2;
        } else {
          addFactor = subroot->width/2 + 1;
        }
      pair<unsigned int, unsigned int> tempB = make_pair(subroot->upperleft.first + addFactor, subroot->upperleft.second);
      subroot->B->upperleft = subroot->upperleft;
      subroot->A->upperleft = tempB;
      Node *temp = subroot->A;
      subroot->A = subroot->B;
      subroot->B = temp;
    
    } else if (subroot->height > subroot->width) {
      if (subroot->A->height > subroot->B->height) {
        unsigned int addFactor = subroot->height/2 + 1;
        pair<unsigned int, unsigned int> tempB = make_pair(subroot->upperleft.first, subroot->upperleft.second + addFactor);
        subroot->A->upperleft = subroot->upperleft;
        subroot->B->upperleft = tempB;
      } else {
      unsigned int addFactor = subroot->height/2;
      pair<unsigned int, unsigned int> tempB = make_pair(subroot->upperleft.first, subroot->upperleft.second + addFactor);
      subroot->A->upperleft = subroot->upperleft;
      subroot->B->upperleft = tempB;
      }


    }
      flipHorizontalHelper(subroot->A);
      flipHorizontalHelper(subroot->B);

  }
  }

  void PTree::flipVerticalHelper(Node *&subroot) {
    if (subroot->A == NULL && subroot->B == NULL) {
      return;

    } else {

      if(subroot->width < subroot->height) {
        unsigned int addFactor;
        if (subroot->height%2 == 0) {
          addFactor = subroot->height/2;
        } else {
          addFactor = subroot->height/2 + 1;
        }
      pair<unsigned int, unsigned int> tempB = make_pair(subroot->upperleft.first, subroot->upperleft.second + addFactor);
      subroot->B->upperleft = subroot->upperleft;
      subroot->A->upperleft = tempB;
      Node *temp = subroot->A;
      subroot->A = subroot->B;
      subroot->B = temp;
    
    } else if (subroot->height <= subroot->width) {
      if (subroot->A->width > subroot->B->width) {
        unsigned int addFactor = subroot->width/2 + 1;
        pair<unsigned int, unsigned int> tempB = make_pair(subroot->upperleft.first + addFactor, subroot->upperleft.second);
        subroot->A->upperleft = subroot->upperleft;
        subroot->B->upperleft = tempB;
      } else {
      unsigned int addFactor = subroot->width/2;
      pair<unsigned int, unsigned int> tempB = make_pair(subroot->upperleft.first + addFactor, subroot->upperleft.second);
      subroot->A->upperleft = subroot->upperleft;
      subroot->B->upperleft = tempB;
      }
    }
      flipVerticalHelper(subroot->A);
      flipVerticalHelper(subroot->B);
  }   
  }
  

  





