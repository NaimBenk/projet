#include <stdlib.h>
#include <stdio.h>

#include "Reseau.h"

/**
 * L'objectif de cette fonctione est de chercher un noeud dans le réseau, cependant si le noeud n'existe pas alors il va être crée
 */ 
Noeud* rechercheCreeNoeudListe(Reseau *R, double x, double y) {
    CellNoeud* listeNoeuds = R->noeuds;

    while (listeNoeuds) {
        // Cas où le noeud est trouvé, on sort de la fonction
        if (listeNoeuds->nd->x == x && listeNoeuds->nd->y == y) {
            return listeNoeuds->nd;
        }
        listeNoeuds = listeNoeuds->suiv;
    }

    // Le noeud n'a pas été trouvé, on crée le noeud
    CellNoeud *cn = (CellNoeud *)malloc(sizeof(CellNoeud));
    Noeud* nouveauNoeud = (Noeud *)malloc(sizeof(Noeud));
    // on augmente le nombre de noeuds du réseau 
    R->nbNoeuds++;
    nouveauNoeud->num= R->nbNoeuds;
    nouveauNoeud->x = x;
    nouveauNoeud->y = y;
    cn->nd=nouveauNoeud;
    cn->suiv=R->noeuds;
    // Ajout en tête dans la liste des noeuds du réseau
    R->noeuds=cn;
    return nouveauNoeud;
}

void insertionEnTete(CellNoeud *cn, Noeud *n) {
    CellNoeud* nouv=(CellNoeud *)malloc(sizeof(CellNoeud));

    nouv->nd=n;
    nouv->suiv=cn;
    cn=nouv;
}

Reseau *reconstitueReseauListe (Chaines *C){
    Noeud* precNoeud;
    Noeud* noeud;
    CellCommodite* commodites;
    CellCommodite* precCom;
    
    // création du réseau vide
    Reseau *res = (Reseau*)malloc(sizeof(Reseau));
    res->gamma=C->gamma;
    res->nbNoeuds=0;
    res->noeuds=NULL;
    // crération de la tête de liste des commodité
    res->commodites=commodites;
    
    CellChaine *chaine_courante = C->chaines;

    while(chaine_courante){
        CellPoint *point = chaine_courante->points;
        precNoeud = rechercheCreeNoeudListe(res, point->x, point->y);
        point=point->suiv;

        commodites = (CellCommodite*)malloc(sizeof(CellCommodite));
        if (precCom) 
            precCom->suiv=commodites;
        precCom=commodites;
        
        // Premier point de la commodité
        commodites->extrA=precNoeud;

        while(point){
            // La fonction teste déjà si le point est déjà présent dans le réseau
            noeud=rechercheCreeNoeudListe(res, point->x, point->y);
            // Mise à jour de la liste des voisins
            // Mise à jour de la liste des voisins du noeud en y ajoutant le noeud précédent
            insertionEnTete(noeud->voisins, precNoeud);
            // Mise à jour de la liste des voisins du précédent en y ajoutant le noeud courant
            insertionEnTete(precNoeud->voisins, noeud);

            precNoeud= noeud;
            point = point->suiv;
        }

        // Dernier point de la commodité
        commodites->extrB=noeud;

        chaine_courante = chaine_courante->suiv;
    }

    return res;
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

int nbLiaisons(Reseau *R) {
    CellNoeud* c= R->noeuds;
    
    // tableau des noeuds déjà visités pour éviter de compter des liasons en double
    int dejaVisites[R->nbNoeuds];
    // remplissage avec des 0 pour dire qu'aucun noeaud a été visité
    for (int i=0; i<R->nbNoeuds; i++) {
        dejaVisites[i]=0;
    }

    int nbLiaison=0;
    CellNoeud* voisin;

    while (c) {
        // marquage comme déjà visité
        dejaVisites[c->nd->num]=1;
        voisin=c->nd->voisins;

        // parcours de tous les voisins pour compter les liaisons
        while(voisin) {
            // Si le voisin n'a pas été déjà visité alors on incrémente le nombre de liaisons
            if (!dejaVisites[voisin->nd->num]) {
                nbLiaison++;
            }
            voisin=voisin->suiv;
        }

        c=c->suiv;
    }

    return nbLiaison;
}