#ifndef __HACHAGE_H__
#define __HACHAGE_H__
#include "Reseau.h"

typedef struct CelluleHachage {
    Noeud *noeud;                   // Pointeur vers le noeud
    struct CelluleHachage *suiv;    // Pointeur vers la cellule suivante dans la liste chaînée
} CelluleHachage;

typedef struct{
  int tailleMax;
  CellNoeud** T;
} TableHachage ;

Noeud* rechercheCreeNoeudHachage(Reseau* R, TableHachage* H, double x, double y);
Reseau* reconstitueReseauHachage(Chaines *C, int M);


#endif