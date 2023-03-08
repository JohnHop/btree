#include "btree.h"

#define CORMER_ORDER 3

int main(void)
{
  Node* root = new Node{'P'};
  root->leaf = false;

  Node* l1 = new Node{ {'C', 'G', 'M'} };
  root->children[0] = l1;
  l1->leaf = false;

  Node* r1 = new Node{ {'T', 'X'} };
  root->children[1] = r1;
  r1->leaf = false;

  Node* l2AB = new Node{ {'A', 'B'} };
  Node* l2DEF = new Node{ {'D', 'E', 'F'} };
  Node* l2JKL = new Node{ {'J', 'K', 'L'} };
  Node* l2NO = new Node{ {'N', 'O'} };
  l1->children[0] = l2AB;
  l1->children[1] = l2DEF;
  l1->children[2] = l2JKL;
  l1->children[3] = l2NO;

  Node* r2QRS = new Node{ {'Q', 'R', 'S'} };
  Node* r2UV = new Node{ {'U', 'V'} };
  Node* r2YZ = new Node{ {'Y','Z'} };
  r1->children[0] = r2QRS;
  r1->children[1] = r2UV;
  r1->children[2] = r2YZ;

  BTree tree{root};

  //Albero inizializzato

  tree.remove('F');
  tree.remove('M');
  tree.remove('G');
  tree.remove('D');
  tree.remove('B');

  tree.print();

  tree.remove('C');
  tree.remove('P');
  tree.remove('V');

  tree.print();
  
  return 0;
}