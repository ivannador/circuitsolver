// Halozat.h függvényei
// Céljuk a villamos hálózatok modellezése

#include "Halozat.h"
#include <fstream>
#include <string>

using namespace std;

namespace Halozat {

	void Halozat::Felepit() {
		Lista lista;			// A hálózat elemeit és csomópontjait tároló lista
		string sor;
		unsigned cspszam, elemszam;
		unsigned cspCsat1 = 0, cspCsat2 = 0;	// Melyik csomópont (kezdõpont, végpont)
		double ertek1 = 0, ertek2 = 0;	// Az ismert értékek ebben tárolódnak ideiglenesen
		char elemNev[3];			// Elemmegadásnál ebben tároljuk az elem nevét, ami alapján eldönthetõ, hogy milyen típusú
		char mertE1, mertE2;		// Mértékegységek megállapításához a beolvasásnál (A=amper, V=volt, O=ohm)
		char valasz;

		fstream input("input.txt");
		if (!(input.is_open())) cout<<"Fajl hiba"<<endl;	// Majd exceptiont kell dobni!
		getline(input, sor);							// Lehet, hogy nem íródik felül!!!

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
		unsigned* cspElemSzam = new unsigned[cspszam];		// Tárolja az egyes csomópontokhoz csatlakozó elemek számát

		for(unsigned i=0;i<elemszam;i++) {
				// Egy sorból ki kell olvasni a csp-k számait és a megadott értékeket
			input >> elemNev >> cspCsat1 >> cspCsat2 >> ertek1 >> mertE1 >> ertek2 >> mertE2;
			lista.ElemLetrehoz(i, cspCsat1, cspCsat2, elemNev, ertek1, ertek2, mertE1, mertE2);
			cspElemSzam[cspCsat1-1]++;
			cspElemSzam[cspCsat2-1]++;
		}
		input >> elemNev >> valasz;		// Beolvassa a megadandó választ
		lista.ValaszLetrehoz(elemNev, valasz);	// Halozat-beli *Valasz a válasz változó mutatója

		for(unsigned i=0;i<cspszam;i++)
			lista.CspLetrehoz(cspElemSzam, i);


	}	// void Felepit()

	void Lista::ElemLetrehoz(unsigned hanyadik, unsigned cspCsat1, unsigned cspCsat2, char* elemNev, // Hanyadik: hányadik elemnél tart
		double ertek1, double ertek2, char mertE1, char mertE2) 
	{
		if(elemNev[0] == 'R') {		// Ha az elem ellenállás
			if(mertE1 == 'V') {
				if(mertE2 == 'O')
					ElemLista[hanyadik] = new Ellenallas(ertek1, ertek2, mertE1); // Ha feszültség és ellenállás van megadva
				else if(mertE2 == 'A') ElemLista[hanyadik] = new Ellenallas(ertek1, ertek2); // Ha feszültség és áram van megadva
				else ElemLista[hanyadik] = new Ellenallas(ertek1, mertE1);	// Ha csak feszültség van megadva
			} else 
			if(mertE1 == 'A') {
				if(mertE2 == 'O')
					ElemLista[hanyadik] = new Ellenallas(ertek1, ertek2, mertE1);	// Ha áram és ellenállás van megadva
				else ElemLista[hanyadik] = new Ellenallas(ertek1, mertE1);	// Ha csak áram van megadva
			}
			else ElemLista[hanyadik] = new Ellenallas(ertek1);	// Ha csak ellenállás van megadva
			ElemLista[hanyadik]->SetTipus(ELLENALLAS);
		}
		else if (elemNev[0] == 'U')	{	// Ha az elem feszültségforrás
			ElemLista[hanyadik] = new Feszultsegforras(ertek1);
			ElemLista[hanyadik]->SetTipus(FESZULTSEGFORRAS); 
		}
		else { ElemLista[hanyadik] = new Aramforras(ertek1);
			   ElemLista[hanyadik]->SetTipus(ARAMFORRAS);
		}

		if(CspLista[cspCsat1-1] != NULL) 		// Itt adja hozzá az elemet az adott csomóponthoz
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
		output << "A halozat valasza: " << *Valasz << endl;		// Még nem teljes!
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

	void Lista::HalozatMegoldo() {		// A KÖVETKEZÕ ALGORITMUSOK FÜGGVÉNYBE KELL KERÜLJENEK!!!
		unsigned maxElem = CspLista[0]->CsatElemSzam; unsigned hanyadik;	// Maximumkereséshez
		for(unsigned i=1;i<CsomopontSzam;i++)			// A legnagyobb csatlakozó elem-számú lesz a 0 potenciál
			if(CspLista[i]->CsatElemSzam > maxElem)
				maxElem = CspLista[hanyadik = i]->CsatElemSzam;
		CspLista[hanyadik]->SetPotencial(0);
		Elem** KozosElem = new Elem*[maxElem]; unsigned kozoselem = 0;			// Két csomópont közös elemeinek tárolására
		char** CsatElem_Nev = new char*[CspLista[hanyadik]->CsatElemSzam];		// Megállapítja, hogy melyik elemek csatlakoznak a 0-hoz
		for(unsigned i=0;i<CspLista[hanyadik]->CsatElemSzam;i++)	
			CsatElem_Nev[i] = CspLista[hanyadik]->Csat_Elem[i]->GetNev();

		for(unsigned i=0;i<CsomopontSzam;i++) {									// Összehasonlítjuk a többi csomóponttal
			for(unsigned j=0;j<CspLista[i]->CsatElemSzam;j++) { 				// Futtatja a vizsgálandó csomópont elemeit 
				for(unsigned k=0;k<CspLista[hanyadik]->CsatElemSzam;k++) 		// Futtatja a 0-csomópont elemeit
					if(CspLista[i]->Csat_Elem[j]->GetNev() == CsatElem_Nev[k]) {	// Ha a vizsgált egy eleme egyezik a 0-csp valamelyik elemével
						KozosElem[kozoselem++] = CspLista[i]->Csat_Elem[j];
						if(CspLista[i]->Csat_Elem[j]->GetFeszultseg() != 0)		// Ha annak az elemnek van megadott feszültsége
							CspLista[i]->SetPotencial(CspLista[i]->Csat_Elem[j]->GetFeszultseg());	// A vizsgált potenciálja ez a feszültség
					}
			}
			if(CspLista[i]->GetPotencial() != 0)
				for(unsigned j=0;j<kozoselem;j++) KozosElem[j]->SetFeszultseg(CspLista[i]->GetPotencial());	// Ekkor minden közös elem feszültsége
			kozoselem = 0;																					// a potenciál lesz
		}	

		for(unsigned i=0;i<CsomopontSzam;i++) {											// Végignézi, hogy melyik csomópontnak 
			if(CspLista[i]->GetPotencial() == 0 && CspLista[i] != CspLista[hanyadik]) {	// nem ismerjük még a potenciálját
				for(unsigned j=0;j<CsomopontSzam;j++) { 								// Futtatja a többi csomópontot
					if(CspLista[j] != CspLista[i] && CspLista[j] != CspLista[hanyadik] && CspLista[j]->GetPotencial() != 0)	// Ne legyen azonos, ne a 0, és ne ismeretlen pot.
						for(unsigned k=0;k<CspLista[i]->CsatElemSzam;k++) {				// Futtatja a vizsgált csp. elemeit
							for(unsigned l=0;l<CspLista[j]->CsatElemSzam;l++)			// Futtatja a másik csp. elemeit
								if(CspLista[i]->Csat_Elem[k]->GetNev() == CspLista[j]->Csat_Elem[l]->GetNev()) {
									KozosElem[kozoselem++] = CspLista[i]->Csat_Elem[k];	
									if(CspLista[i]->Csat_Elem[k]->GetFeszultseg() != 0)
										CspLista[i]->SetPotencial(CspLista[i]->GetPotencial() + CspLista[i]->Csat_Elem[k]->GetFeszultseg());
									// Ha a közös elem feszültsége ismert, a csp. potenciálja a másik csp. potenciálja és az elem feszültsége
								}
						}
				}
				kozoselem = 0;															
				if(CspLista[i]->GetPotencial() != 0)									// Újra vizsgálja, hogy a 0-val van-e közös eleme
					for(unsigned j=0;j<CspLista[i]->CsatElemSzam;j++) { 				// Futtatja a vizsgálandó csomópont elemeit 
						for(unsigned k=0;k<CspLista[hanyadik]->CsatElemSzam;k++) 		// Futtatja a 0-csomópont elemeit
							if(CspLista[i]->Csat_Elem[j]->GetNev() == CsatElem_Nev[k]) {// Ha a vizsgált egy eleme egyezik a 0-csp valamelyik elemével
								KozosElem[kozoselem++] = CspLista[i]->Csat_Elem[j];
						}
					}
					for(unsigned j=0;j<kozoselem;j++) KozosElem[j]->SetFeszultseg(CspLista[i]->GetPotencial());
					kozoselem = 0;														// Ha ismert pot. és van közös elem 0-val, az elemek fesz.-e a pot.
			}
		}
	}

}	// namespace Halozat;

		