/* Strutture dati */

/** Nome del file che contiene tutti i nomi dei file immagine delle carte
 */
const char NOME_FILE[] = "nomi_carte.txt";
/** Lunghezza del nome memorizzato nel NOME_FILE.
 */ 
const int MAX_LUN_RIGA1 = 7;
/** Lunghezza del nome del file immagine completo, contiene il percorso relativo.
 */ 
const int MAX_LUN_RIGA2 = 15;
/** Numero di carte che compone il mazzo.
 */
const int NUM_CARTE = 40;
/** Numero di carte che compondono la mano di gioco.
 */
const int NUM_MANO = 3;
/** Vettore che contiene i pesi delle rispettive carte.
 */
const int PESI[10]={11,0,10,0,0,0,0,2,3,4};
/** Vettore che contiene le precedenze per valore.
 */
const int PREC[10]={0,9,1,8,7,6,5,4,3,2};


/** Rappresenta la mia carta, con i seguenti campi:
 *  val  --> numero generato casualmente, compreso tra 0 e 39, uno per ogni carta. 
 *  nome --> nome del file immagine della carta.
 *  selez--> Valore booleano per dirmi la carta giocata.
 *  esiste --> Valore booleano per dirmi se esiste la carta, 
	       campo che mi serve per le ultime tre mano, che sono un eccezione 
	       rispetto alle altre. 
 */
struct carta_t{
	int val;
	char nome[MAX_LUN_RIGA2+1];
	bool selez;
	bool esiste;
	};


/** Tipo enum, che mi dice se gioca il giocatoreA o B.
 */
enum gioca_t {giocatoreA, giocatoreB};


/** Tipo enum, che mi dice chi gioca per primo, e quindi chi pesca.
 */ 
enum turno_t {primo, secondo}; 

/** Tipo enum, mi definisce la priorità di una carta:
 *  priorità massima se è briscola, priorità media alla carta fiocata per prima,
 *  priorità bassa per la seconda carta giocata.
 */
enum prior_carta_t {brisc, first, second};

/** Rappresenta la struttura del singolo giocatore, contiene le info del giocatore:
 *  mano --> vettore che rappresenta la mano del giocatore, di tipo carta.
 *  punteggio --> valore intero che incrementa in base alle mani che vince.
 */
struct giocatore_t{
	carta_t mano[NUM_MANO];
	int punteggio;
	};


/** Rappresenta la struttura del mio gioco:
 *  mazzo --> vettore che rappresenta il mio mazzo.
 *  idx   --> indice che scorre il mazzo. 
 	      (segnala la fine del gioco e per la carta da pescare a fine turno).
 *  player1 e player2
	  --> rappresentano i due giocatori.
 *  carte_scese
	  --> vettore che segna l'ordine delle carte che scendono volta per volta.
	      nella prima posizione segna l'indice a qui è arrivato, quindi la dimensione è 
	      il numero delle carte più 1.
 */
struct partita_t{
	carta_t mazzo[NUM_CARTE];
	int idx;
	giocatore_t player1;
	giocatore_t player2;
	int carte_scese[NUM_CARTE+1];
};
