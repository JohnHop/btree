#if !defined(BTREE)
#define BTREE

#include <cstddef>
#include <utility>
#include <iostream>
#include <vector>

#define CORMEN_ORDER 3 //minimo t-1 chiavi e massimo 2t-1 chiavi per nodo

struct Node {
  int key_count; //numero di chiavi, posizione della prossima chiave libera e indice dell'ultimo figlio del nodo
  char keys[CORMEN_ORDER*2-1]; //vettore di chiavi
  Node* children[CORMEN_ORDER*2]; //vettore dei figli
  bool leaf;

  Node(): key_count{0}, children{nullptr}, leaf{true} { };
  Node(const char); //Inizializzazione tramite una solachiave
  Node(std::initializer_list<char>);  //Inizializzazione tramite lista di chiavi

  bool is_full() const;

  Node(const Node&) = delete;
  Node(Node&&) = delete;
};

class BTree {
  Node* root;
  
  std::pair<Node*,int> find(const char key, Node* where) const; //Algoritmo di ricerca utilizzato internamente
  void split_child(Node* parent, const int child_pos);  //Separa il figlio @i-esimo in due nodi aventi come genitore @parent
  void insert_nonfull(Node*, const char);

  void print(std::vector<Node*>&);

  int min_keys() { return CORMEN_ORDER-1; };
  char precedessor(const char e, Node* current);
  Node* merge(Node*, const int);
  void delete3a(Node*, const int);

  void destructor_helper(Node*);  //funzione helper del distruttore che aiuta a deallocale l'albero

public:
  BTree(Node* r = nullptr): root{r} { };
  ~BTree();

  std::pair<Node*,int> find(const char key) const { return find(key, root); }; //Restituisce il nodo e l'indice della chiave trovata
  BTree& insert(const char);
  bool remove(const char, Node* current = nullptr);

  void print();
};

inline std::ostream& operator<<(std::ostream& os, const Node& n) {
  os << "[";

  for(int i = 0; i < n.key_count-1; ++i) {
    os << n.keys[i] << ", ";
  }
  return os << n.keys[n.key_count-1] << "]";
}

#endif // BTREE
