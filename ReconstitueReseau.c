#include <stdio.h>
#include <stdlib.h>
#include "Chaine.h"
#include "Reseau.h"
#include "Hachage.h"
#include "ArbreQuat.h"

int main(int argc, char **argv) {
    // Verification du nombre d'arguments fourni
    if (argc != 3) {
        printf("Usage: %s <entree.cha> <numero de methode>\n1: liste\n2: table de hachage\n3: arbre\n", argv[0]);
        return 1;
    }
    int methode = atoi(argv[2]);
    FILE *fichier = fopen(argv[1], "r");
    if (!fichier) {
        perror("Erreur lors de l'ouverture du fichier d'entrée");
        return 1;
    }

    Chaines *chaines = lectureChaines(fichier);
    fclose(fichier);

    if (!chaines) {
        fprintf(stderr, "Erreur lors de la lecture des chaînes depuis le fichier\n");
        return 1;
    }
    
    Reseau *reseau = NULL;

    char *nomSVG = "";


    switch (methode) {
    case 1: // Utilisation de la liste
            reseau = reconstitueReseauListe(chaines);
            nomSVG = "etat_apres_liste.svg";
            break;
    case 2: // Utilisation de la table de hachage
            {
                int tailleTable = 10; // Taille de la table de hachage, exemple arbitraire
                reseau = reconstitueReseauHachage(chaines, tailleTable);
                nomSVG = "etat_apres_hachage.svg";
            }
            break;
    case 3: // Utilisation de l'arbre quaternaire
            {
                double xmin, ymin, xmax, ymax;
                chaineCoordMinMax(chaines, &xmin, &ymin, &xmax, &ymax);
                ArbreQuat *racine = creerArbreQuat((xmin + xmax) / 2, (ymin + ymax) / 2, xmax - xmin, ymax - ymin);
                reseau = reconstitueReseauArbre(chaines);
                nomSVG = "etat_apres_arbre.svg";
            }
            break;

     default:
            fprintf(stderr, "Numéro de méthode invalide. Entrez un nombre entre 1 et 3.\n");
            libererChaines(chaines); // Libérer la mémoire allouée à chaines
            return 1;
    }

    // Affichage de l'état du réseau après reconstruction
    afficheReseauSVG(reseau, nomSVG);

    // Libération des ressources après utilisation
    libererChaines(chaines);
    libererReseau(reseau);
    return 0;
}