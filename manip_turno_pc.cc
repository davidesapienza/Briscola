#include "struttura_dati.h"
#include <gtk/gtk.h>
#include "manip_mano.h"
#include <iostream>
#include "turno_pc.h"




using namespace std;

extern GtkBuilder *builder;
extern partita_t partita;
extern gioca_t gioca;
extern turno_t turno;




/** Funzione che restituisce il valore intero dell'indice della mano, corrispondente
  * alla carta premuta. Ritorna un numero compreso tra 0 e 2.
  * per la mano del player1, det_bool = true, per quella di player2 è false.
  */
int ricerca_id_carta(bool set_mano){
	/* Cerca in player1.mano
	 */
	int id;
	if(set_mano == true){
		for(int i=0; i<NUM_MANO; i++){
			if(partita.player1.mano[i].selez == true)
				id = i;
			}
		}
	/* Cerca in player2.mano
	 */
	else{
		for(int i=0; i<NUM_MANO; i++){
			if(partita.player2.mano[i].selez == true)
				id = i;
			}
		}
	return id;
	}

/** Funzione che memorizza le carte giocate, tiene il conto di quelle già scese
 */
void segna_carta(){
	int id;
	int idx;
	idx=partita.carte_scese[0];
	id=ricerca_id_carta(true);
	partita.carte_scese[idx]=partita.player1.mano[id].val;
	partita.carte_scese[0]++;
	idx=partita.carte_scese[0];
	id=ricerca_id_carta(false);
	partita.carte_scese[idx]=partita.player2.mano[id].val;
	partita.carte_scese[0]++;
}

/** Funzione che somma al punteggio precedente del giocatore vincente,
 *  i pesi delle nuove carte.
 */
static void calcola_punteggio(int vinc, int v1, int v2){
	if(vinc == giocatoreA)
		partita.player1.punteggio=partita.player1.punteggio+PESI[v1%10]+PESI[v2%10];
	if(vinc == giocatoreB)
		partita.player2.punteggio=partita.player2.punteggio+PESI[v1%10]+PESI[v2%10];	
}


/** Funzione che gestisce chi ha vinto, quindi recupera le due carte premute
 *  (saranno quelle con stato selez a true), le confronta tra loro, 
 *  dando la precedenza a quella giocata per prima, poi confrontandole con la 
 *  briscola, e settando il giocatore che ha vinto la mano.
 */
static void gestore_vittoria(){
	/* Ricordandoci che in passa_turno, abbiamo già cambiato il giocatore, 
	 * quindi alla fine mi ritrovo che adesso gioca è già settato al giocatore
 	 * che ha giocato per primo.
	 */
	
	/* Le variabili val, mi rappresentano il numero della carta(0-39).
	 * val1 --> il num. della carta di chi ha giocato per primo.
	 * val2 --> il num. della carta di chi ha giocato per secondo.
	 */
	int val1, val2;

	/* Indici della carta che avrà campo selez a true, ovvero quella giocata.
	 */
	int idx_mia_mano;
	int idx_avv_mano;
	
	/* Chiamo ricerca_id_carta per ottenere l'indice della carta premuta:
	 * true per mano giocatore1, e false per mano del giocatore2.
	 */
	idx_mia_mano = ricerca_id_carta(true);
	idx_avv_mano = ricerca_id_carta(false);
	cout<<"idx_mio= "<<idx_mia_mano<<"\nidx_avv= "<<idx_avv_mano<<endl;

	/* Se giocatoreA ha giocato per primo.
	 */
	if(gioca == giocatoreA){
		val1=partita.player1.mano[idx_mia_mano].val;
		val2=partita.player2.mano[idx_avv_mano].val;
		}
	/* Se giocatoreB ha giocato per primo.
	 */
	else{
		val1=partita.player2.mano[idx_avv_mano].val;
		val2=partita.player1.mano[idx_mia_mano].val;
		}
	
	cout<<"val1 = "<<val1<<"\nval2 = "<<val2<<endl;

	/* Adesso di default setto le priorità first alla carta giocata per prima, 
	 * e second a quella giocata per seconda.
	 */
	prior_carta_t priorc1 = first;
	prior_carta_t priorc2 = second;

	/* la briscola è l'intero della divisione del mazzo[6].
	 */
	int briscola = (partita.mazzo[6].val/10)+1;
	
	// valuto ora se una delle 2 è briscola, o entrambe
	(val1/10)+1 == briscola ? priorc1=brisc : priorc1=first;
	
	(val2/10)+1 == briscola ? priorc2=brisc : priorc2=second;
	
	//valuto se hanno lo stesso seme
	(val1/10)+1 == (val2/10)+1 ? priorc2=priorc1 : priorc2=priorc2;
	
	// adesso devo gestire il fatto della vittoria
	int vinc;
	if(priorc1 == priorc2){
		//deve confrontare la precedenza in bade al vettore PREC
		int valor1=PREC[val1%10];
		int valor2=PREC[val2%10];
		
		if(gioca == giocatoreA && valor1 < valor2)
			vinc=0;
		else if(gioca == giocatoreB && valor1 < valor2)
			vinc=1;
		else if(gioca == giocatoreA && valor1 > valor2){
			gioca=giocatoreB;
			vinc=1;
			}
		else if(gioca == giocatoreB && valor1 > valor2){
			gioca=giocatoreA;
			vinc=0;
			}

		}	

	else if(priorc1 < priorc2){
		gioca == giocatoreA ? vinc=0 : vinc=1;
		cout<<"vince stesso giocatore\n";
		}

	else{
		gioca == giocatoreA ? gioca=giocatoreB : gioca=giocatoreA;
		gioca == giocatoreA ? vinc=0 : vinc=1;
		cout<<"vince altro giocatore\n";
		}
	
	cout<<"il vincitore è "<<vinc<<endl;
	calcola_punteggio(vinc, val1, val2);
	cout<<"punteggio giocA = "<<partita.player1.punteggio<<endl;
	cout<<"punteggio giocB = "<<partita.player2.punteggio<<endl;	
}

/** Funzione che pulisce il campo di gioco, le due immagini giocate scompaiono
 */
void pulisci_tavolo(){
	/* devo pulire il campo digioco
	*/
	
	gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder,"image2")),false);
	gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder,"image3")),false);
	gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder,"image4")),false);
	gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder,"image5")),false);
	
	}


/** Funzione che congela lo stato del gioco quando entrambi i giocatori hanno giocato,
 *  e devono poi pescare prima di rigiocare.
 */
static void congela_gioco(){
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"carta1")),false);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"carta2")),false);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"carta3")),false);

	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"mazzo")),true);
	}
static void fine_turno(){
	/* Congela lo stato del gioco, non è possibile premere
	 * nessun bottone. Abilita il bottone del mazzo
	 */
	congela_gioco();
		
	/* Gestisci chi ha vinto la mano, con tutte le conseguenze, e quindi setta
	 * il turno nel modo appropriato.
	 */
	gestore_vittoria();
		
	/* turno risettato a primo per rigiocare una mano
	 */
	turno = primo;
	
	//fine_mano();
	sleep(1);
		
	
	segna_carta();
	/*for(int i=1; i<partita.carte_scese[0]; i++){
		cout<<partita.carte_scese[i]<<endl;
		}
	*/
	
}


void setta_immagine_tavolo(){
	
	int indice;
	if(gioca == giocatoreA)
		indice=ricerca_id_carta(true);
	else
		indice=ricerca_id_carta(false);
	
	GtkImage *image;
	if(turno == primo){
		if(gioca==giocatoreA){
			// Prende il rif all'immagine sul tavolo.
			image = GTK_IMAGE(gtk_builder_get_object(builder,"image4"));
			// Setta image 3 con l'immagine giusta del bottone premuto.
			gtk_image_set_from_file(image,partita.player1.mano[indice].nome);
			}
		else{
			// Prende il rif all'immagine sul tavolo.
			image = GTK_IMAGE(gtk_builder_get_object(builder,"image2"));
			// Setta image 3 con l'immagine giusta del bottone premuto.
			gtk_image_set_from_file(image,partita.player2.mano[indice].nome);
			}
		}
	else{
		if(gioca==giocatoreA){
			// Prende il rif all'immagine sul tavolo.
			image = GTK_IMAGE(gtk_builder_get_object(builder,"image3"));
			// Setta image 3 con l'immagine giusta del bottone premuto.
			gtk_image_set_from_file(image,partita.player1.mano[indice].nome);	
			}
		else{
			// Prende il rif all'immagine sul tavolo.
			image = GTK_IMAGE(gtk_builder_get_object(builder,"image5"));
			// Setta image 3 con l'immagine giusta del bottone premuto.
			gtk_image_set_from_file(image,partita.player2.mano[indice].nome);
			}
		}
	// Setta tale immagine a visible.
	gtk_widget_set_visible(GTK_WIDGET(image),true);	
	
	
	
	
}

/** Funzione che che gestisce la fine del turno.
 *  I casi sono due :
 *  1 --> ha giocato solo un giocatore, e deve ancora giocare l'altro.
 *  2 --> hanno giocato entrambi i giocatori, e quindi deve gestire la nuova
	  coppia di turni.
 */
void passa_turno(){

	/* Cambia giocatore:
	 * da fare ogni volta che si cambia il turno.
	 * Dato che lo faccio all'inizio della funzione, quando sono nel caso 2,
	 * me lo devo ricordare.
	 */ 
	//gioca == giocatoreA? gioca=giocatoreB : gioca=giocatoreA;
/* devo gestire i vari casi. per comodità, faccio in modo che quando gioca il giocatore fisico, giochi automaticamente anche il pc.
però:
quando il giocatore fisico propone, cambia turno e chiama la funzione selez_carta. e quindi seleziona lui la carta giusta tra le tre sue.
dopo deve gestire la vittoria, e settare il giocatore giusto per ripartire. in più deve settare di nuovo il turno a secondo.

ma in questo caso, se è il pc a dover giocare di nuovo, deve rischiamare la funzione selez_ carta, e settare poi il turno a secondo e settare giocatore A.
	
	
ma in questo caso poi ci sarà da gestire il turno secondo, e quindi gestisci vittoria di nuovo.
*/

	//quando il turno è il primo	
	if(turno == primo){

		if(gioca == giocatoreA){
			gioca=giocatoreB;
			turno = secondo;
			cout<<" 1GIOCATORE "<<gioca<<" e TURNO "<<turno<<endl;
			
			gioca_pc();
			setta_immagine_tavolo();
			gioca=giocatoreA;
			fine_turno();
			cout<<" 1GIOCATORE "<<gioca<<" e TURNO "<<turno<<endl;
			}	
		/*else{	
			
			turno=secondo;
			gioca_pc();
			setta_immagine_tavolo();
			gioca=giocatoreA;
			cout<<" 2GIOCATORE "<<gioca<<" e TURNO "<<turno<<endl;	
			*/
			/* Ogni volta che il turno cambia, risetta la sensibilità delle carte del giocatore 
		 	 * che deve giocare.
		 	 */ 
		/*	setta_mano();
		}*/


		
	}
	//quando il turno è il secondo, so già che deve giocare il giocatorea
	else{
		gioca=giocatoreB;
		fine_turno();
		cout<<" 3GIOCATORE "<<gioca<<" e TURNO "<<turno<<endl;
	}
	
}


