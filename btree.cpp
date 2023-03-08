#include "btree.h"

#include <utility>
#include <vector>
#include <stack>

Node::Node(const char k)
: key_count{1}, children{nullptr}, leaf{true} 
{
  keys[0] = k;
};

Node::Node(std::initializer_list<char> list)
: children{nullptr}, leaf{true}, key_count{0}
{
  for(auto e: list) {
    keys[key_count++] = e;
  }
}

bool Node::is_full() const { 
  return (2*CORMEN_ORDER-1) == key_count; 
}; //FIXME eliminiamole queste dipendenze da costanti simboliche!

/**
 * Restituisce la coppia Nodo* e indice della posizione di @key nel nodo
*/
std::pair<Node*,int> BTree::find(const char key, Node* current) const {
  if(current == nullptr) {
    return std::pair{nullptr, 0}; //se la radice è vuota
  }

  for(int i = 0; i < current->key_count; ++i) {
    if(key == current->keys[i]) {
      return std::pair{current, i};
    }
    else if(key < current->keys[i]) {
      return find(key, current->children[i]);
    }
  }

  //key non trovata in @current
  if(current->leaf) {
    return std::pair{nullptr, 0}; //se è foglia
  }
  else {  //altrimenti cerco a destra
    return find(key, current->children[current->key_count]);
  }
}

/**
 * Separa il nodo @parent->children[child_pos] facendo "salire" la mediana
*/
void BTree::split_child(Node* parent, const int child_pos) {
  Node* z = new Node{}; //nuovo nodo
  Node* y = parent->children[child_pos];  //nodo figlio di @x da separare
  z->leaf = y->leaf;  //visto che @z potrebbe ereditare alcuni figli di @y
  
  int mediana_y = (y->key_count) / 2;  //posizione dell'elemento che "salirà"
  z->key_count = y->key_count - (mediana_y + 1);  //devo determinare il numero di chiavi da mettere in z

  //Sposto le chiavi da y --> z
  for(int i = 0; i < z->key_count; ++i) {
    z->keys[i] = y->keys[mediana_y+1+i];
  }
  //quindi sposto anche (eventualmente) i figli
  for(int i = 0; i <= z->key_count && !(y->leaf); ++i) {
    z->children[i] = y->children[mediana_y+1+i];
  }

  y->key_count = mediana_y; //la mediana è anche il numero degli elementi del nodo che la precedono

  for(int i = parent->key_count; i > child_pos; --i) { //shifto a destra i puntatori di @parent dopo @child_pos
    parent->children[i+1] = parent->children[i];
  }

  parent->children[child_pos+1] = z;

  for(int i = parent->key_count-1; i >= child_pos; --i) { //shifto le chiavi di @parent a destra per far spazio alla nuova chiave che salirà
    parent->keys[i+1] = parent->keys[i];
  }

  parent->keys[child_pos] = y->keys[mediana_y]; //faccio "salire" la nuova chiave
  parent->key_count += 1;
}

/**
 * Inserisce una @key nell'albero
*/
BTree& BTree::insert(const char key) {
  if(root == nullptr) {
    root = new Node;  //crea un nodo foglia vuoto
  }

  //Inizio algoritmo di Comer
  Node* r = root;

  if(r->is_full()) {
    Node* s = new Node{};
    root = s;
    s->leaf = false;
    s->key_count = 0;
    s->children[0] = r;
    split_child(s, 0);  //divide 'r', ovvero la vecchia radice
    insert_nonfull(s, key); 
  }
  else
    insert_nonfull(r, key);
  
  return *this;
}

/**
 * Inserisce @key nel nodo @node che è garantito essere non-full
*/
void BTree::insert_nonfull(Node* node, const char key) {
  int i = node->key_count;  //ottengo il numero di chiavi

  if(node->leaf) {
    while(i > 0 && key < node->keys[i-1]) { //Se sono presenti chiavi && finché @key è più piccola della chiave (i-1)-esima
      node->keys[i] = node->keys[i-1];  //shifto a destra le chiavi in modo da inserirle in modo ordinato
      i -= 1;
    }
    node->keys[i] = key;
    node->key_count += 1;
  }
  else {  //ovvero nodo non-foglia. Quindi la @key va inserita in un figlio
    while(i > 0 && key < node->keys[i-1]) { //determino in quale figlio inserire la chiave
      i -= 1; 
    }
    if(node->children[i]->is_full()) {
      split_child(node, i);
      if(key > node->keys[i]) {
        i += 1;
      }
    }
    insert_nonfull(node->children[i], key);
  }
}

/**
 * 
*/
bool BTree::remove(const char key, Node* current) {
  if(current == nullptr) current = root;

  //Inizio algoritmo di Cormer
  int i = 0;
  while( (i < current->key_count) && (key > current->keys[i]) ) { //Cerchiamo la key da eliminare OPPURE il figlio in cui eventualmente annidarci
    i += 1;
  }

  if( (i < current->key_count) && (key == current->keys[i]) ) { //Ho trovato la chiave da eliminare nel nodo corrente
    if(current->leaf) { //Caso 1: il nodo è una foglia
      for(int j = i; j < current->key_count-1; ++j) { //Quindi traslo tutte le chiavi più a sinistra in modo da eliminare quella attuale nel caso in cui non fosse l'ultima nel nodo
        current->keys[j] = current->keys[j+1];
      }
      current->key_count -= 1;
      return true;
    }
    else {  //Caso 2: il nodo non è foglia
      Node* y = current->children[i]; //ottengo il figlio della chiave trovata
      if(y->key_count > min_keys()) { //Caso 2a: y ha almeno t chiavi (cioè il minimo + 1)
        char predecessor_key = precedessor(key, y);  //ottengo la posizione del precedessore. Procedo come descritto in Cormer
        remove(predecessor_key, y);
        current->keys[i] = predecessor_key;
        return true;
      }
      else {  //y ha meno di t chiavi. Tento il Caso 2b //DOTEST
        Node* z = current->children[i+1]; //ottengo il figlio successore di key
        if(z->key_count > min_keys()) { //è proprio il caso 2b!
          char predecessor_key = precedessor(key, z);  //ottengo la posizione del precedessore. Procedo come descritto in Cormer
          remove(predecessor_key, z); //FIXME spiega questo e metti qui il return
          current->keys[i] = predecessor_key;
          return true;
        }
        else {  //Allora è il caso 2c, ovvero sia y che z hanno almeno t-1 chiavi. Li unisco come descritto da Cormer //? forse dovresti specificare la condizione
          merge(current, i);
          return remove(key, y);
        }
      }
    }
  }
  else if(!current->leaf) {  //Caso 3: @key non è presente in @current e quest'ultimo è un nodo interno
    if(current->children[i]->key_count == min_keys()) { //Se il nodo individuato ha t-1 chiavi, si passa ai casi 3a / 3b.
      //caso 3a: se un vicino di @current->children[#i] ha almeno t chiavi (minimo + 1)
      if( (i > 0) && current->children[i-1]->key_count > min_keys() ||
          (i < current->key_count) && current->children[i+1]->key_count > min_keys()
      ) {
        delete3a(current, i); //il figlio alla posizione #i ottiene una chiave da un vicino
        return remove(key, current->children[i]); //proseguo la ricerca
      } 
      else { //entrambi i vicini di @current->children[#i] hanno il minimo numero di chiavi (caso 3b)
        Node* y = merge(current, (i == current->key_count) ? i-1 : i);  //unisco i vicini
        if(current->key_count == 0) { //evidentemente era la radice
          root = y;
        }
        return remove(key, y);  //Continuo la ricerca di @key nel nuovo nodo creato
      }
    }
    return remove(key, current->children[i]);  //Continuo la ricerca di @key da eliminare
  }

  return true;
}

/**
 * Esegue l'unione dei nodi y e z (foglia!) che sono rispettivamente il figlio che precede e segue la chiave nella posizione @key_pos
 * e inserisce anche @key_pos in y
 *    - perdendo z
 *    - perdendo @current->keys[@key_pos]
 * Restituisce il nuovo nodo creato
*/
Node* BTree::merge(Node* current, const int key_pos) {
  Node* y = current->children[key_pos]; //figlio precedessore
  Node* z = current->children[key_pos+1]; //figlio che segue

  y->keys[y->key_count++] = current->keys[key_pos]; //inserisco @current->keys[@key_pos] in y

  for(int i = 0; i < z->key_count; ++i) { //aggiungo gli elementi di z in y //CHECK forse dovresti eliminarli anche da z
    y->keys[y->key_count] = z->keys[i];
    y->children[y->key_count] = z->children[i];
    y->key_count += 1;
  }
  y->children[y->key_count] = z->children[z->key_count];  //mi prendo, infine, l'ultimo children più a destra di z

  //Elimino la chiave alla posizione @key_pos in @current shiftando le altre chiavi e figli
  for(int i = key_pos; i < current->key_count-1; ++i) {
    current->keys[i] = current->keys[i+1];
    current->children[i+1] = current->children[i+2];  //CHECK
  }
  current->key_count -= 1;

  delete z; //Oramai z non serve più

  return y;
}

/**
 * Esegue il punto 3a dati gli elementi
 *  @parent: è il nodo dove salirà una chiave
 *  @pos: è la posizione del children che contiene la chiave
*/
void BTree::delete3a(Node* parent, const int children_pos) {
  Node* y = parent->children[children_pos];  //è il figlio dove proseguirà la riecerca della chiave da eliminare
  Node* s;  //è il sibling che possiede almeno t chiavi
  int parent_pos;  //la posizione della chiave nel @parent che scenderà in #y
  int s_pos;  //la posizione della chiave in #sibling che "salirà" nel @parent
  int y_pos;  //la posizione in #y dove scenderà la chiave dal parent

  // 1. Capisco il sibling che contiene almeno almeno t chiavi. Uno dei due per forza
  if( (children_pos > 0) && (parent->children[children_pos-1]->key_count > min_keys()) ) {  //se è quello a sinistra
    s = parent->children[children_pos-1];
    parent_pos = children_pos - 1;
    s_pos = s->key_count - 1;
    y_pos = 0;
  }
  else {  //ovvero (children_pos < parent->key_count) && (parent->children[children_pos+1]->key_count > min_keys() )
    s = parent->children[children_pos+1];
    parent_pos = children_pos;
    s_pos = 0;
    y_pos = y->key_count;
  } //questo è quello a destra
  
  // 2. Quindi eseguo prima l'op1: faccio scendere la chiave del parent alla posizione #parent_pos in #y
  if(y_pos == 0) {  //shifto chiavi e children a destra per far spazio alla nuova chiave (e al nuovo figlio)
    for(int i = y->key_count-1; i >= 0; i -= 1) {
      y->keys[i+1] = y->keys[i];
      y->children[i+1] = y->children[i+1];
    }
    y->children[1] = y->children[0];  //sposto anche l'ultimo children non interessato dal ciclo
  }
  y->keys[y_pos] = parent->keys[parent_pos];
  y->key_count += 1;

  //eseguo l'op2: ovvero sposto s->keys[s_pos] in parent->keys[parent_pos] e il suo figlio a destra nella corretta posizione di #y
  parent->keys[parent_pos] = s->keys[s_pos];
  (y_pos == 0) ? y->children[y_pos] = s->children[s_pos+1] : y->children[y_pos+1] = s->children[s_pos];

  if(s_pos == 0) {  //in tal caso devo shiftare a sinistra le chiavi e i figli
    for(int i = 0; i < s->key_count; ++i) {
      s->keys[i] = s->keys[i+1];
      s->children[i] = s->children[i+1];
    }
    s->children[s->key_count-1] = s->children[s->key_count];  //sposto anche l'ultimo children
  }
  s->key_count -= 1;  
}

/**
 * Cerca nel nodo @current il precedessore di @key.
 * Si suppone che il nodo @current non possegga @key
*/
char BTree::precedessor(const char key, Node* current) {
  int i = 0;  //posizione del precedessore

  while(i < current->key_count && key > current->keys[i]) {
    i += 1;
  }

  return (i > 0) ? current->keys[i-1] : current->keys[0];
}

//Esegue la stampa di tutti i nodi dell'albero raggruppati per livello
void BTree::print(void) {
  if(root == nullptr) {
    std::cout << "(empty)\n";
  }
  else {
    std::vector<Node*> figli{root};
    print(figli); //stampo il resto dell'albero
  }
}

/**
 * Stampa un vettore di nodi e mette i loro figli in un altro vettore per la stampa su livelli
*/
void BTree::print(std::vector<Node*>& nodi) {
  std::vector<Node*> figli;  //conterrà tutti i figli dei nodi del vettore @nodi

  for(auto n: nodi) {  //Per ogni nodo del livello corrente
    for(int i = 0; (!n->leaf) && i < n->key_count+1; ++i) { //mette i figli in un vettore (se non è foglia)
      figli.push_back(n->children[i]);
    }
    std::cout << *n;  //stampo i figli del livello
  }
  std::cout << "\n";  //Dopo aver stampato un livello vado a capo

  if(figli.size() > 0)
    print(figli);  //Stampo i figli del livello successivo
}

/**
 * Distruzione ricorsiva dell'albero utilizzando la funzione destructor_helper().
*/
BTree::~BTree() {
  if(root)
    destructor_helper(root);
}

void BTree::destructor_helper(Node* current) {
  if(current->leaf) {
    delete current;
  }
  else {
    for(int i = 0; i < root->key_count+1; ++i) {
      destructor_helper(root->children[i]);
    }
  }
}