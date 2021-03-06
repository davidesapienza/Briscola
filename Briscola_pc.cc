// Progetto: Briscola
// compile with:
// g++ -export-dynamic Briscola.cc `pkg-config --cflags --libs gtk+-3.0`

#include <iostream>
#include <gtk/gtk.h>
#include <cmath>
#include <ctime>
#include <stdlib.h>
#include <fstream>
#include <cstring>

#include "struttura_dati.h"
#include "manip_mano.h"
#include "manip_turno.h"
#include "inizial_gioco.h"
#include "turno_pc.h"

using namespace std;


partita_t partita;


GtkBuilder *builder;

//crea un ogg di tipo enum gioca_t e lo setta per default come giocatoreA
//per default decido che partirò sempre io a giocare
gioca_t gioca = giocatoreA;


// per dire chi è primo a giocare e chi secondo.
turno_t turno = primo;




/** Inizializza la partita: 
 *  l'indice a zero
 *  valore di ogni carta a zero
 *  nome di ogni carta a NULL.
 */
void inizializza_partita()
{
  	partita.idx = 0;
	
  	for(int i=0; i<40; i++){
		partita.mazzo[i].val=0;
		//partita.mazzo[i].nome='\0';
		partita.mazzo[i].selez=false;
		partita.mazzo[i].esiste=false;
	}
	//numero carte giocate =0;
	//punta alla posizione 1;
	partita.carte_scese[0]=1;
	
	
	//genera il mazzo
	genera_mazzo();
	
	/* Quasto for è indispensabile per la prima volta, perchè carica_mano 
	 * è una funzione singola, per la prima volta che bisogna caricare la mano,
 	 * e per l'aggiornamento della mano.
	 */
	for(int i=0; i<3; i++){
		partita.player2.mano[i].selez = true;
		partita.player1.mano[i].selez = true;
		}
	
		
	//carica la mano di gioco con le carte
	carica_mano();

	//IMPORTANTE
	// Faccio idx++ perchè in questo momento punta alla carta del mazzo che però è la mia
	// briscola, quindi non posso pescarla, e la salto perchè deve rimanere fissa
	partita.idx++;
	
	//adesso setto le carte che posso premere e quelle che non posso premere
	setta_mano();

}



/** Chiude la finestra del gioco.
  */
extern "C" gboolean handler_delete_event (GtkWidget *widget,GdkEvent  *event, gpointer   user_data){
	gtk_main_quit();
	}


/** Quando viene cliccato un bottone di una carta, deve 
  * gestire che giocatore ha premuto la carta stessa, e se questo è il primo 
  * o il secondo.
  */
extern "C" void carta_button_clicked(GtkButton *button, gpointer data){
	
	// Prende il nome della label, che per me rappresenta l'indice del vettore 
	// della mia mano di gioco.	
	const char *num_lab = gtk_label_get_text(GTK_LABEL(data));
	
	// Testo della label convertito ad intero.
	int indice=atoi(num_lab);

	gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder,num_lab)), false);	
	partita.player1.mano[indice].selez=true;
	
	setta_immagine_tavolo();
	
	// Cambia turno
	
	passa_turno();
	return ;
		
}

/** Funzione che gestisce il click del bottone mazzo.
 *  Il mazzo viene premuto solo alla fine del gioco di una mano, ovvero 
 *  alla fine di entrambi i turni.
 *  Permette di pescare una carta.
 */
extern "C" void mazzo_button_clicked(GtkButton *button, gpointer data){
	/* Ricarica la mano solo alla fine di entrambi i turno, con le carte nuove.
	 */
	carica_mano();

	pulisci_tavolo();
	
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"mazzo")),false);
	
	if(gioca == giocatoreB){
		gioca_pc();
		setta_immagine_tavolo();
		gioca=giocatoreA;		
		turno = secondo;
		}
	/* Ogni volta che il turno cambia, risetta la sensibilità delle carte del giocatore 
	 * che deve giocare.
	 */ 
	setta_mano();	
	
}


int main(int argc, char *argv[]) {

	

	/* Call this function before using any other GTK+ 
	 * functions in your GUI applications. It will initialize 
	 * everything needed to operate the toolkit and parses 
	 * some standard command line options.

	 * Although you are expected to pass the argc, argv 
	 * parameters from main() to this function, it is possible 
	 * to pass NULL if argv is not available or commandline 	
	 * handling is not required.

	 * argc and argv are adjusted accordingly so your own 
	 * code will never see those standard arguments.
	 */
	gtk_init(&argc, &argv);

	//Creazione di un nuovo oggetto GtkBuilder
	builder = gtk_builder_new();

	//Caricamento dell'interfaccia nel nostro programma
	gtk_builder_add_from_file(builder,"BRISC.glade",NULL);

        //connessione di tutti i segnali
   	gtk_builder_connect_signals(builder, NULL);

	//INIZIO:

	
	
	inizializza_partita();
      	
	
	
	
	//aggancio le immagini delle carte
	//PARTE PROVVISORIA
	//gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"3")),partita.player2.mano[0].nome);
	//gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"carta4")),false);

	//gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"4")),partita.player2.mano[1].nome);
	//gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"carta5")),false);
	
	//gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"5")),partita.player2.mano[2].nome);
	//gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"carta6")),false);
	
	/*gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"0")),partita.player1.mano[0].nome);
	
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"1")),partita.player1.mano[1].nome);
		
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"2")),partita.player1mano[2].nome);*/
	
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"mazz")),"./Carte/retro.jpg");
		
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"image1")),partita.mazzo[6].nome);
	/*mostra_messaggi("Per iniziare una nuova partita: File -> Nuova partita");*/


	/*PARTE DA CAMBIARE CON QUELLA PROVVISORIA
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"image_c4")),"./Carte/retro.jpg");
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"image_c4")),false);

	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"image_c5")),"./Carte/retro.jpg");
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"image_c5")),false);
		
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"image_c6")),"./Carte/retro.jpg");
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"image_c6")),false);
	
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"image_c1")),partita.mazzo[3].nome);
		
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"image_c2")),partita.mazzo[4].nome);
		
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"image_c3")),partita.mazzo[5].nome);
	
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"mazz")),"./Carte/retro.jpg");
		
	gtk_image_set_from_file(GTK_IMAGE(gtk_builder_get_object(builder,"image1")),partita.mazzo[6].nome);
*/
	/* impedisco di pescare la carta
	 */ 
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"mazzo")),false);

	//Partenza del ciclo di gestione degli eventi
    	gtk_main();
	
	
	return 0;
}
