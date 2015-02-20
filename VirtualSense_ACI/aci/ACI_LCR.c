#include <stdlib.h>     /* calloc, free */

//#include "ACI_LCR.h"

double **aggSpect(double **spect, int aggregatore, int tInt, int nClassi) {
	int i,j,agg;

    double **spectAggr = (double **) calloc((int) (tInt / aggregatore), sizeof (double *));


    for (i = 0; i < (int) (tInt / aggregatore); i = i++) {

        spectAggr[i] = (double *) calloc(nClassi, sizeof (double));

        for (j = 0; j < nClassi; j++) {
            double sum = 0;

            for (agg = 0; agg < aggregatore; agg++) {//ciclo aggregante
                sum += spect[(i * aggregatore) + agg][j];
            }
            spectAggr[i][j] = (double) (sum / aggregatore);
        }

    }

    return spectAggr;
}

double *ACI(double **spect, int tInt, int frStart, int frEnd, int filtroBasso, int filtroAlto) {

    double *ikArr, //array ik
            *dArr, //array somme differenze
            **dkArr, //array 2D dk
            *aci;

    int frInt = frEnd - frStart;
    int i,j;

    //alloco array somme intensita ik
    ikArr = (double *) calloc(frInt, sizeof (double));
    //alloco array dk delle differenze
    dkArr = (double **) calloc(tInt, sizeof (double *)); //[i]

    for (i = 0; i < tInt; i++) {
        dkArr[i] = (double *) calloc(frInt, sizeof (double)); //[j]
    }
    //inizializzo
    for (j = 0; j < frInt; j++) {
        ikArr[j] = 0;
    }

    for (j = 0;
            j < frInt;
            j++) {
        for (i = 0;
                i < tInt;
                i++) {
            double ik = spect[i][j + frStart]; //c
            double ikp = spect[i + 1][j + frStart]; //b

            if (ik > filtroBasso && ik < filtroAlto) {
                ikArr[j] = ikArr[j] + ik;
            }
            //filtro
            if (ik > filtroBasso && ikp > filtroBasso && ik < filtroAlto && ikp < filtroAlto) {
                dkArr[i][j] = abs(ik - ikp);
            } else {
                dkArr[i][j] = 0;
            }

        }

        /*double ikfix = spect [tInt][j];
        if(ikfix > filtroBasso && ikfix < filtroAlto ){
                ikArr[j] = ikArr[j] + ikfix;	
        }*/
    }


    //inizializzo array dk e e aci_risultati
    dArr = (double *) calloc(frInt, sizeof (double));
    aci = (double *) calloc(frInt, sizeof (double));

    //calcolo array somme differenze
    for (j = 0; j < frInt; j++) {

        for (i = 0; i < tInt; i++) {
            dArr[j] = dArr[j] + dkArr[i][j];
        }
        //ck divisore
        if (ikArr[j] != 0) {

            double div = (double) dArr[j] / (double) ikArr[j];
            aci[j] = div;
            //fprintf(baciF,"%f\t", baci[nn][j]);
            //fprintf(surf,"%d\t%d\t%f\n",nn, nY, baci[nn][j]); //formato surfer (x y valAci)
        } else {
            aci[j] = 0;
            /*fprintf(baciF,"%f\t ",0);
            fprintf(surf,"%d\t%d\t%d\n",nn, nY, 0);				*/
        }
    }
    for (i = 0; i < tInt; i++) {
        free(dkArr[i]);
    }
    free(ikArr);
    free(dArr);
    free(dkArr);
    return aci;

}

double *ACIf(double **spect, int tInt, int frStart,int frEnd, int filtroBasso, int filtroAlto){		

		double *ikArr, //array ik
			*dArr,//array somme differenze
			**dkArr,//array 2D dk
			*aci;
		
		int frInt = frEnd - frStart;

		int i,j;

		//alloco array somme intensita ik
		ikArr = (double *) calloc(tInt ,sizeof(double));
		//alloco array dk delle differenze
		dkArr = (double **) calloc(tInt ,sizeof(double *));//[i]

		for(i = 0; i < tInt; i++){
			dkArr[i] = (double *) calloc(frInt, sizeof(double));//[j]
		}
		//inizializzo
		for(i = 0; i < tInt ; i++){
				ikArr[i] = 0;
		}

		for(i = 0;
					i < tInt;
					i++){

						for(j = 0;
							j < frInt;
							j++){
								double ik = spect[i][j + frStart];//c
								double ikp = spect[i][j + frStart + 1];//b
								
								if(ik > filtroBasso && ik < filtroAlto ){
									ikArr[i] = ikArr[i] + ik;	
								}
								//filtro
								if(ik > filtroBasso && ikp > filtroBasso && ik < filtroAlto && ikp < filtroAlto){								
									dkArr[i][j] = abs(ik - ikp);							
								} else {
									dkArr[i][j] =  0;
								}
						}
						//add ultima classe frequenza della selezione
						double ikfix = spect [i][frInt + frStart];
						if(ikfix > filtroBasso && ikfix < filtroAlto ){
							ikArr[i] = ikArr[i] + ikfix;	
						}
		}

		
		//inizializzo array dk e e aci_risultati
		dArr = (double *)calloc(tInt, sizeof(double));			
		aci = (double *) calloc(tInt ,sizeof(double));

		//calcolo array somme differenze
		for(i = 0; i < tInt; i++ ){

			for(j = 0; j < frInt; j++){
				dArr[i] = dArr[i] + dkArr[i][j];
			}

			//ck divisore
			if(ikArr[i] != 0){					
				double div = (double) dArr[i] / (double) ikArr[i];
				aci[i] = div;
			}
			else{
				aci[i] = 0;
			}
		}		
		for(i = 0; i < tInt; i++){
			free(dkArr[i]);
		}
		free(ikArr);
		free(dArr);
		free(dkArr);
		return aci;

}

double *ACItAgg(double **spect, int tInt, int frStart, int frEnd, int aggrega, int filtroAlto, int filtroBasso) {

    int aggregatore,
            tIntAgg;
    double *aciAgg,
            **spectAgg,
            *ikArrAgg, //array ik
            *dArrAgg, //array somme differenze
            **dkArrAgg; //array 2D dk


    if (aggrega > 1) aggregatore = aggrega;
    else aggregatore = 1;
    int frInt = frEnd - frStart;
    int i,j;
    spectAgg = aggSpect(spect, aggregatore, tInt, frInt);

    tIntAgg = (int) tInt / aggregatore;

    ikArrAgg = (double *) calloc(frInt, sizeof (double));

    dkArrAgg = (double **) calloc(tIntAgg, sizeof (double *)); //[i]

    for ( i = 0; i < tIntAgg; i++) {
        dkArrAgg[i] = (double *) calloc(frInt, sizeof (double)); //[j]
    }

    //inizializzo array ik pow e freq
    for ( j = 0;
            j < frInt;
            j++) {
        ikArrAgg[j] = 0;
    }


    //gestione indice orizzontale array da 0 a n
    //calcolo matrice intensita
    for ( j = 0;
            j < frInt;
            j++) {
        for ( i = 0;
                i < tIntAgg - 1;
                i++) {
            int ik = spectAgg[i][j + frStart]; //c
            int ikp = spectAgg[i + 1][j + frStart]; //b

            //filtro
            if (ik > filtroBasso && ik < filtroAlto) {

                ikArrAgg[j] = ikArrAgg[j] + ik;

            }
            //filtro
            if (ik > filtroBasso && ikp > filtroBasso && ik < filtroAlto && ikp < filtroAlto) {

                dkArrAgg[i][j] = abs(ik - ikp);

            } else {
                dkArrAgg[i][j] = 0;
            }
        }
    }

    //inizializzo array dk e e baci_risultati
    dArrAgg = (double *) calloc(frInt, sizeof (double));
    aciAgg = (double *) calloc(frInt, sizeof (double));

    //calcolo array somme differenze
    for ( j = 0; j < frInt; j++) {
        //inizializziazione

        for ( i = 0; i < tIntAgg; i++) {
            dArrAgg[j] = dArrAgg[j] + dkArrAgg[i][j];
        }
        //ck divisore
        if (ikArrAgg[j] != 0) {
            double div = (double) dArrAgg[j] / (double) ikArrAgg[j];
            aciAgg[j] = div;
        } else {
            aciAgg[j] = 0;

        }
    }
    return aciAgg;
}



double *ACIfAgg(double **spect, int tInt, int frStart, int frEnd,int filtroBasso, int filtroAlto, int fAgg){
	
	int frInt = frEnd - frStart;

	double *acif = (double *) calloc((int) (frInt / fAgg),sizeof(double));
	double *acif_calc = (double *) calloc(tInt,sizeof(double));
	int i,j;
	
	if (fAgg > 1){
			for( i = 0 ; i < (int) (frInt / fAgg); i++){

				acif_calc = ACIf(spect, tInt, frStart + (i*fAgg), frStart + ((i*fAgg) + fAgg - 1), filtroBasso, filtroAlto);

				double sum = 0;
				for ( j = 0; j < tInt; j++){
					sum += acif_calc[j];
				}
				acif[i] = sum;
			}
		}
		else{
			acif_calc = ACIf(spect, tInt,  frStart, frEnd,  filtroBasso, filtroAlto);

			double sum = 0;
			for ( j = 0; j < tInt; j++){
				sum += acif_calc[j];
			}
			acif[0]=sum;
		}
		
free(acif_calc);

		return acif;
		
}
