#include "struttura_dati.h"
#include <gtk/gtk.h>
#include "manip_mano.h"
#include <iostream>
#include <cstring>

using namespace std;
/** Setta il campo sensitive della mano del giocatore, in base al giocatore.
 */ 
extern GtkBuilder *builder;

extern partita_t partita;
extern gioca_t gioca;

extern turno_t turno;


// idea, fai un case con 2 casi senza break che entrano negli altri.
void setta_mano(){
	if(gioca == giocatoreA){
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"carta1")),true);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"carta2")),true);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"carta3")),true);
		}
	else{
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"carta1")),false);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"carta2")),false);
		gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(builder,"carta3")),false);
		}
}

/** Funzione che ristabilisce il gioco:
  * libera il campo e carica le carte nella mano.
  */
static void refresh_mano(int i){
	
	
	if(turno == primo){
		switch(i){
			case 0: gtk_image_set_from_file(GTK_IMAGE
				(gtk_builder_get_object(builder, "0")),partita.player1.mano[i].nome);
				gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "0")), true);
				break;
			case 1: gtk_image_set_from_file(GTK_IMAGE
				(gtk_builder_get_object(builder, "1")),partita.player1.mano[i].nome);
				gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "1")), true);
				break;
			case 2: gtk_image_set_from_file(GTK_IMAGE
				(gtk_builder_get_object(builder, "2")),partita.player1.mano[i].nome);
				gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "2")), true);
				break;
			default: cout<<"ERRORE, IMPOSSIBILE!!"<<endl;
			}
		
	
		switch(i){
			case 0: gtk_image_set_from_file(GTK_IMAGE
				(gtk_builder_get_object(builder, "3")),partita.player2.mano[i].nome);
				gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "3")), true);
				break;
			case 1: gtk_image_set_from_file(GTK_IMAGE
				(gtk_builder_get_object(builder, "4")),partita.player2.mano[i].nome);
				gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "4")), true);
				break;
			case 2: gtk_image_set_from_file(GTK_IMAGE
				(gtk_builder_get_object(builder, "5")),partita.player2.mano[i].nome);
				gtk_widget_set_visible(GTK_WIDGET(gtk_builder_get_object(builder, "5")), true);
				break;
			default: cout<<"ERRORE, IMPOSSIBILE!!"<<endl;
			}
		}
	partita.idx++;
}


/** Funzione che genera la mano di gioco, caricando le carte da mazzo,
 *  alle rispettive mani.
 *  Funge anche da aggiornamento della mano durante il gioco.
 */
void carica_mano(){

	for(int i=0; i<3; i++){
		if(partita.player1.mano[i].selez == true){
			partita.player1.mano[i].val=partita.mazzo[partita.idx].val;
			strcpy(partita.player1.mano[i].nome, partita.mazzo[partita.idx].nome);
			partita.player1.mano[i].selez=partita.mazzo[partita.idx].selez;

			refresh_mano(i);
			
			partita.player1.mano[i].selez = false;
			}
		}

	for(int i=0; i<3; i++){
		if(partita.player2.mano[i].selez == true){
			partita.player2.mano[i].val=partita.mazzo[partita.idx].val;
			strcpy(partita.player2.mano[i].nome, partita.mazzo[partita.idx].nome);
			partita.player2.mano[i].selez=partita.mazzo[partita.idx].selez;
			

			refresh_mano(i);

			partita.player2.mano[i].selez = false;
			}
		}
}

