#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cmath>
#define EPS 0.85

class Matrice {
private:
	int n;
public:
	Matrice(int n):n(n) {};
	virtual void inserer(int x, int y, double valeur) { }
	virtual double valeur(int x, int y) { return 1; }
	virtual double *calculerPi(double epsilon) { double *Pi = new double[n]; for (int i = 0; i < n; i++) Pi[i] = 1/n; return Pi; }
	virtual void afficher() { std::cout << "Matrice Abstraite"; }
};

class MatricePleine: public Matrice{
private:
	int n;
	double** matrice;
public:
	MatricePleine(int n) :Matrice(n), n(n) {
		matrice = new double*[n];
		if (n) {
			matrice[0] = new double[n*n];
			for (int i = 1; i < n; i++) {
				matrice[i] = matrice[0] + i * n;
			}
			for (int i = 0; i < n*n; i++)
				matrice[0][i] = 0;
		}
	};
	virtual void inserer(int x, int y, double valeur) { matrice[x][y] = valeur; }
	
	virtual double valeur(int x, int y) { return matrice[x][y]; }

	virtual double *calculerPi(double epsilon) {
		double *Pi0 = new double[n];
		double *tab;
		for (int i = 0; i < n; i++) Pi0[i] = 1.0 / n;
		double *PiN = new double[n];
		double maxE, tmp;
		do {
			maxE = 0;
			for (int i = 0; i < n; i++) {
				PiN[i] = 0;
				for (int j = 0; j < n; j++) {
					PiN[i] += valeur(j, i) * Pi0[j];
				}
				if (maxE < (tmp = fabs(PiN[i] - Pi0[i])))
					maxE = tmp;
			}
			tab = Pi0; Pi0 = PiN; PiN = tab;
		} while (maxE > epsilon);
		return PiN; 
	}

	virtual void afficher() {
		std::cout << "[";
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++)
			{
				std::cout << valeur(i, j) << "  ";
			}
			std::cout << std::endl;
		}
		std::cout << "]";
	}
	
	~MatricePleine() {
		if (n) delete[] matrice[0];
		delete[] matrice;
	}
};

class MatriceCreuse : public Matrice {
private:
	int n;
	class liste_t {
	public:
		liste_t *prec;
		liste_t *next;
		int sommet;
		double valeur;
		liste_t(int sommet, double valeur) :sommet(sommet), valeur(valeur), prec(nullptr), next(nullptr) {}
		void inserer(int x, double v) {
			if (sommet == x)
				valeur = v;
			else if (next == nullptr){
				next = new liste_t(x, v);
				next->prec = this;
			}else
				next->inserer(x, v);
		}
		~liste_t() {
			if(next != nullptr)
				delete next;
			if(prec != nullptr)
				prec->next = nullptr;
		}
	};
	liste_t **matrice;
public:
	MatriceCreuse(int n) :Matrice(n), n(n) {
		matrice = new liste_t*[n];
		for (int i = 0; i < n; i++) matrice[i] = nullptr;
	}
	virtual void inserer(int x, int y, double valeur) { 
		if (matrice[y] == nullptr)
			matrice[y] = new liste_t(x, valeur);
		else
			matrice[y]->inserer(x, valeur);
	}
	virtual double valeur(int x, int y) {
		liste_t *curseur;
		curseur = matrice[y];
		while (curseur != nullptr) {
			if (curseur->sommet == x) {
				return curseur->valeur;
			}
			curseur = curseur->next;
		}
		return 0;
	}

	virtual double *calculerPi(double epsilon) {
		double *Pi0 = new double[n];
		double *tab;
		int a = 0;
		for (int i = 0; i < n; i++) Pi0[i] = 1.0 / n;
		double *PiN = new double[n];
		double maxE, tmp;
		do {
			a++;
			maxE = 0;
			for (int i = 0; i < n; i++) {
				PiN[i] = 0;
				for (int j = 0; j < n; j++) {
					PiN[i] += valeur(j, i) * Pi0[j];
				}
				if (maxE < (tmp = fabs(PiN[i] - Pi0[i])))
					maxE = tmp;
			}
			std::cout << maxE << ";" << a << std::endl;
			tab = Pi0; Pi0 = PiN; PiN = tab;
		} while (maxE > epsilon);
		return PiN;
	}

	virtual void afficher() {
		std::cout << "[";
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++)
			{
				std::cout << valeur(i, j) << "  ";
			}
			std::cout << std::endl;
		}
		std::cout << "]";
	}

	~MatriceCreuse() {
		for (int i = 1; i < n; i++)
		{
			if (matrice[i] != nullptr)
				delete matrice[i];
		}
		delete [] matrice;
	}
};

class MatriceWeb : public Matrice {
private:
	int n;
	int nd0;
	struct liste_tag{
		struct liste_tag *prec;
		struct liste_tag *next;
		double valeur;
		int sommet;
	};
	struct liste_tag *create_liste(int sommet, double valeur){
		struct liste_tag *liste = (struct liste_tag *) malloc(sizeof(struct liste_tag));
		liste->sommet = sommet;
		liste->valeur = valeur;
		liste->prec = nullptr;
		liste->next = nullptr;
		return liste;
	}
	int inserer(struct liste_tag *liste, int x, double v) {
		while(liste->next != nullptr){
			liste = liste->next;
			if (liste->sommet == x) {
				liste->valeur = v;
				return 0;
			}
		}
		liste->next = create_liste(x, v);
		liste->next->prec = liste;
		return 1;
	}
	void delete_liste(struct liste_tag *liste) {
		if (liste->next != nullptr)
			delete_liste(liste->next);
		if (liste->prec != nullptr)
			liste->prec->next = nullptr;
		free(liste);
	}
	struct liste_tag **matrice;
	int *degrees;
public:
	MatriceWeb(int n) :Matrice(n), n(n), nd0(n) {
		matrice = new struct liste_tag *[n];
		degrees = new int[n];
		for (int i = 0; i < n; i++) {
			matrice[i] = nullptr;
			degrees[i] = 0;
		}
	}
	virtual void inserer(int x, int y, double valeur) {
		if(degrees[x] == 0)
			nd0--;
		degrees[x]++;
		if (matrice[y] == nullptr) {
			matrice[y] = create_liste(x, valeur);
		}
		else
			inserer(matrice[y], x, valeur);
	}

	double valeurM(int x, int y) {
		struct liste_tag *curseur;
		curseur = matrice[y];
		while (curseur != nullptr) {
			if (curseur->sommet == x) {
				return curseur->valeur;
			}
			curseur = curseur->next;
		}
		if (degrees[x] == 0) {
			return 1.0 / n;
		}
		else
			return 0;
	}

	virtual double valeur(int x, int y) {
		return (valeurM(x,y)*EPS)+((1-EPS)/n);
	}

	virtual double *calculerPi(double epsilon) {
		double *Pi0 = new double[n];
		double *tab;
		int a = 0;
		for (int i = 0; i < n; i++) Pi0[i] = 1.0 / n;
		double *PiN = new double[n];
		double maxE, tmp, alphabar;
		struct liste_tag *curseur;
		do {
			a++;
			maxE = 0;
			alphabar = 0;
			for (int i = 0; i < n; i++)
				alphabar+= Pi0[i] * ((degrees[i] == 0)?(1.0/n):((1.0-EPS)/n));
			for (int i = 0; i < n; i++) {
				PiN[i] = alphabar;
				curseur = matrice[i];
				while(curseur != nullptr){
					PiN[i] += Pi0[curseur->sommet] * EPS * curseur->valeur;
					curseur = curseur->next;
				}
				/*for (int j = 0; j < n; j++) {
					PiN[i] += valeur(j, i) * Pi0[j];
				}*/
				if (maxE < (tmp = fabs(PiN[i] - Pi0[i])))
					maxE = tmp;
			}
			//std::cout << std::endl;
			//for (int i = 0; i < n; i++) {
			//	std::cout << PiN[i] << ":";
			//}
			//std::cout << std::endl;
			std::cout << maxE << ";" << a << std::endl;
			tab = Pi0; Pi0 = PiN; PiN = tab;
		} while (maxE > epsilon);
		return PiN;
	}

	virtual void afficher() {
		std::cout << "[";
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++)
			{
				std::cout << valeur(i, j) << "  ";
			}
			std::cout << std::endl;
		}
		std::cout << "]";
	}
	
	void produitVecteur(double* vecteurEntree, double* vecteurSortie)
	{
		double alphabar = 0;
		liste_tag* curseur;
		for (int i = 0; i < n; i++)
		{
			alphabar+= vecteurEntree[i] * ((degrees[i] == 0)?(1.0/n):((1.0-EPS)/n));
		}
		for (int i = 0; i < n; i++) 
		{
			vecteurSortie[i] = alphabar;
			curseur = matrice[i];
			while(curseur != nullptr)
			{
				vecteurSortie[i] += vecteurEntree[curseur->sommet] * EPS * curseur->valeur;
				curseur = curseur->next;
			}
		}
    }

	~MatriceWeb() {
		for (int i = 1; i < n; i++)
		{
			if (matrice[i] != nullptr)
				delete matrice[i];
		}
		delete[] matrice;
	}
};

class Reader {
private:
	int nbSommets;
	int nbArcs;
	std::ifstream file;
public:
	int offset = 1;
	Reader(const char * filename) {
		file.open(filename);
	}

	int getNbSommets() { return nbSommets; }
	int getNbArcs() { return nbArcs; }

	void readHeader() {
		std::string buffer;
		file.seekg(0, file.beg);
		getline(file, buffer);
		std::sscanf(buffer.c_str(), "%d", &nbSommets);
		getline(file, buffer);
		std::sscanf(buffer.c_str(), "%d", &nbArcs);
	}

	void read(Matrice *m) {
		readHeader();
		int off, n;
		int c;
		int x, y;
		double valeur;
		std::string buffer;
		while (!file.eof())
		{
			getline(file, buffer);
			off = 0;
			if(0 == std::sscanf(buffer.c_str() + off, "%d%n", &x, &n))
				break;
			off += n;
			std::sscanf(buffer.c_str() + off, "%d%n", &c, &n);
			off += n;
			//std::cout << x << std::endl;
			while (2 == std::sscanf(buffer.c_str() + off, "%d%lf%n", &y, &valeur, &n)) {
				off += n;
				m->inserer(x - offset, y - offset, valeur);
			}
		}
		//std::cout << "prout";
	}

	~Reader() { file.close(); }
};

int main(int argc, char* argv[])
{
	Reader r("Stanford.txt");
	r.offset = 1;
	r.readHeader();
	int n = r.getNbSommets();
	//std::cout << n;
	MatriceWeb matrice(n);
	r.read(&matrice);
	std::cout << std::endl;
	//matrice.afficher();
	std::cout << std::endl;
	double * pi = matrice.calculerPi(1e-6);
	//std::cout << std::endl;
	//for (int i = 0; i < n; i++) {
	//	std::cout << pi[i] << ":";
	//}
	std::cout << std::endl;

}
