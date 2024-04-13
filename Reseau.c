#include <stdlib.h>
#include <stdio.h>

#include "Reseau.h"

Noeud* rechercheCreeNoeudListe(Reseau *R, double x, double y) {
    CellNoeud* listeNoeuds = R->noeuds;

    while (listeNoeuds) {
        if (listeNoeuds->nd->x == x && listeNoeuds->nd->y == y) {
            return listeNoeuds->nd;
        }
        listeNoeuds = listeNoeuds->suiv;
    }

    CellNoeud *cn = (CellNoeud *)malloc(sizeof(CellNoeud));
    Noeud* nouveauNoeud = (Noeud *)malloc(sizeof(Noeud));
    nouveauNoeud->num= R->nbNoeuds +1;
    nouveauNoeud->x = x;
    nouveauNoeud->y = y;
    cn->nd = nouveauNoeud;
    cn->suiv=NULL;
    return nouveauNoeud;
}

Reseau *reconstitueReseauListe (Chaines *C){
    Reseau *res = (Reseau*)malloc(sizeof(Reseau));
    res->noeuds =NULL;
    CellChaine *tmpC = C->chaines;
    while(tmpC){
        CellPoint *point = tmpC->points;
        while(point){
            rechercheCreeNoeudListe(res, point->x, point->y);
            point = point->suiv;
        }
        tmpC = tmpC->suiv;
    }
}

int nbCommodites(Reseau *R) {
    int nbCommod = 0;

    CellCommodite *commoditeCourante = R->commodites;

    while (commoditeCourante) {
        nbCommod++;
        commoditeCourante=commoditeCourante->suiv;
    }

    return nbCommod;
}