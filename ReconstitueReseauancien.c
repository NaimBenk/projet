#include <stdio.h>
#include <stdlib.h>
#include "Chaine.h"
#include "Reseau.h"
#include "Hachage.h"
#include "Reseau.h"

int main(int argc, char **argv) {
    // Verification du nombre d'arguments fourni
    if (argc != 3) {
        printf("Usage: %s <entree.cha> <numero de methode>\n1: liste\n2: table de hachage\n3: arbre\n", argv[0]);
        return 1;
    }
    int rep = atoi(argv[2]);
    switch (rep)
    {
    case 1: //liste
        {
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
            reconstitueReseauListe(C);
        }
        break;
    case 2: //table de hachage
        {
            // Ouvrir le fichier d'entrée pour lecture
            FILE *entree = fopen(argv[1], "r");
            if (entree == NULL) {
                perror("Erreur lors de l'ouverture du fichier d'entrée");
                return 1;
            }

            // Lire les chaînes à partir du fichier d'entrée
            Chaines *C = lectureChaines(entree);
            fclose(entree); // Fermer le fichier d'entrée

            if (C == NULL) {
                fprintf(stderr, "Erreur lors de la lecture des chaînes depuis le fichier\n");
                return 1;
            }

            // Ici, vous devez définir M, la taille de votre table de hachage
            int M = 10; // Exemple, peut-être déterminé dynamiquement ou configuré autrement
            Reseau *R = reconstitueReseauHachage(C, M);
            
            // Assurez-vous de libérer la mémoire allouée à C et R après utilisation
            // Libérer les ressources de C et R non montré ici
        }
        break;
    case 3: //arbre
        /* code */
        break;

    default:
        printf("Entrez un nombre entre 1 et 3\n");
        break;
    }
    
    return 0;
}