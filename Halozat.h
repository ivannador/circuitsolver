#ifndef HALOZATELEM_H
#define HALOZATELEM_H

#include <iostream>
#include <exception>

using namespace std;

namespace Halozat {

	enum elem_t {ELLENALLAS, FESZULTSEGFORRAS, ARAMFORRAS};
	class Csomopont;	// Így az Elem látja a Csomopont osztályt

	class Halozat
	{
	protected:
		double* Valasz;
	public:
		Halozat();
		~Halozat(); // {delete[] ElemLista; delete[] CspLista;}
		void Felepit();		// A txt fájl alapján felépíti a hálózatot	
		void ValaszKiir();
	};

	class Elem : public Halozat
	{
	protected:
		double Feszultseg;
		double Aram;
		char* Nev;
		elem_t tipus;
		//Csomopont* Elozo;		// A csatlakozási paraméterek
		//Csomopont* Kovetkezo;	// Csomóponthoz csatlakozik az elem, az az "interfész" két elem között
	public:
		Elem() {Feszultseg = 0; Aram = 0;}
		Elem(double U, double I) {Feszultseg = U; Aram = I;}
		Elem(double E, const char Mertek) {if(Mertek == 'V') { Feszultseg = E; Aram = 0; } else { Aram = E; Feszultseg = 0;}}	// Még nem tuti!
		~Elem(){																		
			Feszultseg, Aram = 0;
			//delete Elozo; Elozo = NULL;
			//delete Kovetkezo; Elozo = NULL;
			delete[] Nev;
		}
		void SetNev(char* nev) {Nev = new char[3]; for(unsigned i=0;i<2;i++) Nev[i] = nev[i]; Nev[2] = '\0';} // Könnyebb beazonosítás céljából
		void SetAram(double I) {Aram = I;}
		void SetFeszultseg(double U) {Feszultseg = U;}
		void SetTipus(elem_t t) {tipus = t;}
		char* GetNev() {return Nev;}
		double GetAram() {return Aram;}
		double GetFeszultseg() {return Feszultseg;}
		//Csomopont* SetElozo(Csomopont* P) {Elozo = P;}
		//Csomopont* SetKovetkezo(Csomopont* N) {Kovetkezo = N;}
		//Csomopont* GetElozo() {return Elozo;}
		//Csomopont* GetKovetkezo() {return Kovetkezo;}
		virtual void print() {cout<<"Feszultseg: "<<Feszultseg<<"V "<<"Aram: "<<Aram<<"A"<<endl;}
		virtual double& GetEllenallasRef();			// A válaszhoz így lehet elérni az Ellenallas alosztály private változóját

		//void operator>>(Csomopont& kov) {this->Kovetkezo = &kov;}
		//void operator<<(Csomopont& elozo) {this->Elozo = &elozo;}
		friend class Lista;
	};

	class Ellenallas : public Elem
	{
		double Rez;
	public:
		Ellenallas(double U, double I):Elem(U, I) {Rez = U/I;}		
		Ellenallas(double E, double R, const char Mertek):Elem(E, Mertek) 
			{Rez = R; if(Mertek == 'A') SetAram(E/R); else SetFeszultseg(E*R);}	// A char az értékmegadás eldöntésére kell
		Ellenallas(double E, const char Mertek):Elem(E, Mertek) {}		// Eldönti, hogy áram, vagy feszültség lett-e megadva
		Ellenallas(double R) {Rez = R;}
		~Ellenallas() {Rez = 0;}
		void SetEllenallas(double R) {Rez = R;}
		double GetEllenallas() {return Rez;}
		double& GetEllenallasRef() {return Rez;}		
		void print() {Elem::print(); cout<<"Ellenallas: "<<Rez<<"Ohm"<<endl;}
	};

	class Aramforras : public Elem					// Lényegében nem különbözik az elem õsosztálytól, azonban egyértelmûbb így létrehozni az elemet
	{
	public:
		Aramforras(double I):Elem(I, 'A') {}
		void print() {Elem::print();}
	};

	class Feszultsegforras : public Elem
	{
	public:
		Feszultsegforras(double U):Elem(U, 'V'){}
		void print() {Elem::print();}
	};

	class Csomopont : public Halozat
	{
		double Potencial;
		unsigned CsatElemSzam;
		Elem** Csat_Elem;
		unsigned TempElem;		// Az eddig eltárolt elemek kézbentartására
	public:
		Csomopont(unsigned elemNum) {CsatElemSzam = elemNum; TempElem = 0; Potencial = 0;}
		~Csomopont() {delete[] Csat_Elem;}
		void AddElem(Elem& elem);
		void SetPotencial(double Pot) {Potencial = Pot;}
		double GetPotencial() {return Potencial;}
		void CalcPotencial();
		void AllocCsat_Elem() {Csat_Elem = new Elem*[CsatElemSzam];}
		friend class Lista;
	};


	class Lista : public Halozat
	{
		unsigned CsomopontSzam;
		unsigned ElemSzam;
		Elem** ElemLista;
		Csomopont** CspLista;
	public:
		~Lista() {delete[] ElemLista; delete[] CspLista;}
		void SetCspSzam(unsigned num) {CsomopontSzam = num;}
		void SetElemSzam(unsigned num) {ElemSzam = num;}
		unsigned GetCspSzam() {return CsomopontSzam;}
		unsigned GetElemSzam() {return ElemSzam;}
		void AllocElemLista(unsigned meret) {ElemLista = new Elem*[meret];}
		void AllocCspLista(unsigned meret) {CspLista = new Csomopont*[meret];}
		void CspLetrehoz(unsigned* cspElemSzam, unsigned hanyadik);		// Csomópontot hoz létre
		void ElemLetrehoz(unsigned hanyadik, unsigned cspCsat1, unsigned cspCsat2, char* elemNev, 
			double ertek1, double ertek2, char mertE1, char mertE2);
		// Adott elemet létrehozza és a tömbben tárolja
		void ValaszLetrehoz(char* elemNev, char valasz);		// A válasz elemhez kötése
		void ElemOsszefuz();	// Összefûzi az elemeket *Elozo és *Kovetkezo mutatóikkal
		void HalozatMegoldo();
	};

}

#endif


