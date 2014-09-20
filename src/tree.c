#include "tree.h"

char **freeClasses(char **classes, int count) {
  int i;

  for(i = 0; i < count; i++)
    free(classes[i]);

  free(classes);

  return NULL;
}

// TODO adapt for new implementation
/*node *freeTree(node *root) {
  leaf *leaves;
  int i;

  //for(i = 0; i < 8192; i++)
  //  if(root[i].leaves)
  //  printf("Leaves pointers: %p->%p->%p\n", root[i].leaves, root[i].leaves->value, root[i].leaves->index);

  //for(i = 0; root[i].leaves == NULL; i++) {
  //  printf("Done rite\n");
  //  if(root[i].leaves) {
  i = 0;
  while(!root[i].leaves)
    i++;

  leaves = root[i].leaves;

  free(leaves->value);
  free(leaves->index);
  free(leaves);
  //    break;
  //  }
  //}

  free(root);
*/
  /*while(currentLeaf) {
    nextLeaf = currentLeaf->next;
    free(currentLeaf);
    currentLeaf = nextLeaf;
  }

  if(NULL != root->leftChild)
    freeTree(root->leftChild);
  if(NULL != root->rightChild)
    freeTree(root->rightChild);

  free(root);
  */
  //return NULL;
//}
