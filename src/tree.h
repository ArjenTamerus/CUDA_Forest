/*
 * @Author Arjen Tamerus
 *
 * tree.h
 *
 * Define tree structure
 *
 *
 */

#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define TREEFILE "../../data/tree_0.xml"
#define TREEROOTNAME "Trees"
#define CLASSIFIERROOTNAME "DecisionTreeClassifier"
#define DECISIONTREEROOTNAME "DecisionTree"
#define DECISIONTREENODENAME "DecisionTreeNode"
#define CLASSNAME "ClassName"
#define PRIORITYNODE "PriorProb"
#define TEXTNODE "text"


#define LONGRANGECONTEXT 1
#define CORONAL 2
#define LEAF 3

#define leftChild children[0]
#define rightChild children[1]

/* TODO implement leaf index in decisionTreeNode */

struct leafNode {
  float* value;
  short* index;
};

typedef struct leafNode leaf;

/* TODO separate sub-structs for leaf, longrangecontext etc*/
struct decisionTreeNode {
  //struct leafNode *leaves;
  //in leaf: value, in coronal & LRC: Treshold;
  //double value;
  //in leaf: nothing, in LRC: leftproportion, in coronal: leftproportion (but
  //should be int
  float threshold;
  //For LRC; should check but could probably be reduced to shorts or even chars
  //in order to save memory
  //TODO hold leaf number in first arg, leaf array index in 2nd, leaf count in 3d, will this fit in short?

  short children[2];
  char arguments[3];
  //short leftChild;
  //short rightChild;
  char type;
};

typedef struct decisionTreeNode node;

#define NEW_NODE {0.0, {0, 0}, {0, 0, 0}, 0};

node *parseXMLTree(const char* filename, char ***classes, int *classCount, leaf **leaves);
void testTree();
void printTree(node *root, int depth);
xmlNode *findTreeRoot(xmlNode *current);
node *parseTree(xmlNode *fileRoot, char **classes, int classCount, leaf **leaves);
char **parseClasses(xmlNode *fileRoot, int *count);
node *freeTree(node *root);
char **freeClasses(char **classes, int classCount);


#endif
