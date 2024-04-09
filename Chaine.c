#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "SVGwriter.h"
#include "Chaine.h"

Chaines* lectureChaines(FILE *f) {
    if (f == NULL) return NULL;

    Chaines *nouvellesChaine_retour = (Chaines*)malloc(sizeof(Chaines));

    // Attribuer les valeurs de gamma et nbchain a nouvellesChaine_retour
    fscanf(f, "NbChain: %d\n", &(nouvellesChaine_retour->nbChaines));
    fscanf(f, "Gamma: %d\n", &(nouvellesChaine_retour->gamma));

    nouvellesChaine_retour->chaines = NULL;

    // On boucle pour chaque ligne
    for (int i = 0; i < nouvellesChaine_retour->nbChaines; i++) {
        // Allouer la mémoire pour une nouvelle chaine
        CellChaine *nouvelleChaine = (CellChaine*)malloc(sizeof(CellChaine));

        fscanf(f, "%d", &(nouvelleChaine->numero)); // Attribuer l'identifiant de la chaine
        int nbPoints;
        fscanf(f, "%d", &nbPoints); // Attribuer le nombre de points de la chaine a nbPoints

        nouvelleChaine->points = NULL;

        // On boucle sur le nombre de points qu'on a dans la ligne
        for (int j = 0; j < nbPoints; j++) {
            CellPoint *nouveauPoint = (CellPoint*)malloc(sizeof(CellPoint));

            // Lire les coordonnées du point
            fscanf(f, "%lf %lf", &(nouveauPoint->x), &(nouveauPoint->y));
            nouveauPoint->suiv = nouvelleChaine->points; // Insertion en tête
            nouvelleChaine->points = nouveauPoint;
        }

        // Ajouter la nouvelle chaine à la liste des chaines
        nouvelleChaine->suiv = nouvellesChaine_retour->chaines; // Insertion en tête
        nouvellesChaine_retour->chaines = nouvelleChaine;
    }

    return nouvellesChaine_retour;
}


void ecrireChaines(Chaines *C, FILE *f) {
    if (!C || !f) return; // Vérifie que la validité des paramètres

    // En-tete
    fprintf(f, "NbChain: %d\n", C->nbChaines);
    fprintf(f, "Gamma: %d\n", C->gamma);

    CellChaine *currentChaine = C->chaines;
    while (currentChaine) { // Boucle sur les chaine

        // Initialisation du compteur de points pour la chaîne actuelle
        int nbPoints = 0;
        CellPoint *pointCounter = currentChaine->points;
        while (pointCounter) { // Compte les points dans la chaîne
            nbPoints++;
            pointCounter = pointCounter->suiv;
        }

        fprintf(f, "%d %d", currentChaine->numero, nbPoints);
        CellPoint *currentPoint = currentChaine->points;
        while (currentPoint) { // Boucle sur chaque point de chaque chaine
            fprintf(f, " %.10g %.10g", currentPoint->x, currentPoint->y);
            currentPoint = currentPoint->suiv;
        }
        fprintf(f, "\n");
        currentChaine = currentChaine->suiv;
    }
}

void afficheChainesSVG(Chaines *C, char* nomInstance){
    int i;
    double maxx=0,maxy=0,minx=1e6,miny=1e6;
    CellChaine *ccour;
    CellPoint *pcour;
    double precx,precy;
    SVGwriter svg;
    ccour=C->chaines;
    while (ccour!=NULL){
        pcour=ccour->points;
        while (pcour!=NULL){
            if (maxx<pcour->x) maxx=pcour->x;
            if (maxy<pcour->y) maxy=pcour->y;
            if (minx>pcour->x) minx=pcour->x;
            if (miny>pcour->y) miny=pcour->y;  
            pcour=pcour->suiv;
        }
    ccour=ccour->suiv;
    }
    SVGinit(&svg,nomInstance,500,500);
    ccour=C->chaines;
    while (ccour!=NULL){
        pcour=ccour->points;
        SVGlineRandColor(&svg);
        SVGpoint(&svg,500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny)); 
        precx=pcour->x;
        precy=pcour->y;  
        pcour=pcour->suiv;
        while (pcour!=NULL){
            SVGline(&svg,500*(precx-minx)/(maxx-minx),500*(precy-miny)/(maxy-miny),500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny));
            SVGpoint(&svg,500*(pcour->x-minx)/(maxx-minx),500*(pcour->y-miny)/(maxy-miny));
            precx=pcour->x;
            precy=pcour->y;    
            pcour=pcour->suiv;
        }
        ccour=ccour->suiv;
    }
    SVGfinalize(&svg);
}

double longueurChaine(CellChaine *c) {
    // Etant donné qu'il faut calculer la distance entre deux points différents on prend les deux premiers points de la liste de chaines 
    CellPoint* courant = c->points->suiv;
    CellPoint* prec = c->points;
    double longueur=0;

    // On boucle sur l'ensemble des points d'une chaine
    while (courant) {
        // On ajoute la distance entre les deux points à la longueur
        longueur+=sqrt((courant->x - prec->x)*(courant->x - prec->x) + (courant->y-prec->y) * (courant->y-prec->y));
        courant=courant->suiv;
        prec=prec->suiv;
    }

    return longueur;
}

double longueurTotale(Chaines *C) {
    CellChaine* courant=C->chaines;
    double longueurTotale=0;

    // On itère sur toutes les chaînes de la liste 
    while (courant) {
        // On ajoute la longueur de chaque chaîne à la longueur totale
        longueurTotale+=longueurChaine(courant);
        courant=courant->suiv;
    }
    
    return longueurTotale;
}

int comptePointsTotal(Chaines *C) {
    int nbPoints = 0;
    CellChaine* currentChain=C->chaines;
    CellPoint* points;


    while (currentChain) {
        points= currentChain->points;
        while (points) { // Compte les points dans la chaîne
            nbPoints++;
            points = points->suiv;
        }
        currentChain = currentChain->suiv;
    }

    return nbPoints;
}