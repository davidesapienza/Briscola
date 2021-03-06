#include <iostream>
#include "struttura_dati.h"
#include "manip_turno.h"
#include <stdlib.h>
#include <gtk/gtk.h>
#include <sstream>


using namespace std;

extern partita_t partita;
extern turno_t turno;
extern GtkBuilder *builder;
extern gioca_t gioca;

/** Struttura dati solo per questo blocco
 */

/** Enum che rappresenta la priorità di una carta :
 *  lisco --> le carte 2, 4, 5, 6, 7;
 *  punti --> le carte 8, 9, 10;
 *  briscola_p --> le carte briscola 2, 4, 5, 6, 7, 8, 9;
 *  carico --> 3, 1;
 *  briscola_a --> 10, 3, 1;
 */
enum prior_carta_gioco {niente, liscio, punti, briscola_p, carico, briscola_a}; 


static prior_carta_gioco carta0, carta1, carta2;

static int scelta;

/** Funzione che prende l'indice della mano del pc, quindi la carta,
 *  e gli attribuisce una determinata priorità.
 */
static prior_carta_gioco prendi_carta(int idx, bool plr){	//funzione valida anche per risp_carta
	//Prende la carta.
	int carta;
	if(plr == false)
		carta = partita.player2.mano[idx].val;
	else
		carta = partita.player1.mano[idx].val;

	int briscola = partita.mazzo[6].val;
	
	// Se è una briscola
	if(carta/10 == briscola/10){
		//devo valutare se è grande o piccola, posso utilizzare L'array PESI;
		//Piccola se PESI[carta%10] è minore di quella di 10(<=9)
		if(PESI[carta%10] <= PESI[8])
			return briscola_p;
		else
			return briscola_a;
		}
	//da qui sicuramente non è una briscola
	if(PESI[carta%10] == 0)
		return liscio;
	//Rimandogo punti e carico;
	if(PESI[carta%10] <= 4)
		return punti;
	//Altrimenti non restano che carichi
	return carico;
}

/** Funzione che gestisce la stessa priorità tra 2 carte. seguendo i criteri del tipo di carta.
 */
static int gest_stessa_prior(int i1, int i2){
	//deso sapere che carte sono
	prior_carta_gioco c1;//, c2;xk non la uso
	int c1_v, c2_v; 
/* ho solo 4 possibilità, per come ho scritto in prendi_carta_bassa */
	if(i1 == 0){
		c1=carta0;
		c1_v=0;
		}
	if(i1 == 1){
		c1=carta1;
		c1_v=1;
		}
	if(i2 == 1){
		//c2=carta1;
		c2_v=1;
		}
	if(i2 == 2){
		//c2=carta2;
		c2_v=2;
		}
	//adesso ho le  mie 2 carte c1 e c2

	//caso facile in cui sia una briscola_p o briscola_a : devo scegliere la più bassa in briscola_p e più alta in briscola_a
	if(c1 == briscola_p){   //ho messo solo c1, perchè tanto è = a c2
		//ritorna la più bassa delle 2
		if(partita.player2.mano[i1].val%10 < partita.player2.mano[i2].val%10)
			return c1_v;
		return c2_v;
		}
	if(c1 == briscola_a){
		//ritorna la più bassa delle 2 perchè ricordiamo che in realta sono 1, 3, 10;
		if(partita.player2.mano[i1].val%10 > partita.player2.mano[i2].val%10)
			return c1_v;
		return c2_v;
		}
	/* Il caso in cui sia liscio, punti, o carico, prende sempre quello di valore minore:
	 * perchè nel liscio e punti, vuol dire che gioca la carta più piccola, con minor peso
	 * mentre per il carico, gioca sì la carta minore, ma equivale all'asso, 
	 * che è quella con peso maggiore, così evito di verificare se strozza.
	 */	
	if(partita.player2.mano[i1].val%10 < partita.player2.mano[i2].val%10)
		return c1_v;
	return c2_v;
	}	
		
/** Funzione che di fatto prende la carta con priorità più alta, che poi è quella più bassa.
 */
static int prendi_carta_bassa(){
	//casi in cui ce ne sia una con priorità maggiore delle altre
	if(carta0 < carta1 && carta0 < carta2)
		return 0;
	if(carta1 < carta0 && carta1 < carta2)
		return 1;
	if(carta2 < carta0 && carta2 <carta1)
		return 2;
	//rimangono i casi in cui o hanno tutte la stessa prior, 
	// o cmq 2 hanno la stessa priorità
/*però non è così semplice : perchè se sono lisci o pochi punti, devo scegliere quella che ha il numero dei carichi scesi maggiore.
  se è briscola_p gioco la più piccola,
  se carico, gioco un 3 se l'asso di quel seme è già sceso o ce l'ho in mano, altrimenti l'asso.
  e per quanto riguarda la briscola alta, prendo sempre la più piccola
*/
	//prima caso in cui sono 3 perchè è più generale
	if(carta0 == carta1 && carta0 == carta2){
		int win;
		win = gest_stessa_prior(0, 1);
		
		if(win == carta0)
			win = gest_stessa_prior(0, 2);
		if(win == carta1)
			win = gest_stessa_prior(1, 2);
		return win;
		}
	if(carta0 == carta1)
		return gest_stessa_prior(0, 1);
	if(carta0 == carta2)
		return gest_stessa_prior(0, 2);
	//manca solo il caso in cui carta1 == carta2, evito di scrivere un if
	return gest_stessa_prior(1, 2);
  	
}	

/** Funzione che propone una carta, e il player1 dovrà rispondere.
 *  Ovvero, il pc gioca per primo, e deve scegliere la sua carta in base alla mano 
 *  che ha in quel momento, e alle carte già scese.
 */
static int proponi_carta(){
	//depende dalle carte che ha in mano, e dalle carte scese in precedenza.
	//diciamo che se riesce gioca liscio, altrimenti punticini, altrimenti briscola, altrimenti carico, e altrimenti briscole alte(1,3);
	/* prende la carta i, e gli attrubuisce una determinata proprietà
	 */
	carta0=prendi_carta(0, false);
	carta1=prendi_carta(1, false);
	carta2=prendi_carta(2, false);

	//Adesso ho le varie priorità
//Quindi o cerco sempre di giocare quella con priorità più bassa, o diventa molto complicato
//Tra due con priorità più bassa, devo guardare le carte che sono già uscite e giocare quella con più carichi fuori, compresi quelli che ho in mano.
	//va da 0 a 2;
	int vinc;
	vinc = prendi_carta_bassa();

	cout<<"IL PC HA GIOCATO "<<vinc<<endl;
	
	return vinc;
}

//FINE PROPONI CARTA

/** Funzione che restituisce true se c'è un carico, una briscola_p, o una briscola_a, o punti
 *  e setta scelta al valore più piccolo.
 */
static bool get_val_basso(prior_carta_gioco p, int v0, int v1, int v2, int va){

/* Devo distinguere però i vari casi...
   se punti va bene sia selezionare quelli con valore più basso, sia quelli con PESI più basso
   se briscola_p val più basso va bene, ma PESI no , perchè le briscole più piccole hanno peso 0
   se carico non va bene val più basso, xk l1 vale più del 3, come anche in Briscola_a..
*/ 
	scelta=-1;
	//se la carta0 è del tipo passato come parametro
	if(carta0 == p)
		scelta=0;
	//se la carta1 è del tipo passato come parametro
	if(carta1 == p){
		//e lo è anche carta0 contemporaneamente
		if(carta0 == p){
		//quando lo sono tutti e due
			//nel caso in cui sia punti o briscola_p
			if(p == punti || p == briscola_p){
				if(v0%10 < v1%10)
					scelta=0;
				else
					scelta=1;
				}
			//nel caso in cui sia carico o briscola_a
			if(p == carico || p == briscola_a){
				if(PESI[v0%10] < PESI[v1%10])
					scelta=0;
				else
					scelta=1;
				}
			}
		//lo è solo carta1(per adesso)
		else
			scelta=1;
		}
	//se carta2 è del tipo passato come parametro			
	if(carta2 == p){
		//e lo è anche carta0 contemporaneamente
		if(carta0 == p){
			//nel caso in cui sia punti o briscola_p
			if(p == punti || p == briscola_p){
				if(v0%10 < v2%10)
					scelta=0;
				else
					scelta=2;
				}
			//nel caso in cui sia carico o briscola_a
			if(p == carico || p == briscola_a){
				if(PESI[v0%10] < PESI[v2%10])
					scelta=0;
				else
					scelta=2;
				}
			}
		//e lo è anche carta1 contemporaneamente
		else if(carta1 == p){
			//nel caso in cui sia punti o briscola_p
			if(p == punti || p == briscola_p){
				if(v1%10 < v2%10)
					scelta=1;
				else
					scelta=2;
				}
			//nel caso in cui sia carico o briscola_a
			if(p == carico || p == briscola_a){
				if(PESI[v1%10] < PESI[v2%10])
					scelta=1;
				else
					scelta=2;
				}
			}
		//lo è solo carta2
		else
			scelta=2;
		}
	//caso in cui tutte e tre sono dello stesso tipo
	if(p == punti || p == briscola_p){
		if(carta0 == carta1 && carta0 == carta2){
			if(v0%10 < v1%10 && v0%10 < v2%10)
				scelta=0;
			else if(v1%10 < v0%10 && v1%10 < v2%10)
				scelta=1;
			else
				scelta=2;
			}
		}
	if(p == carico || p == briscola_a){
		if(carta0 == carta1 && carta0 == carta2){
			if(PESI[v0%10] < PESI[v1%10] && PESI[v0%10] < PESI[v2%10])
				scelta=0;
			else if(PESI[v1%10] < PESI[v0%10] && PESI[v1%10] < PESI[v2%10])
				scelta=1;
			else
				scelta=2;
			}
		}
	if(scelta != -1)
		return true;
	return false;
} 

/** Funzione strozza, che cerca di strozzare con un carico sulla carta giocata dall'avversario
 */
static bool prova_strozza(prior_carta_gioco p, int v0, int v1, int v2, int va){
	scelta=-1;
	
	if((carta0 == carico) && ((v0/10) == (va/10))){
		scelta=0;
		cout<<"sono qui!!! primo if \n";
		}

	//caso in cui lo sia carta1
	if((carta1 == carico) && ((v1/10) == (va/10))){
		cout<<"sono qui!!! secondo if\n";
		//mi dice se ha l'asso mentre carta0 ha il tre
		//caso in cui lo sia insieme a carta0
		if((carta0 == carico) && ((v0/10) == (va/10))){
			cout<<"sono in secondo if annidato!!!\n";
			if (v1 < v0){
				scelta = 1;
				cout<<"sono in secondo if riannidato!!!\n";
				}
			else
				scelta = 0;
			}
		else
			scelta = 1;
		cout<<scelta<<"!!!!!!!!!!!"<<endl;
		} 

	//caso in cui lo sia carta2
	if((carta2 == carico) && ((v2/10) == (va/10))){
		cout<<"sono in terzo if!!!\n";
		//insieme a carta0
		if((carta0 == carico) && ((v0/10) == (va/10))){
			cout<<"sono in terzo if!!! annidato\n";
			if (v2 < v0){
				cout<<"sono in terzo if!!! riannidato\n";
				scelta=2;
				}
			else
				scelta=0;
			}
		else{
			cout<<"sono in else!!!\n";
			//insieme a carta1
			if((carta1 == carico) && ((v1/10) ==  (va/10))){
				cout<<"sono in else!!! annidato\n";
				if (v2 < v1){
					scelta=2;
					cout<<"sono in else!!! riannidato\n";
					}
				else
					scelta=1;
				}
			else
				scelta=2;
			}
		}
	
	//adesso però non so ancora se scelta è uguale ad un carico, devo vederlo
	if(scelta != -1)
		return true;
	return false;
}
		


/** Funzione che segli di fatto la carta migliore da giocare per rispondere all'avversario.*/
static int prendi_carta_bassa2(int idx, prior_carta_gioco avv){
/* devo gestire tutti i casi possibili, le possibilità sono liscio, punti, briscola_p, carico, briscola_a.

partiamo con il liscio -->   se è liscio, cerco di strozzare, se ho carico dello stesso seme, altrimenti gioco liscio, ma facendo in modo di perdere(quindi stesso seme, ma val più basso, o altro seme), altrimenti prendo con punti, altrimenti prendo con liscio stesso seme ma val più alto, altrimenti cedo punti, altrimenti prendo con briscola_p, altrimenti prendo con briscola_a, altrimenti cedo carico. 

se gioca punti --> se riesco, devo prendere con il carico per strozzare, altrimenti gioco liscio fino al cavallo, e con il re prendo con la briscola_p se ce l'ho, altrimenti prendo con briscola_p se non ho liscio, altrimenti cedo pochi punti, altrimenti prendo con briscola_a, altrimenti cedo carico.

se gioca briscola_p --> se è briscola_p gioco liscio se riesco, altrimenti pochi punti, altrimenti briscola_p: però se la sua è briscola_p con peso 0, cerco di giocare briscola_p di val minore, altrimenti cerco di prendere sempre però con briscola_p di val maggiore, altrimenti prendo con briscola_a, altrimenti cedo carico 

se gioca briscola_a --> se re, gioco liscio, altrimenti pochi punti, altrimenti briscola_p, altrimenti prendo con briscola_a, altrimenti cedo carico
		    --> se tre, cerco di prendere con l'asso di briscola, altrimenti gioco liscio, altrimenti pochi punti, altrimenti briscola_p, altrimenti carico, altrimenti gioco il re di briscola

se gioca carico  --> cerco di strozzare se tre, e quindi guardo se ho l'asso, altrimenti cerco di prendere con briscola_p, altrimenti briscola_a, altrimenti liscio, altrimenti punti, altrimenti gioco carico o più piccolo o diverso




ci sarebbero poi tanti particolari: ovvero per ogni cosa, dovrei giocare, quando cerco di perdere, la carta con meno carichi usciti, così me la tolgo e evito di ritrovarmi una carta che gli faccia strozzare poi */
	int n;
	if(avv == liscio)
		n=0;
	else if(avv == punti)
		n=1;
	else if(avv == briscola_p)
		n=2;
	else if(avv == carico)
		n=3;
	else
		n=4;
	//potrei fare uno switch con la carta dell'avversario, quindi 0 se è liscio ecc.

	//mi prendo i valori delle carte della mano del pc per non dovermi sempre riscrivere tutto!!!
	int val_c0, val_c1, val_c2;
	val_c0=partita.player2.mano[0].val;
	val_c1=partita.player2.mano[1].val;
	val_c2=partita.player2.mano[2].val;

	//mi prendo anche il valore della carta dell'avversario (giocatore fisico)
	int val_avv;
	val_avv=partita.player1.mano[idx].val;

	cout<<"valore avv \n"<<val_avv<<endl;

	cout<<"val_c0 \n"<<val_c0<<endl;
	cout<<"val_c1 \n"<<val_c1<<endl;
	cout<<"val_c2 \n"<<val_c2<<endl;


	switch(n){
		//caso di liscio
		case 0: //devo cercare si strozzare, quindi devo vedere se ho un carico
			
			if(prova_strozza(carico, val_c0, val_c1, val_c2, val_avv)==true)
				return scelta;

			//non ci sono carichi dello stesso seme!
			//adesso devo vedere che siano lisci
			//per semplicità, cerco di giocare lisci di seme diverso, altrimenti lisci stesso seme, ma val piùbasso
/* in realtà dovrei giocare lisci con meno carichi scesi, e con val più alti possibile, sempre cercando di perdere, perchè dopo avrò più occasione gi giocare carte basse e fargli prendere per così a mia volta giocare per secondo, e avere più possibilità di giocare carico.
*/
			if(carta0 == liscio && val_c0/10 != val_avv/10)
				return 0;
			if(carta1 == liscio && val_c1/10 != val_avv/10)
				return 1;
			if(carta2 == liscio && val_c2/10 != val_avv/10)
				return 2;
			//adesso potrebbe essere liscio ma dello stesso seme, con val più basso
			if(carta0 == liscio && val_c0 < val_avv)
				return 0;
			if(carta1 == liscio && val_c1 < val_avv)
				return 1;
			if(carta2 == liscio && val_c2 < val_avv)
				return 2;

			//altrimenti provo a vedere se riescoa prendese con punti
			//passo alla funzione punti


/*			if(get_carta_per_val(val_c0, val_c1, val_c2, val_avv, punti) == true)
				return scelta;

*/
			scelta=-1;

			if(carta0 == punti && val_c0/10 == val_avv/10)
				scelta=0;
			if(carta1 == punti && val_c1/10 == val_avv/10){
				//devo vedere se anche carta0 == punti e prendere il maggiore
				if(carta0 == punti && val_c0/10 == val_avv/10){
					if (val_c1 > val_c0)
						scelta = 1;
					else
						scelta = 0;
					}
				else
					scelta = 1;
				} 
			if(carta2 == punti && val_c2/10 == val_avv/10){
				//devo vedere se anche carta0 == punti e prendere il maggiore
				if(carta0 == punti && val_c0/10 == val_avv/10){
					if (val_c2 > val_c0)
						scelta = 2;
					else
						scelta = 0;
					}
				else{
					//insieme a carta1
					if(carta1 == punti && val_c1/10 ==  val_avv/10){
						if (val_c2 > val_c1)
							scelta=2;
						else
							scelta=1;
						}
					else
						scelta=2;
					}
				}
			//adesso però non so ancora se scelta è uguale a punti, devo vederlo
			if(scelta!=-1)
				return scelta;

			//altrimenti cerco di prendere con liscio stesso seme ma con val più alto
			//qui ci arrivo solo se ho solo questo tipo di liscio
			//prior_carta_gioco scelta0, scelta1, scelta3;
			//per forza è dello stesso seme quindi evito di chiederlo, e per forza ha val più altro, quindi evito di chiederlo
			if(carta0 == liscio)
				return 0;
			if(carta1 == liscio)
				return 1;
			if(carta2 == liscio)
				return 2;
			//altrimenti cedo punti, e dato che se erano dello stesso seme li avrei già giocati, saranno di seme diverso e gioco quelli con val più basso
			//cambio metodo, vado dal caso più generale, a quello più specifico
			//da notare che non posso avere tutte e tre le carte a punti con seme diverso dal liscio, perchè o non è liscio ed è briscola, ho io ho una briscola_p che non è punti, avrei già dovuto escludere
			if(carta0 == punti && carta1 == punti){
				if(val_c0%10 < val_c1%10)
					return 0;
				else
					return 1;
				}
			if(carta0 == punti && carta2 == punti){
				if(val_c0%10 < partita.player1.mano[2].val%10)
					return 0;
				else
					return 2;
				}
			if(carta1 == punti && carta2 == punti){
				if(val_c1%10 < val_c2%10)
					return 1;
				else
					return 2;
				}
			//altrimenti al più ho solo una carta che è punti, e a sto punto la gioco
			if(carta0 == punti)
				return 0;
			if(carta1 == punti)
				return 1;
			if(carta2 == punti)
				return 2;
			
			//altrimenti prendo con la briscola_p se ce l'ho
			if(get_val_basso(briscola_p, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;
 			
			//adesso devo prendere con la briscola alta, e prendo con la più bassa
			if(get_val_basso(briscola_a, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;

			//altrimenti devo cedere carico, e per ade anche qui me ne frego
			if(get_val_basso(carico, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;
			//NON DOVREI MAI ARRIVARE FINO A QUI
			cout<<"ERRORE MANCA CASO!!! CASE0"<<endl;
			break;	

/*se gioca punti --> se riesco, devo prendere con il carico per strozzare, altrimenti gioco liscio fino al cavallo, e con il re prendo con la briscola_p se ce l'ho, altrimenti prendo con briscola_p se non ho liscio, altrimenti cedo pochi punti, altrimenti prendo con briscola_a, altrimenti cedo carico.*/
		//caso di punti
		case 1: //UGUALE AL CASO 0
			//devo cercare si strozzare, quindi devo vedere se ho un carico
			//if(carta0 == carico || carta1 == carico || cart2 == carico){
			scelta=-1;
			//caso in cui carta0 è carico, e dello stesso seme
			if(carta0 == carico && val_c0/10 == val_avv/10)
				scelta=0;

			//caso in cui lo sia carta1
			if(carta1 == carico && val_c1/10 == val_avv/10){
				//mi dice se ha l'asso mentre carta0 ha il tre
				//caso in cui lo sia insieme a carta0
				if(carta0 == carico && val_c0/10 == val_avv/10){
					if (val_c1 < val_c0)
						scelta = 1;
					else
						scelta = 0;
					}
				else
					scelta = 1;
				} 

			//caso in cui lo sia carta2
			if(carta2 == carico && val_c2/10 == val_avv/10){
				//insieme a carta0
				if(carta0 == carico && val_c0/10 == val_avv/10){
					if (val_c2 < val_c0)
						scelta=2;
					else
						scelta=0;
					}
				else{
					//insieme a carta1
					if(carta1 == carico && val_c1/10 ==  val_avv/10){
						if (val_c2 < val_c1)
							scelta=2;
						else
							scelta=1;
						}
					else
						scelta=2;
					}
				}
			//adesso però non so ancora se scelta è uguale ad un carico, devo vederlo
			if(scelta != -1)
				return scelta;
			//altrimenti devo giocare liscio se la sua carta è fino al cavallo
			//ve bè per semplicità, anche se è re, gioco liscio
			if(carta0 == liscio)
				return 0;
			if(carta1 == liscio)
				return 1;
			if(carta2 == liscio)
				return 2;
	
			//altrimenti provo a vedere se riescoa prendese con punti
			//RIUTILIZZO SCELTA: perchè a sto punto cerco di prendere con più punti possibile
			scelta = -1;
			if(carta0 == punti && val_c0/10 == val_avv/10)
				scelta=0;
			if(carta1 == punti && val_c1/10 == val_avv/10){
				//devo vedere se anche carta0 == punti e prendere il maggiore
				if(carta0 == punti && val_c0/10 == val_avv/10){
					if (val_c1 > val_c0)
						scelta = 1;
					else
						scelta = 0;
					}
				else
					scelta = 1;
				} 
			if(carta2 == punti && val_c2/10 == val_avv/10){
				//devo vedere se anche carta0 == punti e prendere il maggiore
				if(carta0 == punti && val_c0/10 == val_avv/10){
					if (val_c2 > val_c0)
						scelta = 2;
					else
						scelta = 0;
					}
				else{
					//insieme a carta1
					if(carta1 == punti && val_c1/10 ==  val_avv/10){
						if (val_c2 > val_c1)
							scelta = 2;
						else
							scelta = 1;
						}
					else
						scelta = 2;
					}
				}
			//adesso però non so ancora se scelta è uguale a punti, devo vederlo
			if(scelta != -1)
				return scelta;

			//altrimenti prendo con briscola_p più bassa
			if(get_val_basso(briscola_p, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;

			//altrimenti cedo meno punti possibili
			//RIUTILIZZO SCELTA
	
			if(carta0 == punti)
				scelta=0;
			if(carta1 == punti){
				//devo vedere se anche carta0 == punti e prendere il maggiore
				if(carta0 == punti){
					if (val_c1 > val_c0)
						scelta = 0;
					else
						scelta = 1;
					}
				else
					scelta = 1;
				} 
			if(carta2 == punti){
				//devo vedere se anche carta0 == punti e prendere il maggiore
				if(carta0 == punti){
					if (val_c2 > val_c0)
						scelta = 0;
					else
						scelta = 2;
					}
				else{
					//insieme a carta1
					if(carta1 == punti){
						if (val_c2 > val_c1)
							scelta=1;
						else
							scelta=2;
						}
					else
						scelta=2;
					}
				}
			//adesso però non so ancora se scelta è uguale a punti, devo vederlo
			if(scelta != -1)
				return scelta;
			
			//altrimenti briscola_a con val più basso
			if(get_val_basso(briscola_a, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;
			//altrimenti devo cedere carico, e restituisco quello con val più basso
			if(get_val_basso(carico, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;
			//NON DOVREI MAI ARRIVARE FINO A QUI
			cout<<"ERRORE MANCA CASO!!! CASE1"<<endl;
			break;
/*se gioca briscola_p --> se è briscola_p gioco liscio se riesco, altrimenti pochi punti, altrimenti briscola_p: però se la sua è briscola_p con peso 0, cerco di giocare briscola_p di val minore, altrimenti cerco di prendere sempre però con briscola_p di val maggiore, altrimenti prendo con briscola_a, altrimenti cedo carico */	
		//caso briscola_p
		case 2: //cerco di giocare liscio, qualsiasi esso sia
			if(carta0 == liscio)
				return 0;
			if(carta1 == liscio)
				return 1;
			if(carta2 == liscio)
				return 2;	
			
			//altrimenti cedo meno punti possibili
			if(get_val_basso(punti, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;
			
			//quindi adesso devo giocare briscola
			//mi comporto in 2 modi diversi se PESO == 0 o no
			//mi riprendo idx carta player1
			if(PESI[val_avv%10] == 0){
				//gioco una briscola più piccola se riesco altrimenti una più grande
				if(carta0 == briscola_p && val_c0 <  val_avv)
					return 0;
				if(carta1 == briscola_p && val_c1 <  val_avv)
					return 1;
				if(carta2 == briscola_p && val_c2 <  val_avv)
					return 2;	
				//altrimenti gioca una qualsiasi briscola con peso più alto
				if(carta0 == briscola_p)
					return 0;
				if(carta1 == briscola_p)
					return 1;
				if(carta2 == briscola_p)
					return 2;
			}
			else{
				//riutilizzo scelta, e valuto la briscola piccola maggiore
				scelta=-1;
				if(carta0 == briscola_p && val_c0 >  val_avv)
					scelta=0;
				if(carta1 == briscola_p && val_c1 >  val_avv){
					//devo vedere se anche carta0 == punti e prendere il maggiore
					if(carta0 == briscola_p && val_c0 >  val_avv){
						if (val_c1 > val_c0)
							scelta = 1;
						else
							scelta = 0;
						}
					else
						scelta = 1;
					} 
				if(carta2 == briscola_p && val_c2 >  val_avv){
					//devo vedere se anche carta0 == punti e prendere il maggiore
					if(carta0 == punti && val_c0 >  val_avv){
						if (val_c2 > val_c0)
							scelta = 2;
						else
							scelta = 0;
						}
					else{
						//insieme a carta1
						if(carta1 == briscola_p && val_c1 >  val_avv){
							if (val_c2 > val_c1)
								scelta = 2;
							else
								scelta = 1;
							}
						else
							scelta = 2;
						}
					}
				//adesso però non so ancora se scelta è uguale a punti, devo vederlo
				if(scelta != -1)
					return scelta;
				//altrimenti gioco la briscola che ho
				if(carta0 == briscola_p)
					return 0;
				if(carta1 == briscola_p)
					return 1;
				if(carta2 == briscola_p)
					return 2;
				}
			//altrimenti prendo con briscola_a
			if(get_val_basso(briscola_a, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;
			//altrimenti con carico
			if(get_val_basso(briscola_a, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;

			//NON DOVREI MAI ARRIVARE FINO A QUI
			cout<<"ERRORE MANCA CASO!!! CASE2"<<endl;
			break;
/*se gioca carico  --> cerco di strozzare se tre, e quindi guardo se ho l'asso, altrimenti cerco di prendere con briscola_p, altrimenti briscola_a, altrimenti liscio, altrimenti punti, altrimenti gioco carico o più piccolo o diverso
*/
		//caso carico
		case 3: //caso in cui gioca il tre di carico
			if(val_avv%10 == 2){
				if(carta0 == carico && val_c0%10 == 0)
					return 0;
				if(carta1 == carico && val_c1%10 == 0)
					return 1;
				if(carta2 == carico && val_c2%10 == 0)
					return 2;
				}
			//altrimenti briscola_p
			if(get_val_basso(briscola_p, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;
			//altrimenti briscola_a
			if(get_val_basso(briscola_a, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;
			//altrimenti liscio
			if(carta0 == liscio)
				return 0;
			if(carta1 == liscio)
				return 1;
			if(carta2 == liscio)
				return 2;
			//altrimenti punti
			//altrimenti cedo meno punti possibili
			//RIUTILIZZO SCELTA
			if(get_val_basso(briscola_p, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;
			//altrimenti gioco carico fregandomene se 3 o 1.
			if(get_val_basso(carico, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;
			//NON DOVREI MAI ARRIVARE FINO A QUI
			cout<<"ERRORE MANCA CASO!!! CASE3"<<endl;
			break;
			
/*se gioca briscola_a --> se re, gioco liscio, altrimenti pochi punti, altrimenti briscola_p, altrimenti prendo con briscola_a, altrimenti cedo carico
		    --> se tre, cerco di prendere con l'asso di briscola, altrimenti gioco liscio, altrimenti pochi punti, altrimenti briscola_p, altrimenti carico, altrimenti gioco il re di briscola*/
		//caso briscola_a
		case 4: //per semplicità cerco sempre di batterla poi guarderò se cambiarla
			if(carta0 == briscola_a && PESI[val_c0%10] > PESI[val_avv%10] )
				return 0;
			if(carta1 == briscola_a && PESI[val_c0%10] > PESI[val_avv%10] )
				return 1;
			if(carta2 == briscola_a && PESI[val_c0%10] > PESI[val_avv%10] )
				return 2;
			//altrimenti liscio
			if(carta0 == liscio)
				return 0;
			if(carta1 == liscio)
				return 1;
			if(carta2 == liscio)
				return 2;
			//altrimenti punti i meno possibili
			//RIUTILIZZO SCELTA
			if(get_val_basso(punti, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;
			//altrimenti gioco briscola_p ma quelle con PESI = 0
			if(get_val_basso(briscola_p, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;
			//altrimenti carico
			if(get_val_basso(carico, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;
			//altrimenti briscola_a
			if(get_val_basso(briscola_a, val_c0, val_c1, val_c2, val_avv) == true)
				return scelta;
			//NON DOVREI MAI ARRIVARE FINO A QUI
			cout<<"ERRORE MANCA CASO!!! CASE3"<<endl;
			break;
		//caso impossibile
		default : cout<<"ERRORE, IMPOSSIBILE\n";
		}
	
	cout<<"impossibile che arrivi qui, deve aver fatto un return prima, se no manca una condizione\n";
	return -1;
}

/** Funzione che risponde alla carta giocata.
 *  Ovvero, funzione per scegliere la carta in base a quella che ha giocato player1.
 */
static int rispondi_carta(){
	//Devo recuperare la carta giocata dall'avversario
	int val_c_pr1;
	val_c_pr1=ricerca_id_carta(true); //true per la carta player1

	//edesso mi serve sapere che carta è: liscio, briscola.. ecc.
	prior_carta_gioco carta_plr1;
	carta_plr1=prendi_carta(val_c_pr1, true);

	cout<<"IL GIOCATORE AVVERSARIO HA GIOCATO UN "<<carta_plr1<<endl;
	
/*Adesso ho la carta, so che cos'è, e arriva il difficile.
  se è liscio, cerco di strozzare, se ho carico dello stesso seme, altrimenti gioco liscio, ma facendo in modo di perdere(quindi stesso seme, ma val più basso, o altro seme), altrimenti prendo con punti, altrimenti prendo con liscio stesso seme ma val più alto, altrimenti cedo punti, altrimenti prendo con briscola_p, altrimenti prendo con briscola_a, altrimenti cedo carico. 
  se è briscola gioco liscio se riesco, altrimenti pochi punti, altrimenti briscola se la mia cmq è briscola_p, */
	//devo recuperare le tre carte del pc
	carta0 = prendi_carta(0, false);
	carta1 = prendi_carta(1, false);
	carta2 = prendi_carta(2, false);
	
	// le possibilità sono liscio, punti, briscola_p, carico, briscola_a.
	//le gestisco con prendi_carta_bassa2()
	int vinc;
	vinc = prendi_carta_bassa2(val_c_pr1, carta_plr1);
	cout<<"IL PC HA RISPOSTO "<<vinc<<endl;
	return vinc;
}

/** Funzione che sceglie di fatto la carta del pc da giocare.
 *  Ho due possibili casi : 
 *  1 --> il pc gioca per primo, quindi turno=primo;
 *  2 --> il pc gioca per secondo, quindi turno=secondo;
 */
static void scelta_carta(){
	int i;
	if(turno == primo)
		i=proponi_carta();
		
	else{
		i=rispondi_carta();
		}
	partita.player2.mano[i].selez=true;
}

/** Funzione che preme letteralmente la carta, come fosse un giocatore.
 *//*
static void premi_carta(){
	
}*/

/** Funzione che seleziona la carta da giocare :
 *  setto a selez=true la carta del player2(il mio pc) che voglio giocare.
 *  Ritorna l'indice della player2.mano corrispondente alla carte che voglio giocare.
 */
int selez_carta(){
	scelta_carta();	

	//premi_carta();
	/* Passo false per settre player2
	 */
	return ricerca_id_carta(false);
	}

void gioca_pc(){
	int i=selez_carta();
	

	
	sleep(1);
	ostringstream oss;
	oss<<i+3;
	//gtk_label_set_text(label,oss.str().c_str());

	//mentre setto la carta "premuta" dal pc a false
	gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder,oss.str().c_str())),false);
	oss.clear();
	
	
}
