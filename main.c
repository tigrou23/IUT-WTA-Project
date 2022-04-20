#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma warning(disable:4996)

//Limites
enum {
    MAX_TOURNAMENTS = 10,
    MAX_PLAYER = 128,
    MAX_MATCH = 127,
    MAX_LENGTH = 30,
    DATE_LENGTH = 5
};

//Score
enum {
    LOOSE_64 = 10,
    LOOSE_32 = 45,
    LOOSE_16 = 90,
    LOOSE_8 = 180,
    LOOSE_4 = 360,
    LOOSE_2 = 720,
    LOOSE_1 = 1200,
    WIN_1 = 2000,
};

//Match Phase
enum {
    MATCH_64 = 64,
    MATCH_32 = 32,
    MATCH_16 = 16,
    MATCH_8 = 8,
    MATCH_4 = 4,
    MATCH_2 = 2,
    MATCH_1 = 1
};

//Match Phase Count
enum {
    COUNT_64 = 0,
    COUNT_32 = COUNT_64 + MATCH_64,
    COUNT_16 = COUNT_32 + MATCH_32,
    COUNT_8 = COUNT_16 + MATCH_16,
    COUNT_4 = COUNT_8 + MATCH_8,
    COUNT_2 = COUNT_4 + MATCH_4,
    COUNT_1 = COUNT_2 + MATCH_2,
};

//Définition des nouveux types
typedef struct {
    char nom[MAX_LENGTH + 1];
    int points;
} Joueuse;

typedef struct {
    unsigned int idxGagnante, idxPerdante;
} Match;

typedef struct {
    char nom[MAX_LENGTH + 1], date[DATE_LENGTH];
    Match dataMatch[MAX_MATCH];
} Tournoi;

typedef struct {
    Tournoi dataTournois[MAX_TOURNAMENTS];
    Joueuse dataJoueuses[MAX_TOURNAMENTS * MAX_PLAYER];
    unsigned int nbTournois, idxT, idxJ;
} TournoiWTA;

//FONCTIONS UTILITAIRES

//PLAYER

/**
 * Permet de retourner l'index du joueur dans le tournois.
 * @param tournois Tournoi général, contenant les joueurs
 * @param name Nom recherché
 * @return Index du joueur recherché, sinon -1
 */
int player_exists(const TournoiWTA *tournois, const char name[]) {
    for (int i = 0; i < MAX_TOURNAMENTS * MAX_PLAYER; i++) {
        if (strcmp(tournois->dataJoueuses[i].nom, name) == 0) return i;
    }
    return -1;
}

/**
 * Retourne l'index suivant, un élément du tableau qui est vide.
 * @param tournois Tournoi général, contenant les joueurs
 * @return Un index, pour ajouter une nouvelles joueuse.
 */
unsigned int player_get_new_index(TournoiWTA *tournois) {
    return tournois->idxJ++;
}

/**
 * Ajoute des points à une joueuses donné.
 * @param tournois Tournoi général, contenant les joueurs
 * @param name Nom de la Joueuse
 * @param points Nombre de points à donner
 */
void player_add_points(TournoiWTA *tournois, Joueuse playerData[], const char name[], const int points) {
    int playerIdx = player_exists(tournois, name);
    playerData[playerIdx].points += points;
}

/**
 * Créé une nouvelle joueuse si nécéssaire.
 * @param tournois Tournoi général, contenant les joueurs
 * @param name Nom recherché ou a créer
 * @return Une joueuse, créé ou retrouvée
 */
Joueuse *player_create(TournoiWTA *tournois, const char name[]) {
    int check = player_exists(tournois, name);
    if (check != -1) {
        return &tournois->dataJoueuses[check];
    }

    unsigned int idx = player_get_new_index(tournois);
    strcpy(tournois->dataJoueuses[idx].nom, name);
    tournois->dataJoueuses[idx].points = 0;

    return &tournois->dataJoueuses[idx];
}

/**
 * Permet de créer un match tout en donnant des points à la perdante et / ou la gagnante
 * @param tournois Tournoi général, contenant les joueurs
 * @param tournamentCount Index du Tournoi actuel
 * @param startIn Index du match où commencer
 * @param loopCount Nombre de match à renseigner
 * @param scoreToWinner Nombre de point à donner à la gagnante
 * @param scoreToLooser Nombre de point à donner à la perdante
 * @return Un entier, permettant de connaître à quel index le programme à arrêter ses matches
 */
int player_create_match(TournoiWTA *tournois, const int tournamentCount, const int startIn, const int loopCount,
                        const int scoreToWinner,
                        const int scoreToLooser) {
    for (int i = startIn; i < loopCount + startIn; i++) {
        Joueuse pOne;
        Joueuse pTwo;
        scanf("%s %s", pOne.nom, pTwo.nom);
        //Création des joueuses si nécessaire
        Joueuse *gagnante = player_create(tournois, pOne.nom);
        Joueuse *perdante = player_create(tournois, pTwo.nom);

        //Création du match
        int idxG = player_exists(tournois, pOne.nom);
        int idxP = player_exists(tournois, pTwo.nom);
        tournois->dataTournois[tournamentCount].dataMatch[i].idxGagnante = idxG;
        tournois->dataTournois[tournamentCount].dataMatch[i].idxPerdante = idxP;

        //Attribution des points
        player_add_points(tournois, tournois->dataJoueuses, gagnante->nom, scoreToWinner);
        player_add_points(tournois, tournois->dataJoueuses, perdante->nom, scoreToLooser);
    }

    return startIn + loopCount;
}

/**
 * Attribue des joueuse avec des points personnalisé du tournoi général à un tableau de joueuse
 * @param tournois Tournoi général, contenant les joueurs
 * @param players tableau de joueuse où nous allons copier les élements
 * @param idxTournament index du match où les joueuses seront prises
 * @param index Index de la joueuse à copier
 * @param score Score à attribuer
 * @param winner Devons-nous prendre la gagnante ou la perdante ? (Bool)
 */
void player_create_leaderboard(const TournoiWTA *tournois, Joueuse players[],
                               unsigned int idxTournament, unsigned int index,
                               unsigned int score, unsigned int winner) {

    const unsigned int playerIndex = winner
                                     ? tournois->dataTournois[idxTournament].dataMatch[index].idxGagnante
                                     : tournois->dataTournois[idxTournament].dataMatch[index].idxPerdante;
    index += winner;
    players[index] = tournois->dataJoueuses[playerIndex];
    players[index].points = score;
}

/**
 * Retourne l'index suivant, un élément du tableau qui est vide.
 * @attention était utile lorsqu'on ne pouvais pas stocker le nombre de tournoi déjà stocké.
 * @param tournois Tournoi général, contenant les tournois
 * @return Un index, pour ajouter un nouveau tournoi.
 */
unsigned int tournament_get_new_index(TournoiWTA *tournois) {
    return tournois->idxT;
}

/**
 * Permet de retourner l'index d'un tournoi donné.
 * @param tournois Tournoi général, contenant les tournois
 * @param name Nom recherché
 * @return Index du tournoi recherché, sinon -1
 */
int tournament_exists(const TournoiWTA *tournois, const char name[], const char year[]) {
    for (int i = 0; i < MAX_TOURNAMENTS; i++) {
        if ((strcmp(tournois->dataTournois[i].nom, name) == 0) &&
            strcmp(tournois->dataTournois[i].date, year) == 0)
            return i;
    }

    return -1;
}

/**
 * Trie un tableau de joueuse par ordre lexicographique
 * @param playerData Tableau de joueuse à trier
 * @param arraySize Taille du Tableau à trier
 */
void player_sort(Joueuse playerData[], int arraySize) {
    Joueuse v;
    int j;
    for (int i = 1; i < arraySize; i++) {
        v = playerData[i];
        j = i;
        while (j > 0 && strcmp(playerData[j - 1].nom, v.nom) > 0) {
            playerData[j] = playerData[j - 1];
            j -= 1;
        }
        playerData[j] = v;
    }
}

/**
 * Trie un tableau de joueuse par ordre croissant puis par ordre lexicographique
 * @param playerData Tableau de joueuse à trier
 * @param arraySize Taille du Tableau à trier
 */
void player_sort_score(Joueuse playerData[], int arraySize) {
    Joueuse v;
    int j;
    for (int i = 1; i < arraySize; i++) {
        v = playerData[i];
        j = i;
        while ((j > 0 && playerData[j - 1].points < v.points) ||
               (playerData[j - 1].points == v.points && strcmp(playerData[j - 1].nom, v.nom) > 0)) {
            playerData[j] = playerData[j - 1];
            j -= 1;
        }
        playerData[j] = v;
    }
}

//FONCTIONS GENERALES

/**
 * Vérifie si la valeur mise est bien comprise entre 1 et 10
 * Définie le nombre de tournois et la met dans le tournois.
 * Renvoie une assertion si la valeur n'est pas comprise.
 * @param tournois Tournoi général, contenant les tournois
 */
void definir_nombre_tournois(TournoiWTA *tournois) {
    int nbT;
    scanf("%i", &nbT);

    assert(nbT && nbT > 0 && nbT <= MAX_TOURNAMENTS);
    tournois->nbTournois = nbT;
}


/**
 * Enregistre un tournoi, avec ses 127 match en créant automatiquement les joueuses.
 * @param tournois Tournoi général, contenant les tournois
 */
void enregistrement_tournoi(TournoiWTA *tournois) {
    assert(tournois->idxT <= tournois->nbTournois);

    char city[MAX_LENGTH + 1];
    char year[DATE_LENGTH];
    unsigned int idxTournois = tournament_get_new_index(tournois);

    scanf("%s %s", city, year);
    strcpy(tournois->dataTournois[idxTournois].nom, city);
    strcpy(tournois->dataTournois[idxTournois].date, year);
    int startIn = 0;

    startIn = player_create_match(tournois, idxTournois, startIn, MATCH_64, 0, LOOSE_64);   //64ème de finale
    startIn = player_create_match(tournois, idxTournois, startIn, MATCH_32, 0, LOOSE_32);   //32ème de finale
    startIn = player_create_match(tournois, idxTournois, startIn, MATCH_16, 0, LOOSE_16);   //16ème de finale
    startIn = player_create_match(tournois, idxTournois, startIn, MATCH_8, 0, LOOSE_8);   //8ème de finale
    startIn = player_create_match(tournois, idxTournois, startIn, MATCH_4, 0, LOOSE_4);   //quarts de finale
    startIn = player_create_match(tournois, idxTournois, startIn, MATCH_2, 0, LOOSE_2);   //demi-finales
    player_create_match(tournois, idxTournois, startIn, MATCH_1, WIN_1, LOOSE_1);         //finale

    tournois->idxT++;
}

/**
 * Affichage des matchs d'un tournoi donné
 * @param tournois Tournoi général, contenant les tournois
 */
void affichage_matchs_tournoi(const TournoiWTA *tournois) {
    char city[MAX_LENGTH + 1];
    char year[DATE_LENGTH];

    scanf("%s %s", city, year);
    int idxTournament = tournament_exists(tournois, city, year);
    if (idxTournament == -1)  printf("tournoi inconnu");

    printf("%s %s\n", city, year);
    for (int i = 0; i < MAX_MATCH; i++) {
        //Affichage des titres
        if (i == COUNT_64) printf("64emes de finale\n");
        else if (i == COUNT_32)printf("32emes de finale\n");
        else if (i == COUNT_16) printf("16emes de finale\n");
        else if (i == COUNT_8) printf("8emes de finale\n");
        else if (i == COUNT_4) printf("quarts de finale\n");
        else if (i == COUNT_2) printf("demi-finales\n");
        else if (i == COUNT_1) printf("finale\n");

        //affichage des joueuse
        unsigned int iWin = tournois->dataTournois[idxTournament].dataMatch[i].idxGagnante,
                iLoose = tournois->dataTournois[idxTournament].dataMatch[i].idxPerdante;
        printf("%s %s\n", tournois->dataJoueuses[iWin].nom, tournois->dataJoueuses[iLoose].nom);
    }

}

/**
 * Affichage des matchs d'une joueuse donné, d'un tournoi donné.
 * @param tournois Tournoi général, contenant les tournois
 */
void afficher_matchs_joueuse(const TournoiWTA *tournois) {
    char city[MAX_LENGTH + 1], name[MAX_LENGTH + 1], year[DATE_LENGTH];
    scanf("%s %s %s", city, year, name);

    int idxTournament = tournament_exists(tournois, city, year),
    idxPlayer = player_exists(tournois, name);

    if (idxTournament == -1) printf("tournoi inconnu");
    if (idxPlayer == -1) printf("joueuse inconnue");

    for (int i = 0; i < MAX_MATCH; i++) {
        if (tournois->dataTournois[idxTournament].dataMatch[i].idxGagnante == idxPlayer ||
            tournois->dataTournois[idxTournament].dataMatch[i].idxPerdante == idxPlayer) {
            unsigned int iWin = tournois->dataTournois[idxTournament].dataMatch[i].idxGagnante,
                    iLoose = tournois->dataTournois[idxTournament].dataMatch[i].idxPerdante;
            printf("%s %s\n",
                   tournois->dataJoueuses[iWin].nom,
                   tournois->dataJoueuses[iLoose].nom);
        }
    }
}

/**
 * Affichage des joueuses et leurs score d'un tournoi donné (Trié de manière lexicographique)
 * @param tournois Tournoi général, contenant les tournois
 */
void affichage_joueuses_tournoi(const TournoiWTA *tournois) {
    char city[MAX_LENGTH + 1];
    char year[DATE_LENGTH];
    Joueuse playerData[MAX_PLAYER];

    scanf("%s %s", city, year);
    int idxTournament = tournament_exists(tournois, city, year);
    if (idxTournament == -1) printf("tournoi inconnu");

    //Calcul des points
    for (int i = 0; i < MAX_MATCH; i++) {
        if (i < COUNT_32) player_create_leaderboard(tournois, playerData, idxTournament, i, LOOSE_64, 0);
        else if (i < COUNT_16) player_create_leaderboard(tournois, playerData, idxTournament, i, LOOSE_32, 0);
        else if (i < COUNT_8) player_create_leaderboard(tournois, playerData, idxTournament, i, LOOSE_16, 0);
        else if (i < COUNT_4) player_create_leaderboard(tournois, playerData, idxTournament, i, LOOSE_8, 0);
        else if (i < COUNT_2) player_create_leaderboard(tournois, playerData, idxTournament, i, LOOSE_4, 0);
        else if (i < COUNT_1) player_create_leaderboard(tournois, playerData, idxTournament, i, LOOSE_2, 0);
        else {
            player_create_leaderboard(tournois, playerData, idxTournament, i, LOOSE_1, 0);
            player_create_leaderboard(tournois, playerData, idxTournament, i, WIN_1, 1);
        }
    }

    player_sort(playerData, MAX_PLAYER);

    printf("%s %s\n", city, year);
    for (int i = 0; i < MAX_PLAYER; i++) {
        printf("%s %i\n", playerData[i].nom, playerData[i].points);
    }
}

/**
 * Affichage des 4 derniers tounois des joueuses et leurs score par rapport au tournoi général (Trié par le score, puis de manière lexicographique)
 * @param tournois Tournoi général, contenant les tournois
 */
void afficher_classement(TournoiWTA *tournois) {
    Joueuse playerData[MAX_PLAYER * MAX_TOURNAMENTS];
    for (int i = 0; i < MAX_PLAYER * tournois->idxT; i++) {
        playerData[i] = tournois->dataJoueuses[i];
    }
    enum {
        MAX_TOURNAMENT_TO_SHOW = 4
    };
    int nb_tournament = (int)tournament_get_new_index(tournois) - 1;
    if (nb_tournament <= 0) return;
    if (nb_tournament > 9) nb_tournament = 9;

    if (nb_tournament >= MAX_TOURNAMENT_TO_SHOW) {
        for (int t = 0; t <= nb_tournament - MAX_TOURNAMENT_TO_SHOW; t++) {
            for (int j = 0; j < MAX_MATCH; j++) {

                unsigned int idxG = tournois->dataTournois[t].dataMatch[j].idxGagnante;
                unsigned int idxP = tournois->dataTournois[t].dataMatch[j].idxPerdante;

                if (j < COUNT_32) player_add_points(tournois, playerData, tournois->dataJoueuses[idxP].nom, -LOOSE_64);
                else if (j < COUNT_16)
                    player_add_points(tournois, playerData, tournois->dataJoueuses[idxP].nom, -LOOSE_32);
                else if (j < COUNT_8)
                    player_add_points(tournois, playerData, tournois->dataJoueuses[idxP].nom, -LOOSE_16);
                else if (j < COUNT_4)
                    player_add_points(tournois, playerData, tournois->dataJoueuses[idxP].nom, -LOOSE_8);
                else if (j < COUNT_2)
                    player_add_points(tournois, playerData, tournois->dataJoueuses[idxP].nom, -LOOSE_4);
                else if (j < COUNT_1)
                    player_add_points(tournois, playerData, tournois->dataJoueuses[idxP].nom, -LOOSE_2);
                else {
                    player_add_points(tournois, playerData, tournois->dataJoueuses[idxP].nom, -LOOSE_1);
                    player_add_points(tournois, playerData, tournois->dataJoueuses[idxG].nom, -WIN_1);
                }
            }
        }
    }

    player_sort_score(playerData, MAX_PLAYER * tournois->idxT);
    for (int i = 0; i < MAX_PLAYER * tournois->idxT; i++) {
        if (playerData[i].points > 0)
            printf("%s %i\n", playerData[i].nom, playerData[i].points);
    }
}

//Main où tout se lance, c'est qu'ici tout commence, ici que tout se créé. Observez la magie.
int main(void) {
    char command[MAX_LENGTH + 1];
    TournoiWTA ins;
    ins.idxT = 0;
    ins.idxJ = 0;

    while (1) {
        scanf("%s", command);
        if (strcmp(command, "exit") == 0) exit(0);
        else if (strcmp(command, "definir_nombre_tournois") == 0) definir_nombre_tournois(&ins);
        else if (strcmp(command, "enregistrement_tournoi") == 0) enregistrement_tournoi(&ins);
        else if (strcmp(command, "affichage_matchs_tournoi") == 0) affichage_matchs_tournoi(&ins);
        else if (strcmp(command, "affichage_joueuses_tournoi") == 0) affichage_joueuses_tournoi(&ins);
        else if (strcmp(command, "afficher_matchs_joueuse") == 0) afficher_matchs_joueuse(&ins);
        else if (strcmp(command, "afficher_classement") == 0) afficher_classement(&ins);
        else printf("Commande inexistante, merci de reessayer.\n");
    }
}