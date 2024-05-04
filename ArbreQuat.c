#include <stdio.h>
#include <stdlib.h>
#include "ArbreQuat.h"


void chaineCoordMinMax(Chaines* C, double* xmin, double* ymin, double* xmax, double* ymax) {
    if (!C) return;

    // Initialiser xmin, ymin, xmax, ymax avec des valeurs extrêmes pour les comparer ensuite
    *xmin = *ymin = __DBL_MAX__; // evite de faire une iteration hors boucle pour initialiser aux premieres valeurs avant comparaison
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

void insererNoeudArbre(Noeud* n, ArbreQuat** a, ArbreQuat* parent) {
    if (*a == NULL) { // Arbre vide
        double xc, yc, coteX, coteY;
        if (parent == NULL) {
            // Si aucun parent on choisis les dimensions arbitrairement
            xc = n->x; yc = n->y;
            coteX = coteY = 1.0; // valeurs par défaut
        } else {
            // Calcul des dimensions basée sur le pere
            coteX = parent->coteX / 2;
            coteY = parent->coteY / 2;
            xc = n->x < parent->xc ? parent->xc - coteX / 2 : parent->xc + coteX / 2;
            yc = n->y < parent->yc ? parent->yc - coteY / 2 : parent->yc + coteY / 2;
        }
        *a = creerArbreQuat(xc, yc, coteX, coteY);
        (*a)->noeud = n;


    } else if ((*a)->noeud != NULL) { // Feuille
        // Si le point à insérer est identique on ne fait rien
        if ((*a)->noeud->x == n->x && (*a)->noeud->y == n->y) {
            return;
        }
        // Sinon, scindez la feuille
        Noeud* ancienNoeud = (*a)->noeud;
        (*a)->noeud = NULL; // Efface le noeud car nous allons diviser cette cellule
        insererNoeudArbre(ancienNoeud, a, *a);
        insererNoeudArbre(n, a, *a);


    } else { // Cellule interne
        // On determine dans quelle sous-cellule insérer le noeud
        ArbreQuat** subTree;
        if (n->x < (*a)->xc && n->y < (*a)->yc)
            subTree = &(*a)->no;
        else if (n->x >= (*a)->xc && n->y < (*a)->yc)
            subTree = &(*a)->ne;
        else if (n->x < (*a)->xc && n->y >= (*a)->yc)
            subTree = &(*a)->so;
        else
            subTree = &(*a)->se;

        insererNoeudArbre(n, subTree, *a);
    }
}

Noeud* rechercheCreeNoeudArbre(Reseau* R, ArbreQuat** a, ArbreQuat* parent, double x, double y) {
    if (*a == NULL) {
        // Si l'arbre est vide, on creer un nouveau noeud et initialisez l'arbre
        Noeud* n = (Noeud*)malloc(sizeof(Noeud));
        if (!n) return NULL; // Vérification de l'échec de l'allocation
        n->x = x;
        n->y = y;
        n->num = ++R->nbNoeuds;
        n->voisins = NULL;

        // Créer un nouvel arbre quaternaire pour ce noeud
        *a = creerArbreQuat(x, y, parent ? parent->coteX / 2 : 0.5, parent ? parent->coteY / 2 : 0.5); // Les valeurs 0.5 sont choisis arbitrairement
        if (!*a) {
            free(n);
            return NULL;
        }
        (*a)->noeud = n;

        // Ajout du nouveau noeud à la liste des noeuds du réseau
        CellNoeud* nouvelleCell = (CellNoeud*)malloc(sizeof(CellNoeud));
        if (!nouvelleCell) {
            free(n); // Libération de la mémoire si la cellule ne peut être allouée
            return NULL;
        }
        nouvelleCell->nd = n;
        nouvelleCell->suiv = R->noeuds;
        R->noeuds = nouvelleCell;
        
        return n;
    } else if ((*a)->noeud) {
        // Si un noeud existe déjà à cette position, et que c'est le même on le retourne
        if ((*a)->noeud->x == x && (*a)->noeud->y == y) {
            return (*a)->noeud;
        } else {
            // Si conflit, on divise la cellule en dessandant d'un etage
            Noeud* ancienNoeud = (*a)->noeud;
            (*a)->noeud = NULL; // L'arbre ne contient plus directement un noeud
            insererNoeudArbre(ancienNoeud, a, *a);
            return rechercheCreeNoeudArbre(R, a, *a, x, y);
        }
    } else {
        // Sinon, on détermine dans quelle sous-arbre on vas descendre
        ArbreQuat** subTree = NULL;
        if (x < (*a)->xc && y < (*a)->yc) subTree = &(*a)->no;
        else if (x >= (*a)->xc && y < (*a)->yc) subTree = &(*a)->ne;
        else if (x < (*a)->xc && y >= (*a)->yc) subTree = &(*a)->so;
        else subTree = &(*a)->se;

        return rechercheCreeNoeudArbre(R, subTree, *a, x, y);
    }
}

Reseau* reconstitueReseauArbre(Chaines* C) {
    if (!C) return NULL;

    Reseau* R = (Reseau*)malloc(sizeof(Reseau));
    if (!R) return NULL;
    R->noeuds = NULL;
    R->nbNoeuds = 0;
    R->commodites = NULL;

    double xmin, ymin, xmax, ymax;
    chaineCoordMinMax(C, &xmin, &ymin, &xmax, &ymax);
    ArbreQuat* racine = creerArbreQuat((xmin + xmax) / 2, (ymin + ymax) / 2, xmax - xmin, ymax - ymin);
    if (!racine) {
        free(R);
        return NULL;
    }

    CellChaine* courante = C->chaines;
    while (courante) {
        CellPoint* point = courante->points;
        while (point) {
            if (!rechercheCreeNoeudArbre(R, &racine, racine, point->x, point->y)) {
                libererReseau(R); 
                return NULL;
            }
            point = point->suiv;
        }
        courante = courante->suiv;
    }

    return R;
}