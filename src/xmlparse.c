#include <string.h>
#include "tree.h"

/* TODO error handling */
void *resizeOrAllocStructArray(void *old, int *count, size_t size) {
  void *new_array = NULL;

  if(old) {
    new_array = calloc(2 * *count, size);

    memcpy(new_array, old, *count * size);

    *count <<= 1;

    if(*count < 0) {
      printf("EE|| %s: %d: overflow.\n", __FILE__, __LINE__);
      abort();
    }

    free(old);
  } else {
    *count = 1024;
    new_array = calloc(*count, size);
  }

  return new_array;
}

node *parseXMLTree(const char *filename, char ***classes, int *classCount, leaf **leaves) {
  node *treeRoot;
  xmlDoc *xmlFile = NULL;
  xmlNode *fileRoot = NULL;  /* TODO */

  /* XXX is hard-coding the format sensible? */
  //* TODO check xmlParserOption (third arg) */
  /* libxml whines when moving these two allocs to their own function */
  if(NULL == (xmlFile = xmlReadFile(filename, "UTF-8", 0))) {
    printf("EE|| %s:%d: could not read file '%s'\n", __FILE__, __LINE__, filename);
    return NULL;
  }

  if(NULL == (fileRoot = xmlDocGetRootElement(xmlFile))) {
    printf("EE|| %s:%d: could not read file '%s'\n", __FILE__, __LINE__, filename);
    return NULL;
  }

  /* XXX quick test */
  printf("Testing node: root name: %s\n", fileRoot->name);

  /* TODO parse classes*/
  *classes = parseClasses(fileRoot, classCount);

  /* TODO parse tree into memory*/
  treeRoot = parseTree(fileRoot, *classes, *classCount, leaves);

  //printTree(treeRoot, 0);

  // free tree
  //treeRoot = freeTree(treeRoot);

  /* Clean up libxml2 memory */
  xmlFreeDoc(xmlFile);
  xmlCleanupParser();

  return treeRoot;
}

xmlNode *findTreeRoot(xmlNode *current) {
  /* TODO make pretty (AKA not shite) */
  while(current && strcmp(TREEROOTNAME, (char*) current->name))
    current = current->next;
  if(current)
    current = current->children;
  while(current && strcmp(CLASSIFIERROOTNAME, (char*) current->name))
    current = current->next;
  if(current)
    current = current->children;
  while(current && strcmp(DECISIONTREEROOTNAME, (char*) current->name))
    current = current->next;
  /*if(current)
    current= current->children;*/

  if(!current) {
    printf("EE|| %s:%d: could not find decision tree root node\n", __FILE__, __LINE__);
    return NULL;
  }

  return current;

}

node xmlToNode(xmlNode *current) {
  //fprintf(stderr, "lordylordy %p", (void*) current);
  node new_node = NEW_NODE;// (node*) calloc(1, sizeof(node));
  xmlAttrPtr attr = NULL;
  char *attrVal;
  char *tok;

  //if(!new_node)
  //  return NULL;

  attr = current->properties;

  if(NULL == attr)
    new_node.type = LEAF;
  else {
    //featurename
    attrVal = (char*) xmlGetProp(current, attr->name);
    tok = strtok(attrVal, "**");

    if(!(strcmp("Coronal", tok)))
      new_node.type = CORONAL;
    else if(!(strcmp("LongRangeContext", tok))) {
      new_node.type = LONGRANGECONTEXT;

      //function arguments
      tok = strtok(NULL, "|");
      new_node.arguments[0] = atoi(tok);
      tok = strtok(NULL, "|");
      new_node.arguments[1] = atoi(tok);
      tok = strtok(NULL, "*");
      new_node.arguments[2] = atoi(tok);
    }
    else {
      // TODO error
      //free(new_node);
      free(attrVal);
      printf("EE| Error at %s: %d\n", __FILE__, __LINE__);
      abort();
    }

    free(attrVal);
  }

  if(new_node.type == LEAF) {
    // TODO parse actual Lk leaves
  }
  else {
    attr = attr->next;
    attrVal = (char*) xmlGetProp(current, attr->name);
    new_node.threshold = strtof(attrVal, NULL);
    free(attrVal);

    /* XXX not necessary, value not used (leftProportion)
    attr = attr->next;
    attrVal = (char*) xmlGetProp(current, attr->name);
    new_node.value = strtod(attrVal, NULL);
    free(attrVal);*/
  }

  return new_node;
}


/*TODO*/
int parseLeaves(xmlNode *curr, node *root, char **classes, int classCount, int leaf_pos, leaf *leaf_array, int *leaf_size) {
  xmlNode *xml_leaves = curr->children;
  xmlAttrPtr attr = NULL;
  char *attrVal;
  int count = 0;
  int i;

  int pos = leaf_pos;

  //printf("Entering parseLeaves: %p<-%p->%p, sized %d, position %d.\n", leaf_array->value, leaf_array, leaf_array->index, *leaf_size, pos);

  while(xml_leaves) {
    //non-'text' node
    if(xml_leaves->type == XML_ELEMENT_NODE) {
      attr = xml_leaves->properties;
      //TODO error checking
      //nextLeaf = (leaf*)calloc(1, sizeof(leaf));

      //if(NULL == root->leaves)
      //  root->leaves = nextLeaf;
      //else
      //  currentLeaf->next = nextLeaf;

      if(pos >= *leaf_size) {
        leaf_array->value = resizeOrAllocStructArray(leaf_array->value, leaf_size, sizeof(double));
        *leaf_size >>= 1;
        leaf_array->index = resizeOrAllocStructArray(leaf_array->index, leaf_size, sizeof(int));
      }

      while(attr) {
        if(!(strcmp((char*) attr->name, "Class"))) {
          attrVal = (char*) xmlGetProp(xml_leaves, attr->name);

          for(i = 0; i < classCount; i++) {
            if(!(strcmp((char*) attrVal, classes[i]))) {
              leaf_array->index[pos] = i;
              break;
            }
          }

          free(attrVal);
        }
        else if(!(strcmp((char*) attr->name, "Val"))) {
          attrVal = (char*) xmlGetProp(xml_leaves, attr->name);

          leaf_array->value[pos] = strtod(attrVal, NULL);

          free(attrVal);
        }

        attr = attr->next;
      }

      //currentLeaf = nextLeaf;
      //nextLeaf = nextLeaf->next;
      pos++;
      count++;
    }

    xml_leaves = xml_leaves->next;
  }

  root->arguments[2] = count;

  return pos;
}

/*Build decisionTreeNodes*/
//TODO initial step left_child_array and right_child_array implementation
int buildTree(xmlNode *curr, char **classes, int class_count, node **array,
    int *pos, int *array_size, int **lc, int **rc, leaf *leaves) {
  static int leaf_count = 0;
  static int leaf_pos = 0;
  static int leaf_size = 0;
  //set own pos and then increment global pos
  int self_pos = (*pos)++;
  int child_pos;
  node *self = NULL;
  xmlNode *child = curr->children;

  if(*pos >= *array_size) {
    //printf("TRUE: %d >= %d\n", *pos, *array_size);
    *array = (node*) resizeOrAllocStructArray(*array, array_size, sizeof(node));
    //*array_size >>= 1;
    //*lc = (int*) resizeOrAllocStructArray(*lc, array_size, sizeof(int));
    //*array_size >>= 1;
    //*rc = (int*) resizeOrAllocStructArray(*rc, array_size, sizeof(int));
  }

  (*array)[self_pos] = xmlToNode(curr);
  self = &((*array)[self_pos]);

  if(self->type == LEAF) {
    self->arguments[0] = leaf_count++;
    self->arguments[1] = leaf_pos;
    leaf_pos = parseLeaves(curr, self, classes, class_count, leaf_pos, leaves, &leaf_size);
    self->arguments[2] = leaf_pos - self->arguments[1];
  }
  else {
    while(child && strcmp((char*) child->name, DECISIONTREENODENAME))
        child = child->next;

    //TODO error handling
    if(!child)
      printf("EE|| Error in %s, line %d: child pointer is NULL\n", __FILE__, __LINE__);

    child_pos = buildTree(child, classes, class_count, array, pos, array_size, lc, rc, leaves);
    //(*lc)[self_pos] = child_pos;
    (*array)[self_pos].leftChild = child_pos;

    child = child->next;
    while(child && strcmp((char*) child->name, DECISIONTREENODENAME))
        child = child->next;

    //TODO error handling
    if(!child)
      printf("EE|| Error in %s, line %d: child pointer is NULL\n", __FILE__, __LINE__);

    child_pos = buildTree(child, classes, class_count, array, pos, array_size, lc, rc, leaves);
    //(*rc)[self_pos] = child_pos;
    (*array)[self_pos].rightChild = child_pos;
  }

  return self_pos;
}

/*void setChildNodes(node *array, int *lc, int *rc, int array_size, leaf *leaves) {
  int i;

  for(i = 0; i <= array_size; i++) {
    array[i].leftChild = array[lc[i]];
    array[i].rightChild = array[rc[i]];



    if(array[i].type == LEAF)
      array[i].leaves = leaves;
  }

}*/

/* TODO */
node *parseTree(xmlNode *fileRoot, char **classes, int classCount, leaf **leaves) {
  node *root = NULL;// = calloc(1, sizeof(node));
  xmlNode *iter = findTreeRoot(fileRoot->children);


  printf("%s\n", (char*) iter->name);

  iter = iter->children;

  //xmlAttrPtr attr;
  int i;
  for(; iter->next && iter->next->next; iter = iter->next) {
    if(iter->type == XML_ELEMENT_NODE) {
      for(i = 0; i < 0; i++)
        printf(" ");
      printf("%d : %s", iter->line, (char*) iter->name);
      //for(attr = iter->properties; NULL != attr; attr = attr->next)
      //  printf(" : %s (%s)", attr->name, xmlGetProp(iter, attr->name));
      printf("\n");
    }
    //if(recurse)
   //   print_element_names(iter->children, 1, 1, 0);
  }

  printf("%s\n", (char*) iter->name);

  node **array = (node**) calloc(1, sizeof(node*));
  if(!array) {
    printf("EE| Error at %s: %d.\n", __FILE__, __LINE__);
    abort();
  }

  int *pos = (int*) calloc(1, sizeof(int));
  int *array_size = (int*) calloc(1, sizeof(int));
  //int **lc = (int**) calloc(1, sizeof(int*));
  //int **rc = (int**) calloc(1, sizeof(int*));

  int **lc = NULL, **rc = NULL;


  *leaves = (leaf*) calloc(1, sizeof(leaf));

  buildTree(iter, classes, classCount, array, pos, array_size, lc, rc, *leaves);
  //setChildNodes(*array, *lc, *rc, (*pos) - 1, leaves);
  root = *array;

  free(lc);
  free(rc);
  free(array);
  free(pos);
  free(array_size);

  return root;
}

xmlNode *findClassRoot(xmlNode *current) {
  /* TODO make pretty (AKA not shite) */
  while(current && strcmp(TREEROOTNAME, (char*) current->name))
    current = current->next;
  if(current)
    current = current->children;
  while(current && strcmp(CLASSIFIERROOTNAME, (char*) current->name))
    current = current->next;
  if(current)
    current = current->children;

  if(!current) {
    printf("EE|| %s:%d: could not find decision tree root node\n", __FILE__, __LINE__);
    return NULL;
  }

  return current;

}
/* TODO */
char **parseClasses(xmlNode *fileRoot, int *count) {
  char **classes = NULL;
  xmlNode *iter = findClassRoot(fileRoot->children);
  xmlNode *reset = iter;
  int i;

  //iter = iter->children;
  //while(iter) {
  //  printf("TESTI: %s\n", (char*) iter->name);
  //  iter = iter->next;
  //}

  for(i = 0; iter; iter = iter->next) {
    if(!(strcmp(PRIORITYNODE, (char*) iter->name)))
      break;
    if(strcmp(CLASSNAME, (char*) iter->name))
      continue;

    i++;
  }

  printf("--\n%d\n--\n", i);

  classes = (char**) malloc(i * sizeof(char*));

  iter = reset;

  for(i = 0; iter; iter = iter->next) {
    if(!(strcmp(PRIORITYNODE, (char*) iter->name)))
      break;
    if(strcmp(CLASSNAME, (char*) iter->name))
      continue;

    /*
    char *content = (char*) xmlNodeGetContent(iter);
    classes[i] = strdup(content);
    xmlFree(content);
    */
    classes[i] = (char*) xmlNodeGetContent(iter);

    i++;
  }

  //if(!iter) {
  //  printf("EE|| %s:%d: error parsing classes (node is NULL)\n", __FILE__, __LINE__);
  //  return NULL;
  //}

  #ifdef DEBUG_LOADS
  int j;
  for(j = 0; j < i; j++)
    printf("Class found: %s\n", classes[j]);
  #endif

  *count = i;

  return classes;
}
