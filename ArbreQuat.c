#include <stdio.h>
#include <stdlib.h>
#include "ArbreQuat.h"


void chaineCoordMinMax(Chaines* C, double* xmin, double* ymin, double* xmax, double* ymax) {
    if (!C) return;

    // Initialiser xmin, ymin, xmax, ymax avec des valeurs extrêmes pour les comparer ensuite
    *xmin = *ymin = __DBL_MAX__;
    *xmax = *ymax = -__DBL_MAX__;

    CellChaine* currentChaine = C->chaines;
    while (currentChaine) {
        CellPoint* currentPoint = currentChaine->points;
        while (currentPoint) {
            if (currentPoint->x < *xmin) *xmin = currentPoint->x;
            if (currentPoint->x > *xmax) *xmax = currentPoint->x;
            if (currentPoint->y < *ymin) *ymin = currentPoint->y;
            if (currentPoint->y > *ymax) *ymax = currentPoint->y;

            currentPoint = currentPoint->suiv;
        }
        currentChaine = currentChaine->suiv;
    }
}

ArbreQuat* creerArbreQuat(double xc, double yc, double coteX, double coteY) {
    ArbreQuat* nouveau = (ArbreQuat*)malloc(sizeof(ArbreQuat));
    if (!nouveau) return NULL; // Vérification si l'allocation a échoué

    // Initialisation des propriétés de la nouvelle cellule
    nouveau->xc = xc;
    nouveau->yc = yc;
    nouveau->coteX = coteX;
    nouveau->coteY = coteY;
    nouveau->noeud = NULL; // Aucun noeud du réseau n'est encore associé à cette cellule
    // Initialisation des sous-arbres à NULL
    nouveau->so = nouveau->se = nouveau->no = nouveau->ne = NULL;

    return nouveau;
}




void insererNoeudArbre(Noeud* n, ArbreQuat** a, ArbreQuat* parent);
Noeud* rechercheCreeNoeudArbre(Reseau* R, ArbreQuat** a, ArbreQuat* parent, double x, double y);
Reseau* reconstitueReseauArbre(Chaines* C);