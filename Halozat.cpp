// Halozat.h f�ggv�nyei
// C�ljuk a villamos h�l�zatok modellez�se

#include "Halozat.h"
#include <fstream>
#include <string>

using namespace std;

namespace Halozat {

	void Halozat::Felepit() {
		Lista lista;			// A h�l�zat elemeit �s csom�pontjait t�rol� lista
		string sor;
		unsigned cspszam, elemszam;
		unsigned cspCsat1 = 0, cspCsat2 = 0;	// Melyik csom�pont (kezd�pont, v�gpont)
		double ertek1 = 0, ertek2 = 0;	// Az ismert �rt�kek ebben t�rol�dnak ideiglenesen
		char elemNev[3];			// Elemmegad�sn�l ebben t�roljuk az elem nev�t, ami alapj�n eld�nthet�, hogy milyen t�pus�
		char mertE1, mertE2;		// M�rt�kegys�gek meg�llap�t�s�hoz a beolvas�sn�l (A=amper, V=volt, O=ohm)
		char valasz;

		fstream input("input.txt");
		if (!(input.is_open())) cout<<"Fajl hiba"<<endl;	// Majd exceptiont kell dobni!
		getline(input, sor);							// Lehet, hogy nem �r�dik fel�l!!!

		if(sor.size() == 8) {
			cspszam = sor[7]-'0';
		} else { 
			cspszam = (sor[7]-'0') * 10 + (sor[8]-'0');
		}

		getline(input, sor);
		if(sor.size() == 9) {
			elemszam = sor[9]-'0';
		} else {
			elemszam = (sor[9]-'0') * 10 + (sor[10]-'0');
		}
		lista.SetCspSzam(cspszam);
		lista.SetElemSzam(elemszam);
		lista.AllocElemLista(elemszam);
		lista.AllocCspLista(cspszam);
		unsigned* cspElemSzam = new unsigned[cspszam];		// T�rolja az egyes csom�pontokhoz csatlakoz� elemek sz�m�t

		for(unsigned i=0;i<elemszam;i++) {
				// Egy sorb�l ki kell olvasni a csp-k sz�mait �s a megadott �rt�keket
			input >> elemNev >> cspCsat1 >> cspCsat2 >> ertek1 >> mertE1 >> ertek2 >> mertE2;
			lista.ElemLetrehoz(i, cspCsat1, cspCsat2, elemNev, ertek1, ertek2, mertE1, mertE2);
			cspElemSzam[cspCsat1-1]++;
			cspElemSzam[cspCsat2-1]++;
		}
		input >> elemNev >> valasz;		// Beolvassa a megadand� v�laszt
		lista.ValaszLetrehoz(elemNev, valasz);	// Halozat-beli *Valasz a v�lasz v�ltoz� mutat�ja

		for(unsigned i=0;i<cspszam;i++)
			lista.CspLetrehoz(cspElemSzam, i);


	}	// void Felepit()

	void Lista::ElemLetrehoz(unsigned hanyadik, unsigned cspCsat1, unsigned cspCsat2, char* elemNev, // Hanyadik: h�nyadik elemn�l tart
		double ertek1, double ertek2, char mertE1, char mertE2) 
	{
		if(elemNev[0] == 'R') {		// Ha az elem ellen�ll�s
			if(mertE1 == 'V') {
				if(mertE2 == 'O')
					ElemLista[hanyadik] = new Ellenallas(ertek1, ertek2, mertE1); // Ha fesz�lts�g �s ellen�ll�s van megadva
				else if(mertE2 == 'A') ElemLista[hanyadik] = new Ellenallas(ertek1, ertek2); // Ha fesz�lts�g �s �ram van megadva
				else ElemLista[hanyadik] = new Ellenallas(ertek1, mertE1);	// Ha csak fesz�lts�g van megadva
			} else 
			if(mertE1 == 'A') {
				if(mertE2 == 'O')
					ElemLista[hanyadik] = new Ellenallas(ertek1, ertek2, mertE1);	// Ha �ram �s ellen�ll�s van megadva
				else ElemLista[hanyadik] = new Ellenallas(ertek1, mertE1);	// Ha csak �ram van megadva
			}
			else ElemLista[hanyadik] = new Ellenallas(ertek1);	// Ha csak ellen�ll�s van megadva
			ElemLista[hanyadik]->SetTipus(ELLENALLAS);
		}
		else if (elemNev[0] == 'U')	{	// Ha az elem fesz�lts�gforr�s
			ElemLista[hanyadik] = new Feszultsegforras(ertek1);
			ElemLista[hanyadik]->SetTipus(FESZULTSEGFORRAS); 
		}
		else { ElemLista[hanyadik] = new Aramforras(ertek1);
			   ElemLista[hanyadik]->SetTipus(ARAMFORRAS);
		}

		if(CspLista[cspCsat1-1] != NULL) 		// Itt adja hozz� az elemet az adott csom�ponthoz
			CspLista[cspCsat1-1]->AddElem(*ElemLista[hanyadik]);
		
		if(CspLista[cspCsat2-1] != NULL) 
			CspLista[cspCsat2-1]->AddElem(*ElemLista[hanyadik]);	
		ElemLista[hanyadik]->SetNev(elemNev);
	}	// void ElemLetrehoz()

	void Lista::CspLetrehoz(unsigned* cspElemSzam, unsigned hanyadik) {
		CspLista[hanyadik] = new Csomopont(cspElemSzam[hanyadik]);
		CspLista[hanyadik]->AllocCsat_Elem();
	}	// void CspLetrehoz()

	void Halozat::ValaszKiir() {
		ofstream output("output.txt");
		output << "A halozat valasza: " << *Valasz << endl;		// M�g nem teljes!
	}	// void ValaszKiir()

	void Csomopont::AddElem(Elem& elem) {
		Csat_Elem[TempElem] = &elem;
	}	// void AddElem()

	void Lista::ValaszLetrehoz(char* elemNev, char valasz) {
		for(unsigned i=0;i<ElemSzam;i++) {
			if(ElemLista[i]->GetNev() == elemNev) {
				if(valasz == 'I')
					*Valasz = ElemLista[i]->Aram;
				else if(valasz == 'U')
					*Valasz = ElemLista[i]->Feszultseg;
				else if(valasz == 'R')
					*Valasz = ElemLista[i]->GetEllenallasRef();
			}
		}
	}	// void ValaszLetrehoz()

	/*void Lista::ElemOsszefuz() {
		for(unsigned i=0;i<CsomopontSzam;i++) {
			for(unsigned j=0;j<ElemSzam;j++) {
				CspLista[i]->Csat_Elem[j]->

	}	// void	ElemOsszefuz()*/

	void Lista::HalozatMegoldo() {		// A K�VETKEZ� ALGORITMUSOK F�GGV�NYBE KELL KER�LJENEK!!!
		unsigned maxElem = CspLista[0]->CsatElemSzam; unsigned hanyadik;	// Maximumkeres�shez
		for(unsigned i=1;i<CsomopontSzam;i++)			// A legnagyobb csatlakoz� elem-sz�m� lesz a 0 potenci�l
			if(CspLista[i]->CsatElemSzam > maxElem)
				maxElem = CspLista[hanyadik = i]->CsatElemSzam;
		CspLista[hanyadik]->SetPotencial(0);
		Elem** KozosElem = new Elem*[maxElem]; unsigned kozoselem = 0;			// K�t csom�pont k�z�s elemeinek t�rol�s�ra
		char** CsatElem_Nev = new char*[CspLista[hanyadik]->CsatElemSzam];		// Meg�llap�tja, hogy melyik elemek csatlakoznak a 0-hoz
		for(unsigned i=0;i<CspLista[hanyadik]->CsatElemSzam;i++)	
			CsatElem_Nev[i] = CspLista[hanyadik]->Csat_Elem[i]->GetNev();

		for(unsigned i=0;i<CsomopontSzam;i++) {									// �sszehasonl�tjuk a t�bbi csom�ponttal
			for(unsigned j=0;j<CspLista[i]->CsatElemSzam;j++) { 				// Futtatja a vizsg�land� csom�pont elemeit 
				for(unsigned k=0;k<CspLista[hanyadik]->CsatElemSzam;k++) 		// Futtatja a 0-csom�pont elemeit
					if(CspLista[i]->Csat_Elem[j]->GetNev() == CsatElem_Nev[k]) {	// Ha a vizsg�lt egy eleme egyezik a 0-csp valamelyik elem�vel
						KozosElem[kozoselem++] = CspLista[i]->Csat_Elem[j];
						if(CspLista[i]->Csat_Elem[j]->GetFeszultseg() != 0)		// Ha annak az elemnek van megadott fesz�lts�ge
							CspLista[i]->SetPotencial(CspLista[i]->Csat_Elem[j]->GetFeszultseg());	// A vizsg�lt potenci�lja ez a fesz�lts�g
					}
			}
			if(CspLista[i]->GetPotencial() != 0)
				for(unsigned j=0;j<kozoselem;j++) KozosElem[j]->SetFeszultseg(CspLista[i]->GetPotencial());	// Ekkor minden k�z�s elem fesz�lts�ge
			kozoselem = 0;																					// a potenci�l lesz
		}	

		for(unsigned i=0;i<CsomopontSzam;i++) {											// V�gign�zi, hogy melyik csom�pontnak 
			if(CspLista[i]->GetPotencial() == 0 && CspLista[i] != CspLista[hanyadik]) {	// nem ismerj�k m�g a potenci�lj�t
				for(unsigned j=0;j<CsomopontSzam;j++) { 								// Futtatja a t�bbi csom�pontot
					if(CspLista[j] != CspLista[i] && CspLista[j] != CspLista[hanyadik] && CspLista[j]->GetPotencial() != 0)	// Ne legyen azonos, ne a 0, �s ne ismeretlen pot.
						for(unsigned k=0;k<CspLista[i]->CsatElemSzam;k++) {				// Futtatja a vizsg�lt csp. elemeit
							for(unsigned l=0;l<CspLista[j]->CsatElemSzam;l++)			// Futtatja a m�sik csp. elemeit
								if(CspLista[i]->Csat_Elem[k]->GetNev() == CspLista[j]->Csat_Elem[l]->GetNev()) {
									KozosElem[kozoselem++] = CspLista[i]->Csat_Elem[k];	
									if(CspLista[i]->Csat_Elem[k]->GetFeszultseg() != 0)
										CspLista[i]->SetPotencial(CspLista[i]->GetPotencial() + CspLista[i]->Csat_Elem[k]->GetFeszultseg());
									// Ha a k�z�s elem fesz�lts�ge ismert, a csp. potenci�lja a m�sik csp. potenci�lja �s az elem fesz�lts�ge
								}
						}
				}
				kozoselem = 0;															
				if(CspLista[i]->GetPotencial() != 0)									// �jra vizsg�lja, hogy a 0-val van-e k�z�s eleme
					for(unsigned j=0;j<CspLista[i]->CsatElemSzam;j++) { 				// Futtatja a vizsg�land� csom�pont elemeit 
						for(unsigned k=0;k<CspLista[hanyadik]->CsatElemSzam;k++) 		// Futtatja a 0-csom�pont elemeit
							if(CspLista[i]->Csat_Elem[j]->GetNev() == CsatElem_Nev[k]) {// Ha a vizsg�lt egy eleme egyezik a 0-csp valamelyik elem�vel
								KozosElem[kozoselem++] = CspLista[i]->Csat_Elem[j];
						}
					}
					for(unsigned j=0;j<kozoselem;j++) KozosElem[j]->SetFeszultseg(CspLista[i]->GetPotencial());
					kozoselem = 0;														// Ha ismert pot. �s van k�z�s elem 0-val, az elemek fesz.-e a pot.
			}
		}
	}

}	// namespace Halozat;

		