#include <stdlib.h>
#include <stdio.h>

#include "Reseau.h"
#include "SVGwriter.h"

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
    CellCommodite *commodites = NULL;
    CellCommodite *precCom = NULL;

    
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

void ecrireReseau(Reseau *R, FILE *f) {
    if (!R || !f) return; // Vérifie que le réseau et le fichier sont valides

    // Écriture des informations générales du réseau
    fprintf(f, "NbNoeuds: %d\n", R->nbNoeuds);
    fprintf(f, "NbLiaisons: %d\n", nbLiaisons(R)); // Utilise la fonction `nbLiaisons` pour compter les liaisons
    fprintf(f, "NbCommodites: %d\n", nbCommodites(R)); // Utilise la fonction `nbCommodites` pour compter les commodités
    fprintf(f, "Gamma: %d\n", R->gamma);

    // Écriture des nœuds du réseau
    CellNoeud *cn = R->noeuds;
    while (cn != NULL) {
        fprintf(f, "v %d %f %f\n", cn->nd->num, cn->nd->x, cn->nd->y);
        cn = cn->suiv;
    }

    // Écriture des liaisons
    cn = R->noeuds;
    while (cn != NULL) {
        CellNoeud *voisin = cn->nd->voisins;
        while (voisin != NULL) {
            if (cn->nd->num < voisin->nd->num) { // Évite les doublons en écrivant chaque liaison une seule fois
                fprintf(f, "l %d %d\n", cn->nd->num, voisin->nd->num);
            }
            voisin = voisin->suiv;
        }
        cn = cn->suiv;
    }

    // Écriture des commodités
    CellCommodite *commod = R->commodites;
    while (commod != NULL) {
        fprintf(f, "k %d %d\n", commod->extrA->num, commod->extrB->num);
        commod = commod->suiv;
    }
}


void afficheReseauSVG(Reseau *R, char* nomInstance){
    if (!R || !R->noeuds) {
        printf("Réseau vide ou non initialisé. Aucun affichage SVG possible.\n");
        return;
    }
    CellNoeud *courN,*courv;
    SVGwriter svg;
    double maxx=0,maxy=0,minx=1e6,miny=1e6;

    courN=R->noeuds;
    while (courN!=NULL){
        if (maxx<courN->nd->x) maxx=courN->nd->x;
        if (maxy<courN->nd->y) maxy=courN->nd->y;
        if (minx>courN->nd->x) minx=courN->nd->x;
        if (miny>courN->nd->y) miny=courN->nd->y;
        courN=courN->suiv;
    }
    SVGinit(&svg,nomInstance,500,500);
    courN=R->noeuds;
    while (courN!=NULL){
        SVGpoint(&svg,500*(courN->nd->x-minx)/(maxx-minx),500*(courN->nd->y-miny)/(maxy-miny));
        courv=courN->nd->voisins;
        while (courv!=NULL){
            if (courv->nd->num<courN->nd->num)
                SVGline(&svg,500*(courv->nd->x-minx)/(maxx-minx),500*(courv->nd->y-miny)/(maxy-miny),500*(courN->nd->x-minx)/(maxx-minx),500*(courN->nd->y-miny)/(maxy-miny));
            courv=courv->suiv;
        }
        courN=courN->suiv;
    }
    SVGfinalize(&svg);
}


void libererChaines(Chaines *C) {
    if (!C) return;

    CellChaine *chaineCourante = C->chaines;
    while (chaineCourante != NULL) {
        CellChaine *tempChaine = chaineCourante;
        chaineCourante = chaineCourante->suiv;

        // Libérer tous les points dans la chaine courante
        CellPoint *pointCourant = tempChaine->points;
        while (pointCourant != NULL) {
            CellPoint *tempPoint = pointCourant;
            pointCourant = pointCourant->suiv;
            free(tempPoint);
        }

        free(tempChaine);
    }

    free(C);
}

void libererReseau(Reseau *R) {
    if (!R) return;

    // Libérer tous les noeuds dans le réseau
    CellNoeud *noeudCourant = R->noeuds;
    while (noeudCourant != NULL) {
        CellNoeud *tempNoeud = noeudCourant;
        noeudCourant = noeudCourant->suiv;

        // Libérer la liste des voisins pour chaque noeud
        CellNoeud *voisinCourant = tempNoeud->nd->voisins;
        while (voisinCourant != NULL) {
            CellNoeud *tempVoisin = voisinCourant;
            voisinCourant = voisinCourant->suiv;
            free(tempVoisin);
        }

        free(tempNoeud->nd);  // Libérer le noeud lui-même
        free(tempNoeud);      // Libérer la cellule noeud
    }

    // Libérer toutes les commodités dans le réseau
    CellCommodite *commoditeCourante = R->commodites;
    while (commoditeCourante != NULL) {
        CellCommodite *tempCommodite = commoditeCourante;
        commoditeCourante = commoditeCourante->suiv;
        free(tempCommodite);
    }

    free(R);
}