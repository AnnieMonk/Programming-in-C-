#include <iostream>
#include <exception>
#include<vector>
#include<string>
#include<thread>
#include<mutex>

using namespace std;

const char *crt = "\n-------------------------------------------\n";

class Datum {
	int *_dan, *_mjesec, *_godina;
public:
	Datum(int dan = 1, int mjesec = 1, int godina = 2000) {
		_dan = new int(dan);
		_mjesec = new int(mjesec);
		_godina = new int(godina);
	}
	Datum(const Datum &dat) {
		_dan = new int(*dat._dan);
		_mjesec = new int(*dat._mjesec);
		_godina = new int(*dat._godina);
	}

	//Nema operator=

	Datum& operator=(const Datum& obj)
	{
		if (this != &obj)
		{
			*_dan = *obj._dan;
			*_mjesec = *obj._mjesec;
			*_godina = *obj._godina;

		}

		return *this;
	}


	~Datum() {
		delete _dan; _dan = nullptr;
		delete _mjesec; _mjesec = nullptr;
		delete _godina; _godina = nullptr;
	}
	friend ostream &operator<<(ostream &COUT, const Datum &obj) {
		COUT << *obj._dan << " " << *obj._mjesec << " " << *obj._godina;
		return COUT;
	}

};
class Izuzetak : public exception {
	string _funkcija;
public:
	Izuzetak(const char* poruka, string funkcija) : exception(poruka), _funkcija(funkcija)
	{

	}

	const char* what() const
	{
		string error = "Greska: " + string(exception::what()) + "\nFunkcija: " + _funkcija;

		char* temp = new char[error.size() + 1];
		strcpy_s(temp, error.size() + 1, error.c_str());

		return temp;
	}
};

template <class T1, class T2, int max>
class Kolekcija {
	T1 *_elementi1[max];
	T2 *_elementi2[max];
	int _trenutnoElemenata;
	bool _omoguciDupliranjeElemenata;
public:
	Kolekcija(bool omoguciDupliranjeElemenata = false) {
		_trenutnoElemenata = 0;
		_omoguciDupliranjeElemenata = omoguciDupliranjeElemenata;

		for (size_t i = 0; i < max; i++)
		{
			_elementi1[i] = nullptr;
			_elementi2[i] = nullptr;
		}
	}

	Kolekcija(const Kolekcija& obj) : _omoguciDupliranjeElemenata(obj._omoguciDupliranjeElemenata)
	{
		_trenutnoElemenata = obj._trenutnoElemenata;
		for (size_t i = 0; i < obj._trenutnoElemenata; i++) {
			_elementi1[i] = new T1(*obj._elementi1[i]);
			_elementi2[i] = new T2(*obj._elementi2[i]);
		}
	}

	Kolekcija& operator=(const Kolekcija& obj)
	{
		if (this != &obj)
		{
			for (size_t i = 0; i < _trenutnoElemenata; i++)
			{
				delete _elementi1[i];
				_elementi1[i] = nullptr;
				delete _elementi2[i];
				_elementi2[i] = nullptr;
			}
			_omoguciDupliranjeElemenata = obj._omoguciDupliranjeElemenata;
			_trenutnoElemenata = obj._trenutnoElemenata;
			for (size_t i = 0; i < obj._trenutnoElemenata; i++) {
				_elementi1[i] = new T1(*obj._elementi1[i]);
				_elementi2[i] = new T2(*obj._elementi2[i]);
			}
		}

		return *this;
	}

	~Kolekcija() {
		for (size_t i = 0; i < _trenutnoElemenata; i++) {
			delete _elementi1[i]; _elementi1[i] = nullptr;
			delete _elementi2[i]; _elementi2[i] = nullptr;
		}
		_trenutnoElemenata = 0;
	}

	int GetTrenutno() const { return _trenutnoElemenata; }
	int GetMax() const { return max; }

	T1& GetElement1(int lokacija) const
	{
		if (lokacija < 0 || lokacija >= _trenutnoElemenata)
			throw Izuzetak("Nepostojeca lokacija", __FUNCTION__);
		return *_elementi1[lokacija];
	}

	T2& GetElement2(int lokacija) const
	{
		if (lokacija < 0 || lokacija >= _trenutnoElemenata)
			throw Izuzetak("Nepostojeca lokacija", __FUNCTION__);
		return *_elementi2[lokacija];
	}

	friend ostream &operator<<(ostream &COUT, const Kolekcija &obj)
	{
		for (size_t i = 0; i < obj.GetTrenutno(); i++)
			COUT << obj.GetElement1(i) << " " << obj.GetElement2(i) << endl;

		return COUT;
	}

	bool AddElement(T1 ele1, T2 ele2)//poziv kons. kopije za T1 i T2
	{
		if (_trenutnoElemenata == max)
			return false;

		if (!_omoguciDupliranjeElemenata)
			for (size_t i = 0; i < _trenutnoElemenata; i++)
				if (ele1 == *_elementi1[i] && ele2 == *_elementi2[i])
					return false;

		_elementi1[_trenutnoElemenata] = new T1(ele1);
		_elementi2[_trenutnoElemenata] = new T2(ele2);

		_trenutnoElemenata++;

		return true;
	}

	void RemoveByIndex(int index)
	{
		if (index < 0 || index >= _trenutnoElemenata)
			throw Izuzetak("Nepostojeca lokacija", __FUNCTION__);

		delete _elementi1[index];
		delete _elementi2[index];

		for (size_t i = index; i < _trenutnoElemenata - 1; i++)
		{
			_elementi1[i] = _elementi1[i + 1];
			_elementi2[i] = _elementi2[i + 1];

		}
		_elementi1[_trenutnoElemenata - 1] = nullptr;
		_elementi2[_trenutnoElemenata - 1] = nullptr;
		_trenutnoElemenata--;

	}

	void RemoveElement(const T1& el)
	{
		for (size_t i = 0; i < _trenutnoElemenata; i++)
		{
			if (*_elementi1[i] == el)
			{
				RemoveByIndex(i);

				if (_omoguciDupliranjeElemenata)
					i--;
				else
					return;
			}
		}
	}
};

class Dogadjaj
{
	Datum _datumOdrzavanja;
	Kolekcija<string, bool, 20> _obaveze; //cuva informaciju o obavezama koje je potrebno ispuniti prije samog dogadjaja, string se odnosi na opis, a bool na izvrsenje te obaveze (da li je zavrsena ili ne)

	char *_naziv;
	int _notificirajPrije; //oznacava broj dana prije samog dogadjaja kada ce krenuti notifikacija/podsjetnik
	bool _rekurzivnaNotifikacija; //ako je vrijednost true onda se korisnik notificira svaki dan do _datumaOdrzavanja dogadjaja, a pocevsi prije dogadjaja za _brojDanaZaNotifikaciju

public:
	Dogadjaj(Datum datumOdrzavanja, const char *naziv, int brojDana = 1,
		bool rekurzivnaNotifikacija = false) : _datumOdrzavanja(datumOdrzavanja)
	{
		_naziv = new char[strlen(naziv) + 1];
		strcpy_s(_naziv, strlen(naziv) + 1, naziv);

		_notificirajPrije = brojDana;
		_rekurzivnaNotifikacija = rekurzivnaNotifikacija;
	}

	Dogadjaj(const Dogadjaj &obj) : _datumOdrzavanja(obj._datumOdrzavanja)
	{
		_naziv = new char[strlen(obj._naziv) + 1];
		strcpy_s(_naziv, strlen(obj._naziv) + 1, obj._naziv);

		_notificirajPrije = obj._notificirajPrije;
		_rekurzivnaNotifikacija = obj._rekurzivnaNotifikacija;
		_obaveze = obj._obaveze;//Poziv operatora= za Kolekciju
	}

	~Dogadjaj()
	{
		delete[] _naziv;
		_naziv = nullptr;
	}
	char *GetNaziv() { return _naziv; }

	bool AddObavezu(string obaveza)
	{
		return _obaveze.AddElement(obaveza, false);
	}

	bool OznaciObavezuKaoZavrsenu(string nazivObaveze)
	{
		for (size_t i = 0; i < _obaveze.GetTrenutno(); i++)
		{
			if (_obaveze.GetElement1(i) == nazivObaveze && _obaveze.GetElement2(i) == false)
			{
				_obaveze.GetElement2(i) = true;
				return true;
			}
		}

		return false;
	}

	friend bool operator==(const Dogadjaj&, const Dogadjaj&);
};

bool operator==(const Dogadjaj& d1, const Dogadjaj& d2)
{
	return strcmp(d1._naziv, d2._naziv) == 0;
}

class Student
{
	int _indeks;
	string _imePrezime;
	vector<Dogadjaj> _dogadjaji;//_dogadjaji = obj._dogadjaji;
public:

	Student(int indeks, string imePrezime) : _indeks(indeks), _imePrezime(imePrezime) {}

	bool OznaciObavezuKaoZavrsenu(string nazivDogadjaja, string nazivObaveze)
	{
		for (size_t i = 0; i < _dogadjaji.size(); i++)
		{
			if (_dogadjaji[i].GetNaziv() == nazivDogadjaja)
			{
				return _dogadjaji[i].OznaciObavezuKaoZavrsenu(nazivObaveze);
			}
		}

		return false;
	}

	int GetIndeks() const { return _indeks; }

	vector<Dogadjaj>& GetDogadjaji() { return _dogadjaji; }

	bool AddDogadjaj(const Dogadjaj& d)
	{
		for (size_t i = 0; i < _dogadjaji.size(); i++)
		{
			if (_dogadjaji[i] == d)
				return false;
		}

		_dogadjaji.push_back(d);//koristi kons. kopije

		return true;
	}
	friend bool operator==(const Student&, const Student&);
	friend bool operator==(const Student&, int);


	friend ostream &operator<<(ostream &COUT, const Student &obj)
	{
		COUT << obj._imePrezime << " (" << obj._indeks << ")" << endl;
		return COUT;
	}
};

bool operator==(const Student& s1, const Student& s2)
{
	return s1._indeks == s2._indeks;
}

bool operator==(const Student& s1, int brojIndeksa)
{
	return s1._indeks == brojIndeksa;
}

mutex cuvarResursa;

class DLWMSReminder
{
	vector<Student> _reminiderList;
	void StartReminder(const Datum& datum, int& brojac)
	{
		string notifikacija;
		for (size_t i = 0; i < _reminiderList.size(); i++)
		{
			//notifikacija = _reminiderList[i].GetNotifikacije(datum, brojac);

			if (notifikacija != "")
			{
				cuvarResursa.lock();
				cout << notifikacija << endl;
				cuvarResursa.unlock();
			}
		}
	}
public:
	void AddStudent(const Student& s)
	{
		vector<Student>::iterator it = _reminiderList.begin();
		while (it != _reminiderList.end())
		{
			if (*it == s)
				throw Izuzetak("Student vec postoji.", __FUNCTION__);

			it++;
		}

		_reminiderList.push_back(s);
	}

	bool OznaciObavezuKaoZavrsenu(int brojIndeksa, string nazivDogadjaj, string nazivObaveze)
	{
		for (size_t i = 0; i < _reminiderList.size(); i++)
		{
			if (_reminiderList[i] == brojIndeksa)
				return _reminiderList[i].OznaciObavezuKaoZavrsenu(nazivDogadjaj, nazivObaveze);
		}

		return false;
	}

	int PosaljiNotifikacije(const Datum& datum)
	{
		int brojac = 0;
		thread t(&DLWMSReminder::StartReminder, this, datum, ref(brojac));
		t.join();//detach i sa future<int> async

		return brojac;
	}
};

void main() {

	/**************************************************************************
	1. SVE KLASE TREBAJU POSJEDOVATI ADEKVATAN DESTRUKTOR
	2. NAMJERNO IZOSTAVLJANJE KOMPLETNIH I/ILI POJEDINIH DIJELOVA DESTRUKTORA KOJI UZROKUJU RUNTIME ERROR CE BITI OZNACENO KAO "RE"
	3. SPASAVAJTE PROJEKAT KAKO BI SE SPRIJECILO GUBLJENJE URADJENOG ZADATKA
	4. PROGRAMSKI CODE SE TAKODJER NALAZI U FAJLU CODE_ParcijalniII.TXT
	5. NAZIVI FUNKCIJA MORAJU BITI IDENTI�NI ONIMA KOJI SU KORI�TENI U TESTNOM CODE-U.OSTALE, POMO�NE FUNKCIJE MO�ETE IMENOVATI PO �ELJI.
	****************************************************************************/

#pragma region Datum

	Datum danas(28, 1, 2018), sutra(29, 1, 2018);
	Datum datumIspitaPRIII(30, 1, 2018), datumIspitBPII(31, 1, 2018);
	Datum prekosutra(danas);//kons. kopije
	prekosutra = danas; //operator = 
	cout << danas << endl
		<< sutra << endl
		<< prekosutra << crt;

#pragma endregion

#pragma region Kolekcija

	/*
	AddElement :: omogucava dodavanje novog elementa u kolekciju. Ukoliko je moguce, osigurati automatsko prosiranje kolekcije prilikom dodavanja svakog novog elementa, te onemoguciti ponavljanje elemenata
	RemoveElement :: na osnovu parametra tipa T1 uklanja elemente iz kolekcije i ukoliko je moguce smanjuje velicinu niza/kolekcije. Prilikom uklanjanja elemenata ocuvati redoslijed njihovog dodavanja
	*/
	Kolekcija<int, float, 10> kolekcija1;
	for (size_t i = 0; i < kolekcija1.GetMax(); i++)
		if (!kolekcija1.AddElement(i, i + (0.6 * i)))
			cout << "Elementi " << i << " i " << i + (0.6 * i) << " nisu dodati u kolekciju" << endl;

	cout << kolekcija1.GetElement1(0) << " " << kolekcija1.GetElement2(0) << endl;
	cout << kolekcija1 << endl;

	kolekcija1.RemoveElement(1);

	Kolekcija<int, float, 10> kolekcija2;
	kolekcija2 = kolekcija1;//operator=
	cout << kolekcija2 << crt;

	if (kolekcija1.GetTrenutno() == kolekcija2.GetTrenutno())
		cout << "ISTI BROJ ELEMENATA" << endl;

	Kolekcija<int, float, 10> kolekcija3(kolekcija2);//kons. kopije
	cout << kolekcija3 << crt;

#pragma endregion

#pragma region Dogadjaj

	Dogadjaj ispitPRIII(datumIspitaPRIII, "Ispit iz PRIII", 5, true),
		ispitBPII(datumIspitBPII, "Ispit iz BPII", 7, true);
	///*po vlasitom izboru definisati listu zabranjenih rijeci koje ce onemoguciti dodavanje odredjene obaveze. Prilikom provjere koristiti regex*/
	if (ispitPRIII.AddObavezu("Preraditi pdf materijale"))cout << "Obaveza dodana!" << endl;
	////onemoguciti dupliranje obaveza
	if (!ispitPRIII.AddObavezu("Preraditi pdf materijale"))cout << "Obaveza nije dodana!" << endl;
	if (ispitPRIII.AddObavezu("Pregledati video materijale"))cout << "Obaveza dodana!" << endl;
	if (ispitPRIII.AddObavezu("Preraditi ispitne zadatke"))cout << "Obaveza dodana!" << endl;
	if (ispitPRIII.AddObavezu("Samostalno vjezbati"))cout << "Obaveza dodana!" << endl;

	if (ispitBPII.AddObavezu("Preraditi knjigu SQL za 24 h"))cout << "Obaveza dodana!" << endl;
	if (ispitBPII.AddObavezu("Pregledati video materijale"))cout << "Obaveza dodana!" << endl;
	if (ispitBPII.AddObavezu("Napraviti bazu za konkretnu aplikaciju"))cout << "Obaveza dodana!" << endl;

	Student jasmin(150051, "Jasmin Azemovic"), adel(160061, "Adel Handzic");

	if (jasmin.AddDogadjaj(ispitPRIII) && jasmin.AddDogadjaj(ispitBPII))
		cout << "Dogadjaj uspjesno dodan!" << endl;

	if (adel.AddDogadjaj(ispitPRIII) && adel.AddDogadjaj(ispitBPII))
		cout << "Dogadjaj uspjesno dodan!" << endl;
	////onemoguciti dupliranje dogadjaja
	if (!adel.AddDogadjaj(ispitPRIII))
		cout << "Dogadjaj nije uspjesno dodan!" << endl;

	DLWMSReminder reminder;

	try
	{
		reminder.AddStudent(jasmin);
		reminder.AddStudent(adel);
		//u slucaju dupliranja studenata funkcija baca izuzetak tipa Izuzetak
		reminder.AddStudent(jasmin);
	}
	catch (exception &err)
	{
		//ispisati sve informacije o nastalom izuzetku
		const char* error = err.what();
		cout << "\nException: " << error << endl;

		delete[] error; //Nije najbolje rjesenje.
	}

	////da bi bila oznacena kao zavrsena, obaveza mora postojati i mora biti oznacena kao nezavrsena (false)
	if (reminder.OznaciObavezuKaoZavrsenu(150051, "Ispit iz PRIII", "Pregledati video materijale"))
		cout << "Obaveza oznacena kao zavrsena" << endl;

	//jasmin = adel; //RE - ne impl. operator= u klasi Student

	/*metodi PosaljiNotifikacije se salje trenutni datum na osnovu cega ona pretrazuje sve studente koje treba podsjetiti/notoficirati o dogadjajima koji se priblizavaju.
	Koristeci multithread-ing, svim studentima se salju notifikacije sa sljedecim sadrzajem:
	-------------------------------------------------------------------------
	Postovani Jasmin Azemovic,
	Dogadjaj Ispit iz PRIII je zakazan za 3 dana, a do sada ste obavili 56% obaveza vezanih za ovaj dogadjaj. Neispunjene obaveze su:
	1.Preraditi ispitne zadatke
	2.Samostalno vjezbati
	Predlazemo Vam da ispunite i ostale planirane obaveze.
	FIT Reminder
	-------------------------------------------------------------------------
	Dakle, notifikacije ce biti poslane svim studentima koji su dodali dogadjaj za 30.01.2018. godine i oznacili da zele da budu podsjecani ponovo/rekurzivno najmanje 2 dana prije samog dogadjaja (podaci se odnose na konkretan dogadjaj: Ispit iz PRIII)

	*/
	int poslato = 0;
	////funkcija vraca broj poslatih podsjetnika/notifikacija
	poslato = reminder.PosaljiNotifikacije(danas);
	cout << "Za " << danas << " poslato ukupno " << poslato << " podsjetnika!" << endl;
	poslato = reminder.PosaljiNotifikacije(sutra);
	cout << "Za " << sutra << " poslato ukupno " << poslato << " podsjetnika!" << endl;

#pragma endregion

	system("pause");
}
