#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define NBRE_METROS 10
#define NBRE_SEGMENTS 5

enum SEGMENTS{AB,BA=0,BC,CB=1,CD,EC,FB};

int MR1[] = {AB,BC,CD}; /*La ligne de métro MR1 lie entre A et D*/
int MR2[] = {FB,BA}; /*La ligne de métro MR2 lie entre F et A*/
int MR3[] = {EC,CB}; /*La ligne de métro MR3 lie entre E et B*/

char CHEMINS[NBRE_SEGMENTS][8] = {"A --- B", "B --- C","C --- D","C -- E","B --- F"};// Pour raison d'affichage du chemin de chaque métro

typedef struct Donnees_Metro {
	int ID;
	int* ligne_metro; //MR1 ou MR2 ou MR3
}* donnees_metro;

sem_t sem_segments [NBRE_SEGMENTS];

void acceder_au_segment(int segment, int ID_metro) 
{
	sem_wait(&sem_segments[segment]);	
	printf("Metro n° %d entre le segment %s \n",ID_metro, CHEMINS+segment);
}
void quitter_le_segment(int segment, int ID_metro)
{
	printf("Metro n° %d quitte le segment %s \n",ID_metro, CHEMINS+segment);
	sem_post(&sem_segments[segment]);
}
void* metro(void* infos_metro) 
{
	int ID = ((donnees_metro)infos_metro)->ID;
	// Tant que qu\'il y a d\'autres segments avant d'arriver au terminus; continuer d'acceder aux segments en respectant les règles du partage
	for(int nextSegment = 0; nextSegment<sizeof(((donnees_metro)infos_metro)->ligne_metro)/sizeof(int); nextSegment++)
	{
		acceder_au_segment((((donnees_metro)infos_metro)->ligne_metro)[nextSegment], ID);
		usleep(6000000); // 6  sec -> 6 min
		quitter_le_segment((((donnees_metro)infos_metro)->ligne_metro)[nextSegment], ID);
	}	
}

int main()
{
	pthread_t metros[NBRE_METROS];
	for(int i = 0; i < NBRE_SEGMENTS; i++)
		sem_init(&sem_segments[i],0,1); // Chaque segment ne peut être ocuupé que par un seul métro

	for(int i = 0; i < NBRE_METROS; i++)
	{
		donnees_metro infos_metro = (donnees_metro) malloc(sizeof(struct Donnees_Metro));
		infos_metro->ID = i + 1;
		if (i<NBRE_METROS/3) { // Les métros n° 1, n° 2 et n° 3 auront la ligne de métro MR1 (A ---> D)  
			infos_metro->ligne_metro = (int*) malloc(sizeof(MR1));
			for(int i = 0; i < sizeof(MR1)/sizeof(int); i++)
				*(infos_metro->ligne_metro + i) = *(MR1+i);
		}
		else if ( i<2*NBRE_METROS/3) { // Les métros n°4, n°5 et n°6 auront la ligne de métro MR2 (F ---> A)
			infos_metro->ligne_metro = (int*) malloc(sizeof(MR2));
			for(int i = 0; i < sizeof(MR2)/sizeof(int); i++)
				*(infos_metro->ligne_metro + i) = *(MR2+i);
		}
		else { // Les métros n°7, n°8, n°9 et n°10 auront la ligne de métro MR3 (E ---> C)
			infos_metro->ligne_metro = (int*) malloc(sizeof(MR3));
			for(int i = 0; i < sizeof(MR3)/sizeof(int); i++)
				*(infos_metro->ligne_metro + i) = *(MR3+i);
		}
		     
		pthread_create(&metros[i],NULL,metro, (void*)infos_metro);
	}
	for(int i = 0; i < NBRE_METROS; i++)
		pthread_join(metros[i],NULL);

	return 0;
}
