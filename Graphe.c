#include "Graphe.h"
#include "Reseau.h"
#include "Struct_File.h"
#include "Struct_Liste_Entiers.h"

// Retourne l'indice du sommet dans le tableau des sommets du graphe ou -1 s'il n'est pas trouvé
int cherche_sommet(Graphe *g, int x, int y) {
    Sommet *s = g->T_som[0];

    int i = 0;
    while (s) {
        if (s->x == x && s->y ==y) 
            return i;
        i++;
        s = g->T_som[i];
    }

    return -1;
}

// Crée un nouvelle arête et l'insère en tête dans les voisins de s1 et de s2
void creation_arete(Graphe *g, Sommet *s1, Sommet *s2) {
    Arete *a = (Arete *)sizeof(Arete);

    // Si s1 a déjà une liste d'arêtes initialisée
    Cellule_arete* s1_voisins = s1->L_voisin;
    Cellule_arete* s2_voisins = s2->L_voisin;

    // Nouvelle arrete
    Cellule_arete* cell_arete = (Cellule_arete *)malloc(sizeof(Cellule_arete));
    Arete* arete = (Arete*)malloc(sizeof(Arete));
    cell_arete->a = arete;
    cell_arete->suiv = NULL;

    arete->u = cherche_sommet(g, s1->x, s1->y);
    arete->v = cherche_sommet(g, s2->x, s2->y);
    // Insertion en tête si la liste des voisins existe déjà dans s1
    if (s1_voisins) {
        cell_arete->suiv = s1_voisins;
    } 

    s1->L_voisin = cell_arete;

    // Insertion en tête si la liste des voisins existe déjà dans s2
    if (s2_voisins) {
        cell_arete->suiv = s2_voisins;
    } 

    s2->L_voisin = cell_arete;
}

Graphe* creerGraphe(Reseau* r) {
    Graphe* g = (Graphe*)malloc(sizeof(Graphe));
    // Simple initialisation en copiant les attributs du reseau dans le graphe
    g->nbsom = r->nbNoeuds;
    g->nbcommod = nbCommodites(r);
    g->gamma = r->gamma;

    CellNoeud* noeud_courant = r->noeuds;
    Sommet* sommet_courant;
    Sommet* sommet_voisin;
    Arete* arete_courant;
    CellNoeud* voisin_courant;

    // Allocation du tableau des sommets
    g->T_som = (Sommet **)malloc(sizeof(Sommet *)*r->nbNoeuds);

    // On crée tous les sommets et on les mets numérotés dans le graphe
    int i=0;
    while (noeud_courant) {
        // Si on a déjà créer le sommet précédemment pas besoin de l'alloué
        int index_sommet_existant = cherche_sommet(g, noeud_courant->nd->x, noeud_courant->nd->y); // -1 s'il n'existe pas sinon l'indice du sommet dans g->Tsom
        if (index_sommet_existant > -1) {
            sommet_courant = g->T_som[index_sommet_existant];
        } else {
            sommet_courant = (Sommet *)malloc(sizeof(Sommet));
            // Copie des coordonnées
            sommet_courant->x = noeud_courant->nd->x;
            sommet_courant->y = noeud_courant->nd->y;
            sommet_courant->num = i;
            g->T_som[i] = sommet_courant;
            i++; //dans ce cas on doit incrémenter i
        }

        // On initialise la tête de liste au premier des voisins de la liste des voisins du noeud courant
        voisin_courant = noeud_courant->nd->voisins;
        // Notre méthode est aussi de créer tous les sommets voisins
        while (voisin_courant) {
            int index_sommet_voisin_existant = cherche_sommet(g, voisin_courant->nd->x, voisin_courant->nd->y);
            // Dans le cas où le sommet existe déjà il ne faut surout pas le recréer
            if (index_sommet_voisin_existant > -1) {
                sommet_voisin = g->T_som[index_sommet_voisin_existant];
            } else {
                sommet_voisin = (Sommet*)malloc(sizeof(Sommet));
                sommet_voisin->num = i;
                sommet_voisin->x = voisin_courant->nd->x;
                sommet_voisin->y = voisin_courant->nd->y;
                // On créer une arête uniquement dans ce cas car le sommet est nouveau donc il ne peut pas encore avoir d'arête avec le sommet courant
                creation_arete(g, sommet_courant, sommet_voisin);

                g->T_som[i] = sommet_voisin;
                i++; //On doit aussi incrémenter i
            }
            voisin_courant = voisin_courant->suiv;
        }
        
        noeud_courant = noeud_courant->suiv;
    }

    // commodités du réseau
    CellCommodite* commodite_courante = r->commodites;
    // commodités du graphe 
    Commod* commod;
    // Allocation du tableau de commodités !!ce ne sont pas des pointeurs sur des commodités mais bien des commodités directement 
    g->T_commod = (Commod *)malloc(sizeof(Commod)*g->nbcommod);
    int indice_tab_commod=0;
    // On crée les commodités
    while (commodite_courante) {
        // Première extrémité        
        g->T_commod[indice_tab_commod].e1=cherche_sommet(g, commodite_courante->extrA->x, commodite_courante->extrA->y);
        // Seconde extrémité
        g->T_commod[indice_tab_commod].e2=cherche_sommet(g, commodite_courante->extrB->x, commodite_courante->extrB->y);

        indice_tab_commod++;
        commodite_courante = commodite_courante->suiv;
    }

    return g;
}

// 1. On enfile le sommet de départ
// 2. On enfile les sommets adjacents à la tête de file
// 3. On défile
// 4. Tant que la file n'est pas vide, on ré-itère les points 2 et 3
int parcours_en_largeur(Graphe *g, Sommet* u, Sommet* v) {
    // Si u et v sont les mêmes sommets alors le plus court chemin est nul
    if (u->num == v->num) return 0; 

    // Initialisation du tableau pour marquer les sommets visités
    int* visites = (int*)malloc(g->nbsom * sizeof(int));
    for (int i = 0; i < g->nbsom; ++i)
        visites[i] = 0;

    // Initialisation du tableau pour noter les distances de chaque sommet à u
    int* tab_distances = (int*)malloc(g->nbsom * sizeof(int));
    for (int i = 0; i < g->nbsom; ++i)
        tab_distances[i] = 0;

    int plus_court_chemin = 1;
    int valeur_tete_file;
    // création de la file
    S_file* file = (S_file *)malloc(sizeof(S_file));
    // Initialisation de la file
    Init_file(file);

    // On enfile le sommet de départ
    enfile(file, u->num);
    visites[u->num] = 1;

    while(!estFileVide(file)) {
        // On défile
        valeur_tete_file = defile(file);
        // On enfile les sommets adjacents à la tête de file
        Cellule_arete* voisins = g->T_som[valeur_tete_file]->L_voisin;
        while(voisins) {
            // on doit traîter les deux cas car on ne sait pas si le sommet voisin est u ou v dans l'arête
            if(voisins->a->u == valeur_tete_file) {
                if(!visites[voisins->a->v]) {
                    enfile(file, voisins->a->v);
                    visites[voisins->a->v] = 1;
                }
            } else if(voisins->a->v == valeur_tete_file) {
                if(!visites[voisins->a->u]) {
                    enfile(file, voisins->a->u);
                    visites[voisins->a->u] = 1;
                }
            }
            // On enfile le sommet voisin
            voisins = voisins->suiv;
        }
    }

    free(file);
}

