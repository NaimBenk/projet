#include "Graphe.h"
#include "Reseau.h"

#include "Struct_File.h"
#include "Reseau.h"

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
int plus_petite_distance(Graphe *g, Sommet* u, Sommet* v) {
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
                    tab_distances[voisins->a->v]=tab_distances[voisins->a->u]+1;
                }
            } else if(voisins->a->v == valeur_tete_file) {
                if(!visites[voisins->a->u]) {
                    enfile(file, voisins->a->u);
                    visites[voisins->a->u] = 1;
                    tab_distances[voisins->a->u]=tab_distances[voisins->a->v]+1;
                }
            }
            voisins = voisins->suiv;
        }
    }

    free(visites);
    free(tab_distances);
    free(file);

    return tab_distances[v->num];
}

// Cette fontion renvoie un pointeur sur la tête de liste d'entiers
Cellule_liste_entiers* creer_liste_parcours(Graphe *g, Sommet* u, Sommet* v) {

    // initialisation de l'arborescence 
    Noeud_abr* courant_abr=(Noeud_abr*)malloc(sizeof(Noeud_abr));
    courant_abr->val=u->num;
    courant_abr->pere=NULL;
    //stockage de tous les noeuds de l'arbre dans un tableau
    Noeud_abr** tab_arbre=(Noeud_abr**)malloc(sizeof(Noeud_abr *)*g->nbsom);
    tab_arbre[u->num]=courant_abr;

    // Initialisation du tableau pour marquer les sommets visités
    int* visites = (int*)malloc(g->nbsom * sizeof(int));
    for (int i = 0; i < g->nbsom; ++i)
        visites[i] = 0;

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
        // on récupère le noeud de l'arbre correspondant à la tête de file
        courant_abr=tab_arbre[valeur_tete_file];
        while(voisins) {
            // création d'un nouveau noeud dans l'arbre
            Noeud_abr* nouv_noeud=(Noeud_abr*)malloc(sizeof(Noeud_abr));
            // Les nouveaux noeuds ont pour père le sommet qui les précède
            nouv_noeud->pere=courant_abr;
            // on doit traîter les deux cas car on ne sait pas si le sommet voisin est u ou v dans l'arête
            if(voisins->a->u == valeur_tete_file) {
                if(!visites[voisins->a->v]) {
                    enfile(file, voisins->a->v);
                    visites[voisins->a->v] = 1;
                    nouv_noeud->val=voisins->a->v;
                    tab_arbre[voisins->a->v]=nouv_noeud;
                }
            } else if(voisins->a->v == valeur_tete_file) {
                if(!visites[voisins->a->u]) {
                    enfile(file, voisins->a->u);
                    visites[voisins->a->u] = 1;
                    nouv_noeud->val=voisins->a->u;
                    tab_arbre[voisins->a->u]=nouv_noeud;
                }
            }
            voisins = voisins->suiv;
        }
    }


    //création de la liste d'entier, le chaine va être retourné de v à u
    Cellule_liste_entiers* liste_chaine=(Cellule_liste_entiers*)malloc(sizeof(Cellule_liste_entiers));
    liste_chaine->nb=v->num;
    Cellule_liste_entiers* tete_liste=liste_chaine;

    Noeud_abr* noeud_chaine=tab_arbre[v->num]->pere;

    while (noeud_chaine) {
        Cellule_liste_entiers* suiv_chaine=(Cellule_liste_entiers*)malloc(sizeof(Cellule_liste_entiers));
        suiv_chaine->nb=noeud_chaine->val;
        noeud_chaine=noeud_chaine->pere;
        liste_chaine->suiv=suiv_chaine;
        liste_chaine=suiv_chaine;
    }

    //libération du tableau de noeud
    for (int i=0; i<g->nbsom; i++) {
        free(tab_arbre[i]);
    }

    free(tab_arbre);

    //libération de la file
    free(file);

    free(visites);

    return tete_liste;
}

int reorganiseReseau(Reseau* r) {
    Graphe* g=creerGraphe(r);
    int gamma=g->gamma;

    int** mat=(int**)malloc(sizeof(int *)*g->nbsom);

    int i;
    int j;
    // initialisation de la matrice avec uniquement des 0
    for (i=0; i<g->nbsom; i++) {
        mat[i]=(int*)malloc(sizeof(int)*g->nbsom);
        for (j=0; j<g->nbsom; j++) {
            mat[i][j]=0;
        }
    }

    Cellule_liste_entiers* courant;
    Cellule_liste_entiers* suivant;

    for (i=0; i<g->nbcommod; i++) {
        // Calcule la plus courte chaîne pour chaque commodité
        plus_petite_distance(g, g->T_som[g->T_commod[i].e1], g->T_som[g->T_commod[i].e2]);
        // Parcours les arêtes du plus court chemin entre e1 et e2
        courant=creer_liste_parcours(g, g->T_som[g->T_commod[i].e1], g->T_som[g->T_commod[i].e2]);
        suivant=courant->suiv;

        // On parcourt la liste d'entiers
        while (suivant) {
            // Étant donné que nous avons une matrice sommet-sommet, elle est symétrique
            mat[courant->nb][suivant->nb]++;
            mat[suivant->nb][courant->nb]++;

            courant=suivant;
            suivant=suivant->suiv;
        }
    }

    // verifie que toutes les valeurs de la matrice sont inférieures à gamma
    for (i=0; i<g->nbsom; i++) {
        for (j=0; j<g->nbsom; j++) {
            if (mat[i][j]>=gamma) return 0;
        }
    }

    return 1;
}

