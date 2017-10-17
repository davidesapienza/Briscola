#include "struttura_dati.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>

using namespace std;

extern partita_t partita;

/** Carica i nomi delle carte dal file e li mette nel mazzo[].carta[].
 */
static void carica_file(){

	ifstream f(NOME_FILE) ;
      	if (!f) {
		cout<<"Errore in apertura del file "<<NOME_FILE<<endl ;
		}
	
	for(int i=0; i<NUM_CARTE; i++){
		int j=0;
		while(j<MAX_LUN_RIGA1){
			f>>partita.mazzo[i].nome[j];
			j++;
			}
		partita.mazzo[i].nome[j] = '\0';
		}
		
}




/** Funzione che gestisce la fine di entrambi i turni.

 *  abilitare il bottone del mazzo, congelando quelli delle carte della mano,
 */
/*void fine_mano(){
	

}*/


/** Mi genera numeri compresi tra 0 e 39, che mi rappresentano le carte.
 */
static void genera_nemero_carte(){

	/* Utilizzo un vettore ausiliario per dirmi se ho già oppure no 
	 * la carta, perchè non devo avere 2 carte uguali.
	 * In realtà questo array funziona al contrario:
	 * mi dice se nella posizione creata c'è già la carta.
	 */
	bool vet_aus[NUM_CARTE];
	// Inizializza vet_aus a false.
	for(int j=0; j<NUM_CARTE; j++)
		vet_aus[j]=false;
		
	/* Questo for in realtà funziona al contrario rispetto a quello che uno 
	 * potrebbe pensare:
	 * i--> rappresenta il numero e la carta vera e propria
	 * j--> l'indice del mazzo di carte
	 * Quindi trova un indice compreso tra 0 e 39, e se in quella posizione,
	 * vet_aus è già true allora non lo accetta, altrimenti memorizza la carta i.
	 */
	for(int i=0; i<NUM_CARTE; i++){
		int j=0;
		/* Genera un numero casuale. Questo numero in realtà rappresenterà
		 * l'indice del vettore mazzo.
		 */
		j=rand()%NUM_CARTE;

		/* Finchè il numero è maggiore di 40, lo divide proprio per 40, 
		 * e tiene il resto della divisione.
		 */
		/*while(j>=40){
			j=j%40;
			}*/

		/* Controlla che nella posizione indicata da j non sia già presente 
		 * un'altra carta, e se libera memorizza la carta i.
		 */
		if(vet_aus[j]==false){
			vet_aus[j]=true;
			partita.mazzo[j].val=i;
			}
		/* Altrimenti rimano ancora sulla carta i, perchè non l' hai ancora
		 * memorizzata.
		 */
		else
			i--;
		}
}




	
/** Riordina i nomi delle carte in base al vettore dei val.
 */
static void genera_nome_carte(){
	
	carica_file();
	
	/** Riordina l'array mazzo[].nome.[].
	 */
	for(int j=0; j<NUM_CARTE; j++){
		cout<<partita.mazzo[j].val+1<<"  ";
		}
	cout<<endl;

	char aus[NUM_CARTE][MAX_LUN_RIGA2+1];
	
	for(int i=0; i<NUM_CARTE; i++){
		strcpy(aus[i], partita.mazzo[partita.mazzo[i].val].nome);
		strcpy(partita.mazzo[partita.mazzo[i].val].nome, "./Carte/");
		strcat(partita.mazzo[partita.mazzo[i].val].nome, aus[i]);
		}
	

	for(int i=0; i<NUM_CARTE; i++){
		strcpy(aus[i], partita.mazzo[partita.mazzo[i].val].nome);
		}

	for(int i=0; i<NUM_CARTE; i++){
		strcpy(partita.mazzo[i].nome, aus[i]);
		}

	for(int j=0; j<NUM_CARTE; j++){
		cout<<partita.mazzo[j].nome<<"  ";		
		}
	cout<<endl;

	
	

}



/** Funzione che genera il mazzo casualmente: mescola il mazzo.
 */
void genera_mazzo(){
	
	/** Mi ganera partita.mazzo[].val.
	 */
	genera_nemero_carte();

	/** Mi ganera partita.mazzo[].nome[].
	 */
	genera_nome_carte();

	/* for che serve a me per stampare le carte e vedere che non ci sono doppioni.
	 */
	for(int j=0; j<NUM_CARTE; j++){
		cout<<partita.mazzo[j].nome<<endl;
		cout<<partita.mazzo[j].val+1<<"  "<<endl;
		}
}




