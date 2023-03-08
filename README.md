# BTree in C++
Una semplice implementazione in C++ di un BTree come descritto da Cormen nel suo libro _Introduzione agli Algoritmi e Strutture Dati_.

L'obiettivo è quello di realizzare una implementazione più semplice possibile dello pseudocodice illustrato nel libro.

Nel repo fornisco il codice in C++ per le procedure presentate nel _capitolo 18_:
* `B-TREE-SEARCH`
* `B-TREE-SPLIT-CHILD`
* `B-TREE-INSERT`
* `B-TREE-INSERT-NONFULL`
* `B-TREE-DELETE`

Oltre queste, ho implementato una banale (e grezza) funzione membro `print()` per poter visualizzare l'albero su livelli.

Per quanto riguarda `B-TREE-DELETE`, Cormen non fornisce lo pseudocodice ma soltato una descrizione (piuttosto accurata).
Sebbene il codice della procedure che ho prodotto possa sembrare lungo e astruso, in realtà è l'esatta mappatura dei punti illustrati dall'autore. Quindi niente paura perché è più semplice di quanto possa apparire.

Il codice è riccamente commentato, nella speranza che possa essere il più immediatamente comprensibile.
In ogni caso è necessario, ovviamente, leggere a fondo ciascun paragrafo e, sopratutto, lo pseudocodice, prima di passare all'implementazione di una procedure repo.

Nel `main()` ho costruito un albero uguale a quello presente in _figura 18.8_ del libro. In seguito, a titolo di esempio, ho eseguito tutti i passi della figura, compreso l'_esercizio 18.3-1_.

Per avere una comparazione in modo tale da verificare la correttezza del codice, ho utilizzato il noto [visualizzatore](https://ysangkok.github.io/js-clrs-btree/btree.html) di [ysangkok](https://github.com/ysangkok/js-clrs-btree). Nel file `test.txt` ho messo il JSON per costruire all'interno del sito l'albero della _figura 18.8_ (il primo form in alto a destra impostando il parametro `M = 3`).

Se volete "giocare" con il BTree, dovrete fare attenzione alla costante simbolica `CORMEN_ORDER` definita nel file `btree.h`, che determina l'ordine dell'albero come nella definizione presentata da Cormen. Pertanto dovrete utilizzare un valore maggiore o uguale di 2.

Per quanto riguarda le chiavi che possono essere inserite nell'albero, allo stato attuale è possibile inserire solo caratteri. Ho fatto questa scelta in modo tale da poter svolgere gli esempi del libro.

Tutto il lavoro svolto fino ad ora è un _work in progress_. In particolare occorre, come minimo:
- [ ] Eliminare la dipendenza dalla costante simbolica `CORMEN_ORDER`
- [ ] _Templatizzare_ la classe BTree in modo da poter contenere elementi definiti dall'utente (e non soltanto di tipo `char`)