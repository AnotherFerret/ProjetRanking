#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cmath>

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#define ABS(N) (((N)<0)?(-(N)):((N)))

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
class MatriceG : public Matrice {
private:
	int n;
	int nd0;
	double alpha;
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
	MatriceG(int n, double alpha) :Matrice(n), n(n), nd0(n), alpha(alpha) {
		matrice = new struct liste_tag *[n];
		degrees = new int[n];
		for (int i = 0; i < n; i++) {
			matrice[i] = nullptr;
			degrees[i] = 0;
		}
	}

	int size(){return n;}
	virtual void inserer(int x, int y, double valeur) {
		if(degrees[x] == 0)
			nd0--;
		degrees[x]++;
		if (matrice[y] == nullptr) {
			matrice[y] = create_liste(x, valeur*alpha + (1-alpha)/n);
		}
		else
			inserer(matrice[y], x, valeur*alpha + (1-alpha)/n);
	}

	virtual double valeur(int x, int y) {
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
			return (1-alpha) / n;
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
				alphabar+= Pi0[i] * ((degrees[i] == 0)?(1.0/n):((1.0-alpha)/n));
			for (int i = 0; i < n; i++) {
				PiN[i] = alphabar;
				curseur = matrice[i];
				while(curseur != nullptr){
					PiN[i] += Pi0[curseur->sommet] * curseur->valeur;
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

	double getMin(int i){
		if(matrice[i] == nullptr)
			return (1.0-alpha)/n;
		struct liste_tag *curseur;
		curseur = matrice[i];
		double res = (1.0-alpha)/n;
		while (curseur != nullptr) {
			if (curseur->valeur < res) {
				res = curseur->valeur;
			}
			curseur = curseur->next;
		}
		return res;
	}

	double * getVecteurMin(){
		double * res = new double[n];
		for(int i = 0; i< n; i++){
			res[i] = getMin(i);
		}
		return res;
	}

	double getMax(int i){
		if(matrice[i] == nullptr)
			return 1.0 /n;
		struct liste_tag *curseur;
		curseur = matrice[i];
		double res = (1.0-alpha)/n;
		while (curseur != nullptr) {
			if (curseur->valeur > res) {
				res = curseur->valeur;
			}
			curseur = curseur->next;
		}
		return res;
	}

	double * getVecteurMax(){
		double * res = new double[n];
		for(int i = 0; i< n; i++){
			res[i] = getMax(i);
		}
		return res;
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
			alphabar+= vecteurEntree[i] * ((degrees[i] == 0)?(1.0/n):((1.0-alpha)/n));
		}
		for (int i = 0; i < n; i++) 
		{
			vecteurSortie[i] = alphabar;
			curseur = matrice[i];
			while(curseur != nullptr)
			{
				vecteurSortie[i] += vecteurEntree[curseur->sommet] * curseur->valeur;
				curseur = curseur->next;
			}
		}
    }

	~MatriceG() {
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
	int order = 0;
	int h = 0;
	Reader(const char * filename, int offset, int order, int h) {
		this->offset = offset;
		this->order = order;
		this->h = h;
		file.open(filename);
	}

	int getNbSommets() { return nbSommets; }
	int getNbArcs() { return nbArcs; }

	void readHeader() {
		std::string buffer;
		file.seekg(0, file.beg);
		if (h == 0){
			getline(file, buffer);
			std::sscanf(buffer.c_str(), "%d", &nbSommets);
			getline(file, buffer);
			std::sscanf(buffer.c_str(), "%d", &nbArcs);
		}else{
			getline(file, buffer);
			std::sscanf(buffer.c_str(), "%d", &nbArcs);
			getline(file, buffer);
			std::sscanf(buffer.c_str(), "%d", &nbSommets);
		}
	}

	void read(Matrice *m) {
		readHeader();
		int off, n;
		int c;
		int x, y;
		double valeur;
		std::string buffer;
		if(order == 0){
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
		}else{
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
				while (2 == std::sscanf(buffer.c_str() + off, "%lf%d%n", &valeur, &y, &n)) {
					off += n;
					m->inserer(x - offset, y - offset, valeur);
				}
			}
		}
	}

	~Reader() { file.close(); }
};

void somme_vect(double * vect1, double * vect2, int size){
	for(int i = 0; i < size; i++)
	{
		vect1[i] += vect2[i];
	}
}

double* diff_vect(double * vect1, double * vect2, int size){
	double* vecteur = new double[size];
	for(int i = 0; i < size; i++)
	{
		vecteur[i] = ABS(vect1[i] - vect2[i]);
	}
	return vecteur;
}

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

double calcul_norme(double* vecteur1, double* vecteur2, int size)
{
	int i = 0;
	double result = 0.0;
	
	for(i = 0;i< size;i++)
	{
		result += ABS(vecteur1[i]-vecteur2[i]);
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

void produit_nabla_norme(double* nabla, double Xk, int size, double * nablap)
{
	for(int i = 0; i<size; i++)
	{
		nablap[i] = nabla[i] * (1.0-Xk);
	}
}

int main(int argc, char **argv)
{
	string line;
	ifstream file(argv[1]);
	
	int line_number = 0;
	int current_line = 0;
	double norme_x = 0;
	double norme_y = 0;

	
	//init matrice web
	Reader r(argv[1],atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
	r.readHeader();
	MatriceG matrice(r.getNbSommets(), EPS);
	r.read(&matrice);
	
	//variables tests à supprimer au final
	double* vecteur_max_delta = matrice.getVecteurMax();
	double* vecteur_min_nabla = matrice.getVecteurMin();

	double* vecteur_xkg = new double[matrice.size()];
	double* vecteur_ykg = new double[matrice.size()];

	double* vecteur_x = NULL;
	double* vecteur_y = NULL;
	
	/*if(file)
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
	
	show_vecteur(vecteur_x, line_number);
	cout << endl;
	show_vecteur(vecteur_y, line_number);
	cout << endl;*/

	vecteur_x = matrice.getVecteurMin();
	vecteur_y = matrice.getVecteurMax();
	
	int count = 0;
	double reste = 0; 
	double * nablax = new double[matrice.size()];
	double * nablay = new double[matrice.size()];
	int n = matrice.size();
	//future boucle tant que calcul_norme(difference(vecteur_x, vecteur_y)) > epsilon
	cout << "taille : "<< n << ":: résidus : " << reste << endl;
	while((reste = calcul_norme(vecteur_x, vecteur_y, n)) > 1e-6){

		cout << "itération : "<< count << ":: résidus : " << reste << ":: x :" << calcul_norme(vecteur_x, n) << ":: y :" << calcul_norme(vecteur_y, n) << endl;

		matrice.produitVecteur(vecteur_x, vecteur_xkg);
		produit_nabla_norme(vecteur_min_nabla, calcul_norme(vecteur_x, n),n, nablax);
		somme_vect(vecteur_xkg, nablax, n);

		matrice.produitVecteur(vecteur_y, vecteur_ykg);
		produit_nabla_norme(vecteur_min_nabla, calcul_norme(vecteur_y, n),n, nablay);
		somme_vect(vecteur_ykg, nablay, matrice.size());

		for(int i = 0; i < n; i++){
			vecteur_x[i] = max(vecteur_x[i], ABS(vecteur_xkg[i]));
			vecteur_y[i] = min(vecteur_y[i], ABS(vecteur_ykg[i]));
		}
		count ++;
	}
	cout << "itération : "<< count << ":: résidus : " << reste << endl;
/*
	norme_x = calcul_norme(vecteur_x, line_number);
	norme_y = calcul_norme(vecteur_y, line_number);
	
	cout << "norme x : " << norme_x << " norme_y : " << norme_y << endl;
	cout << endl;
	cout << "norme x : " << calcul_norme(matrice.getVecteurMax(), matrice.size()) << " norme_y : " << calcul_norme(matrice.getVecteurMin(), matrice.size()) << endl;

	norme_x = 1-norme_x;
	norme_y = 1-norme_y;
	
	cout << "norme x : " << norme_x << " norme_y : " << norme_y << endl;
	
	//tester pour s'assurer qu'il y a bien surcharge d'opérateur sur l'affectation, et pas que seuls les pointeurs sont copiés......
	vecteur_x_tmp = produit_nabla_norme(vecteur_min_nabla, norme_x, line_number);
	vecteur_y_tmp = produit_nabla_norme(vecteur_min_nabla, norme_y, line_number);
	*/



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
