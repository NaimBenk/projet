#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Chaine.h"
#include "Reseau.h"
#include "Hachage.h"
#include "ArbreQuat.h"

Chaines* generationAleatoire(int nbChaines, int nbPointsChaine, int xmax, int ymax) {
    //on alloue la mémoire d'une chaine
    Chaines *chaines = (Chaines *)malloc(sizeof(Chaines));
    if (chaines == NULL) {
        perror("Erreur d'allocation de mémoire");
        return NULL;
    }
    //On initialise la chaine
    chaines->gamma = 3;
    chaines->nbChaines = nbChaines;
    chaines->chaines = NULL;

    //on initialise le générateur de nombres aléatoires
    srand((unsigned int)time(NULL));

    //on crée un nombre de chaines égal à nbChaines
    for (int i = 0; i < nbChaines; i++) {
        //on ccrée une nouvelle chaine
        CellChaine *chaine = (CellChaine *)malloc(sizeof(CellChaine));
        if (chaine == NULL) {
            perror("Erreur d'allocation de mémoire");
            return NULL;
        }
        chaine->numero = i;
        chaine->points = NULL;
        chaine->suiv = NULL;

        //boucle pour ajouter nbPointsChaine points à la chaîne
        CellPoint *dernierPoint = NULL;
        for (int j = 0; j < nbPointsChaine; j++) {
            //génération aléatoire de coordonnées x et y
            double x = (double)rand() / RAND_MAX * xmax;
            double y = (double)rand() / RAND_MAX * ymax;

            //on crée un nouveau point
            CellPoint *point = (CellPoint *)malloc(sizeof(CellPoint));
            if (point == NULL) {
                perror("Erreur d'allocation de mémoire");
                return NULL;
            }
            //on arrondi au centième près pour plus de lisibilité
            point->x = round(x * 100.0) / 100.0;;
            point->y = round(y * 100.0) / 100.0;;
            point->suiv = NULL;

            //on ajoute le point à la chaîne
            if (dernierPoint == NULL) {
                chaine->points = point;
            } else {
                dernierPoint->suiv = point;
            }
            //mise a jour du dernier point
            dernierPoint = point;
        }

        //on ajoute la chaîne à la liste des chaînes de Chaines
        chaine->suiv = chaines->chaines;
        chaines->chaines = chaine;
    }
    //on retourne la chaine
    return chaines;
}


void mesureTempsCSV(const char *fichierCSV){
    FILE *CSV = fopen(fichierCSV, "w");
    if (!CSV) {
        fprintf(stderr, "Erreur d'ouverture du fichier");
        return;
    }
    //on commence à construire le fichier CSV en lui indiquant le nom des différentes colonnes
    fprintf(CSV,"NbPoints, tempsListe,tempsHachage500, tempsHachage2500, tempsHachage5000, tempsHachage10000,tempsArbre\n");
    //on fait varier le nombre de chaines entre 500 et 5000 avec un pas de 500 à chaque itération
    for(int i=500;i<=5000;i+=500){
        //on génère aléatoirement 100 points pour chaque chaîne avec des coordonnées x et y pouvant aller de 0 à 5000
        Chaines *C=generationAleatoire(i,100,5000,5000);
        if (C == NULL) {
            fprintf(stderr, "Erreur de lecture de la chaine\n");
            return;
        }
        //on calcule le temps d'execution pour la liste chainée
        clock_t debut = clock();
        Reseau *RListe = reconstitueReseauListe(C);
        clock_t fin = clock();
        double tempsListe = ((double)(fin - debut)) / CLOCKS_PER_SEC;
        free(RListe);
        //on calcule le temps d'execution pour l'arbre
        debut = clock();
        Reseau *RArbre = reconstitueReseauArbre(C);
        fin = clock();
        double tempsArbre = ((double)(fin - debut)) / CLOCKS_PER_SEC;
        free(RArbre);
        //on calcule le temps d'execution pour la table de hachage taille 500
        debut = clock();
        Reseau *RHachage = reconstitueReseauHachage(C, 500);
        fin = clock();
        double tempsHachage500 = ((double)(fin - debut)) / CLOCKS_PER_SEC;
        free(RHachage);
        //on calcule le temps d'execution pour la table de hachage taille 2500
        debut = clock();
        RHachage = reconstitueReseauHachage(C, 2500);
        fin = clock();
        double tempsHachage2500 = ((double)(fin - debut)) / CLOCKS_PER_SEC;   
        free(RHachage);
        //on calcule le temps d'execution pour la table de hachage taille 5000
        debut = clock();
        RHachage = reconstitueReseauHachage(C, 5000);
        fin = clock();
        double tempsHachage5000 = ((double)(fin - debut)) / CLOCKS_PER_SEC;   
        free(RHachage);
        //on calcule le temps d'execution pour la table de hachage taille 10000
        debut = clock();
        RHachage = reconstitueReseauHachage(C, 10000);
        fin = clock();
        double tempsHachage10000 = ((double)(fin - debut)) / CLOCKS_PER_SEC;   
        free(RHachage);

        free(C);
        //on écrit à chaque tour de boucle le temps effectué par chaque structure pour un nombre de points donné
        fprintf(CSV,"%d,%lf,%lf,%lf,%lf,%lf,%lf\n",
                i,
                tempsListe,
                tempsHachage500,
                tempsHachage2500,
                tempsHachage5000,
                tempsHachage10000,
                tempsArbre);
    }
    //on ferme le fichier
    fclose(CSV);
}




void tracerGraphiqueCSV(const char *nomFichierCSV) {
    FILE *graphique = popen("gnuplot -persist", "w");
    if (!graphique) {
        perror("Erreur d'ouverture du pipe gnuplot");
        return;
    }

    //on configure le séparateur de données pour lire le fichier CSV 
    //chaque colonne contient une information
    fprintf(graphique, "set datafile separator \',\'\n");
    
    //on configure le nom des axes ainsi que le titre du graphique
    fprintf(graphique, "set xlabel \'Nb points\'\n");
    fprintf(graphique, "set ylabel \'Temps (s)\'\n");
    fprintf(graphique, "set title \'Comparaison des temps\'\n");
    
    //on configure le style des courbes
    fprintf(graphique, "set style data linespoints\n");
    
    //on trace les courbes en utilisant les colonnes du fichier CSV
    //la premiere pour les listes chaines, la deuxiemes table de hachage taille 500, etc ...
    fprintf(graphique, "plot \'%s\' using 1:2 title \'Liste Chainee\' with linespoints,\\\n", nomFichierCSV);
    fprintf(graphique, "\'%s\' using 1:3 title \'Hachage500\' with linespoints,\\\n", nomFichierCSV);
    fprintf(graphique, "\'%s\' using 1:4 title \'Hachage2500\' with linespoints,\\\n", nomFichierCSV);
    fprintf(graphique, "\'%s\' using 1:5 title \'Hachage5000\' with linespoints,\\\n", nomFichierCSV);
    fprintf(graphique, "\'%s\' using 1:6 title \'Hachage10000\' with linespoints,\\\n", nomFichierCSV);
    fprintf(graphique, "\'%s\' using 1:7 title \'Arbre\' with linespoints\n", nomFichierCSV);
    pclose(graphique);

}


int main(int argc, char **argv){
    //on initialise le fichier CSV
    const char *fichierCSV="courbe.csv";
    //on appelle la fontion permettant de mesurer le temps d'execution et on stocke les données dans le CSV
    mesureTempsCSV(fichierCSV);
    //on trace le graphique à partir des données du fichier CSV
    tracerGraphiqueCSV(fichierCSV);
    return 0;

}