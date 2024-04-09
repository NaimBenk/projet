#include <stdio.h>
#include "Chaine.h"

int main(int argc, char **argv) {
    // Verification du nombre d'arguments fourni
    if (argc != 3) {
        printf("Usage: %s <entree> <sortie>\n", argv[0]);
        return 1;
    }

    // Ouvrir le fichier d'entrée pour lecture
    FILE *entree = fopen(argv[1], "r");
    if (entree == NULL) {
        perror("Error opening input file");
        return 1;
    }

    // Lire les chaînes à partir du fichier d'entrée
    Chaines *C = lectureChaines(entree);
    fclose(entree); // Fermer le fichier d'entrée car on en a plus besoin

    if (C == NULL) {
        fprintf(stderr, "Error reading chains from file\n");
        return 1;
    }

    // Ouvrir le fichier de sortie pour l'écriture
    FILE *sortie = fopen(argv[2], "w");
    if (sortie == NULL) {
        perror("Error opening output file");
        return 1;
    }

    // Ecrire les chaînes dans le fichier de sortie
    ecrireChaines(C, sortie);
    fclose(sortie); // Fermer le fichier de sortie après l'écriture


    afficheChainesSVG(C, "visuel_chaines");
    printf("%d \n", comptePointsTotal(C));

    // Librere la mémoire allouée pour les chaines et effectuez un nettoyage

    return 0;
}