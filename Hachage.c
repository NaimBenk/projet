#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "Hachage.h"

int FHash(int k, int M) {
    double A = (sqrt(5) - 1) / 2;
    double B = k * A - (int)(k * A);
    return (int)(M * B);
}

Noeud* rechercheCreeNoeudHachage(Reseau* R, TableHachage* H, double x, double y) {
    // Calcul de l'indice de hachage à partir des coordonnées x, y
    int index = FHash((int)(x + y), H->tailleMax);

    // Recherche du nœud dans la table de hachage
    CellNoeud* liste = H->T[index];
    while (liste != NULL) {
        if (liste->nd->x == x && liste->nd->y == y) {
            // Nœud trouvé
            return liste->nd;
        }
        liste = liste->suiv;
    }

    // Nœud non trouvé, création d'un nouveau
    Noeud* nouveau = (Noeud*)malloc(sizeof(Noeud));
    if (!nouveau) return NULL; // Gestion d'erreur en cas d'allocation mémoire échouée
    nouveau->x = x;
    nouveau->y = y;
    nouveau->num = ++R->nbNoeuds; // Incrémentation du nombre de nœuds dans le réseau
    nouveau->voisins = NULL; // Initialisation de la liste des voisins

    // Ajout du nouveau nœud à la table de hachage
    CellNoeud* nouvelleCellule = (CellNoeud*)malloc(sizeof(CellNoeud));
    if (!nouvelleCellule) { free(nouveau); return NULL; } // Gestion d'erreur
    nouvelleCellule->nd = nouveau;
    nouvelleCellule->suiv = H->T[index];
    H->T[index] = nouvelleCellule;

    // Ajout également du nouveau nœud à la liste des nœuds du réseau
    nouvelleCellule = (CellNoeud*)malloc(sizeof(CellNoeud));
    if (!nouvelleCellule) { // Gestion d'erreur
        free(nouveau);
        // Ici, on devrait aussi retirer le nœud de la table de hachage si nécessaire
        return NULL;
    }
    nouvelleCellule->nd = nouveau;
    nouvelleCellule->suiv = R->noeuds;
    R->noeuds = nouvelleCellule;

    return nouveau;
}


Reseau* reconstitueReseauHachage(Chaines *C, int M) {
    // Allocation de la structure du réseau
    Reseau* R = (Reseau*)malloc(sizeof(Reseau));
    if (!R) return NULL; // En cas d'échec d'allocation
    
    R->nbNoeuds = 0;
    R->noeuds = NULL;
    R->commodites = NULL;
    R->gamma = C->gamma; // Copie de gamma de C à R

    // Initialisation de la table de hachage
    TableHachage* H = (TableHachage*)malloc(sizeof(TableHachage));
    if (!H) { free(R); return NULL; } // Libération de R en cas d'échec d'allocation de H
    
    H->tailleMax = M;
    H->T = (CellNoeud**)malloc(M * sizeof(CellNoeud*));
    if (!H->T) { free(H); free(R); return NULL; } // Libération en cas d'échec d'allocation
    for (int i = 0; i < M; i++) {
        H->T[i] = NULL;
    }

    // Parcours des chaînes pour créer ou trouver les nœuds
    CellChaine* cellChaineActuelle = C->chaines;
    while (cellChaineActuelle != NULL) {
        CellPoint* pointActuel = cellChaineActuelle->points;
        while (pointActuel != NULL) {
            // Création ou récupération du nœud correspondant au point actuel
            rechercheCreeNoeudHachage(R, H, pointActuel->x, pointActuel->y);
            pointActuel = pointActuel->suiv;
        }
        cellChaineActuelle = cellChaineActuelle->suiv;
    }

    // À ce stade, tous les nœuds ont été ajoutés au réseau R et à la table de hachage H

    // Libération de la table de hachage (les nœuds restent dans R)
    for (int i = 0; i < M; i++) {
        // Libération des listes chainées dans chaque entrée de H
        CellNoeud* current = H->T[i];
        while (current != NULL) {
            CellNoeud* tmp = current;
            current = current->suiv;
            free(tmp); // On libère la cellule, mais pas le nœud (qui est dans R)
        }
    }
    free(H->T); // Libération du tableau de pointeurs
    free(H); // Libération de la structure de la table de hachage

    return R; // Retour du réseau reconstruit
}