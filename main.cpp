#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cmath>
#include "matrice.h"

#define EPS 0.85
using namespace std;

//declaration matrices
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

	void read(Matrice *m) 
	{
		readHeader();
		int off, n;
		int c;
		int x, y;
		double valeur;
		
		string buffer;
		
		while (!file.eof())
		{
			std::cout << "test" << std::endl;
			getline(file, buffer);
			off = 0;
			if(0 == std::sscanf(buffer.c_str() + off, "%d%n", &x, &n)) {break;}
			
			off += n;
			std::sscanf(buffer.c_str() + off, "%d%n", &c, &n);
			off += n;
			std::cout << "test2" << std::endl;
			
			while (2 == std::sscanf(buffer.c_str() + off, "%d%lf%n", &y, &valeur, &n)) 
			{
				std::cout << "test3" << std::endl;
				off += n;
				std::cout << "test4" << std::endl;
				//segfault ligne dessous
				m->inserer(x - offset, y - offset, valeur);
				std::cout << "test5" << std::endl;
			}
			std::cout << "test10" << std::endl;
		}
	}

	~Reader() { file.close(); }
};


//
void show_vecteur(double* vecteur, int size)
{
	int i = 0;
	
	for(i = 0; i < size; i++)
	{
		cout << vecteur[i] << " ";
	}
	cout << endl;
}


double* init_vecteur(int size, double value)
{
	double* vecteur = new double[size];
	for(int j = 0; j < size; j++)
		{
			vecteur[j] = value;
		}
	return vecteur;
}


//

double calcul_norme(double* vecteur, int size)
{
	int i = 0;
	double result = 0.0;
	
	for(i = 0;i< size;i++)
	{
		result += vecteur[i];
	}
	
	return result;
}

//fonction à modifier pour prendre en compte le surfer ( et initialiser G en même temps ? ) 
void fill_vecteur_min(double* vecteur, string line, int current_line)
{
	int i = 0;
	int reading = 0;
	
	//int line_size = 0;
	double tmp_double = 0.0;
	
	string current = "";
	
	while(line[i] != '\n' && line[i] != '\0')
	{
		while(line[i] == ' ' && line[i] != '\0')
		{
			i++;
		}
		while((line[i] >= '0' && line[i] <= '9') || line[i] == 'E' || line[i] == '-' || line[i] == '.')
		{
			current += line[i];
			i++;
		}
		
		//cout << current << "\n";
		if(reading == 0)
		{
			//ignore le premier chiffre
			reading = 1;
		}
		else if(reading == 1)
		{
			//stocke la taille de la ligne
			reading = 2;
			//line_size = stoi(current);
		}
		else if(reading == 2)
		{
			//recupere la valeur double
			reading = 3;
			if( !current.empty() ){tmp_double = stod(current);}
							
		}
		else if(reading == 3)
		{
			//ecrit la valeur double dans la case suivante
			if(tmp_double < vecteur[current_line] || vecteur[current_line] == 0.0)
			{
				vecteur[current_line] = tmp_double;
			}
			reading = 2;
		}
		i++;
		current = "";
	}
}

//fonction à modifier pour prendre en compte le surfer
void fill_vecteur_max(double* vecteur, string line, int current_line)
{
	int i = 0;
	int reading = 0;
	
	//int line_size = 0;
	double tmp_double = 0.0;
	
	string current = "";
	
	while(line[i] != '\n' && line[i] != '\0')
	{
		while(line[i] == ' ' && line[i] != '\0')
		{
			i++;
		}
		while((line[i] >= '0' && line[i] <= '9') || line[i] == 'E' || line[i] == '-' || line[i] == '.')
		{
			current += line[i];
			i++;
		}
		
		//cout << current << "\n";
		if(reading == 0)
		{
			//ignore le premier chiffre
			reading = 1;
		}
		else if(reading == 1)
		{
			//stocke la taille de la ligne
			reading = 2;
			//line_size = stoi(current);
		}
		else if(reading == 2)
		{
			//recupere la valeur double
			reading = 3;
			if( !current.empty() ){tmp_double = stod(current);}
							
		}
		else if(reading == 3)
		{
			//ecrit la valeur double dans la case suivante
			if(tmp_double > vecteur[current_line] || vecteur[current_line] == 0.0)
			{
				vecteur[current_line] = tmp_double;
			}
			reading = 2;
		}
		i++;
		current = "";
	}
}

double* produit_nabla_norme(double* nabla, double Xk, int size)
{
	double* result = init_vecteur(size, 0.0);
	for(int i = 0; i<size; i++)
	{
		result[i] = nabla[i] * Xk;
	}
	return result;
}

int main(int argc, char **argv)
{
	string line;
	ifstream file(argv[1]);
	
	int line_number = 0;
	int current_line = 0;
	double norme_x = 0;
	double norme_y = 0;

	
	double* vecteur_max_delta = NULL;
	double* vecteur_min_nabla = NULL;
	double* vecteur_x = NULL;
	double* vecteur_y = NULL;
	
	//init matrice web
	Reader r(argv[1]);
	r.offset = 1;
	r.readHeader();
	MatriceWeb matrice(r.getNbSommets());
	r.read(&matrice);
	
	//variables tests à supprimer au final
	double* vecteur_x_tmp = NULL;
	double* vecteur_y_tmp = NULL;
	
	if(file)
	{
		while(getline(file, line))
		{
			if(current_line == 0)
			{
				
			}
			else if(current_line == 1)
			{
				line_number = stoi(line);
				vecteur_max_delta = init_vecteur(line_number, 0.0);
				vecteur_min_nabla = init_vecteur(line_number, 0.0);
				vecteur_x = init_vecteur(line_number, 0.0);
				vecteur_y = init_vecteur(line_number, 0.0);
			}
			else if(current_line >= 2)
			{
				fill_vecteur_min(vecteur_min_nabla, line, current_line-2);
				fill_vecteur_max(vecteur_max_delta, line, current_line-2);
			}
			current_line++;
		}	
	}
	
	//init X^0 Y^0
	vecteur_x = vecteur_max_delta;
	vecteur_y = vecteur_min_nabla;
	
	show_vecteur(vecteur_x, line_number);
	cout << endl;
	show_vecteur(vecteur_y, line_number);
	
	//future boucle tant que calcul_norme(difference(vecteur_x, vecteur_y)) > epsilon
	norme_x = calcul_norme(vecteur_x, line_number);
	norme_y = calcul_norme(vecteur_y, line_number);
	
	cout << "norme x : " << norme_x << " norme_y : " << norme_y << endl;
	
	norme_x = 1-norme_x;
	norme_y = 1-norme_y;
	
	cout << "norme x : " << norme_x << " norme_y : " << norme_y << endl;
	
	//tester pour s'assurer qu'il y a bien surcharge d'opérateur sur l'affectation, et pas que seuls les pointeurs sont copiés......
	vecteur_x_tmp = produit_nabla_norme(vecteur_min_nabla, norme_x, line_number);
	vecteur_y_tmp = produit_nabla_norme(vecteur_min_nabla, norme_y, line_number);
	
	//a faire
	// XkG = produit(vecteur_x, G)
	// XkG += somme_vect(XkG, produit_nabla_norme(vecteur_min_nabla, norme_x, line_number));
	// YkG = produit(vecteur_y, G)
	// YkG += somme_vect(YkG, produit_nabla_norme(vecteur_min_nabla, norme_y, line_number))
	// boucle
	//		|vecteur_x[i] = max(vecteur_x[i], XkG[i])
	//		|vecteur_y[i] = min(vecteur_y[i], YkG[i])
	// fin boucle
	//
	// 
	// continuer tant que truc epsilon faux
	

	
	return 0;
}
