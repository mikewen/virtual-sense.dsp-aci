/* 
 * File:   ACI_LCR.h
 * Author: piccio
 *
 * Created on 1 dicembre 2013, 19.04
 */

#ifndef LIBRERIAACI_LCR_H
#define	LIBRERIAACI_LCR_H

#define WND_LEN		512
#define FFT_LENGTH  WND_LEN
#define HWAFFT_SCALE	1
#define NUM_BINS	(FFT_LENGTH/2 + 1) 	// Useful freq. bins, there rest are symmetric

/************************************************************************/
/*     Funzioni estratte dalla libreria ACI usata in wavesurfer         */
/*      Servono per calcolare i 3 indici ACI :                          */
/*      1-      ACIt                                                    */
/*      2-      ACIf - (calcolato tramite la funzione ACIfAgg che rende */
/*                      i risultati confrontabili con ACIt      )       */
/*      3-      ACItAgg                                                 */
/*      Le variabili filtro Alto e filtro Basso servono per calcolare   */
/*      le tre versioni dell'indice ACI :                               */
/*      1- ACI - valori spettrogramma maggiori del filtro basso         */
/*      2- ACI forefield - valori spettrogramma compresi tra filtro     */
/*                       basso e filtro alto                            */
/*      3- ACI farfield - valori spettrogramma maggiori del filto alto  */
/************************************************************************/


//Funzione ACI per tempo
double *ACI(double **spect,  // matrice spettrogramma --> spect[tInt][frEnd - fStart]
	int tInt, //lunghezza intervallo di tempo
	int frStart,//class di frequenza di inizio
	int frEnd,//class di frequenza di fine
	int filtroAlto, //filtro alto (vengono presi valori spettrogrammi minori di filtro Alto)
	int filtroBasso);//filtro basso (vengono presi valori spettrogrammi maggiori di filtro Alto)

//Funzione ACI per frequenza
double *ACIf(double **spect,  // matrice spettrogramma --> spect[tInt][frEnd - fStart]
	int tInt, //lunghezza intervallo di tempo
	int frStart,//class di frequenza di inizio
	int frEnd,//class di frequenza di fine
	int filtroAlto, //filtro alto (vengono presi valori spettrogrammi minori di filtro Alto)
	int filtroBasso);//filtro basso (vengono presi valori spettrogrammi maggiori di filtro Alto)

//Calcolo Aci su matrice aggregata
//dovrebbe essere uguale ad aci per tempo con tInt differente ( tIntAgg = tInt \ aggrega ) 
double *ACItAgg(double **spect,  // matrice spettrogramma --> spect[tInt][frEnd - fStart]
	int tInt, //lunghezza intervallo di tempo
	int frStart,//class di frequenza di inizio
	int frEnd,//class di frequenza di fine
	int aggrega, //fattore di aggregazione - viene passato alla funzione aggSpect
	int filtroAlto, //filtro alto (vengono presi valori spettrogrammi minori di filtro Alto)
	int filtroBasso);//filtro basso (vengono presi valori spettrogrammi maggiori di filtro Alto)


//Calcolo aci per frequenze aggregato

/********************************************************************************/
/*	Rispetto a ACItAgg, che è indice come ACI e ACIf, la funzione ACIfAgg	*/
/*      serve a rendere confrontabile l'indice ACIf con l'indice ACIt         */
/********************************************************************************/

double *ACIfAgg(double **spect,  // matrice spettrogramma --> spect[tInt][frEnd - fStart]
	int tInt, //lunghezza intervallo di tempo
	int frStart,//class di frequenza di inizio
	int frEnd,//class di frequenza di fine
	int filtroAlto, //filtro alto (vengono presi valori spettrogrammi minori di filtro Alto)
	int filtroBasso,//filtro basso (vengono presi valori spettrogrammi maggiori di filtro Alto)
	int fAgg);



//Calcola spettrogramma aggregato per AciAgg
double **aggSpect(double **spect, //spettrogramma  spect[tInt][nClassi]
	int aggregatore, //fattore di aggregazione
	int tInt, //lunghezza intervallo di tempo
	int nClassi);//classi di frequenza

#endif	/* LIBRERIAACI_LCR_H */

