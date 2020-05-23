#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cmath>
#include <iomanip>
#include <chrono>

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#define ABS(N) (((N)<0)?(-(N)):((N)))

#define EPS 0.85
using namespace std;

double calcul_norme(double* vecteur1, double* vecteur2, int size);

std::string beautify_duration(std::chrono::nanoseconds nanoseconds)
{
    using namespace std::chrono;
    typedef duration<int, std::ratio<86400>> days;
    auto d = duration_cast<days>(nanoseconds);
    nanoseconds -= d;
    auto h = duration_cast<hours>(nanoseconds);
    nanoseconds -= h;
    auto m = duration_cast<minutes>(nanoseconds);
    nanoseconds -= m;
    auto s = duration_cast<seconds>(nanoseconds);
    nanoseconds -= s;
    auto ms = duration_cast<milliseconds>(nanoseconds);
    nanoseconds -= ms;

    auto dc = d.count();
    auto hc = h.count();
    auto mc = m.count();
    auto sc = s.count();
    auto msc = ms.count();
    auto nsc = nanoseconds.count();

    std::stringstream ss;
    ss.fill('0');
    if (dc) {
        ss << d.count() << "d";
    }
    if (dc || hc) {
        if (dc) { ss << std::setw(2); } //pad if second set of numbers
        ss << h.count() << "h";
    }
    if (dc || hc || mc) {
        if (dc || hc) { ss << std::setw(2); }
        ss << m.count() << "m";
    }
    if (dc || hc || mc || sc) {
        if (dc || hc || mc) { ss << std::setw(2); }
        ss << s.count() << 's';
    }
    if (dc || hc || mc || sc || msc) {
        if (dc || hc || mc || sc) { ss << std::setw(3); }
        ss << ms.count() << "ms";
    }
    if (dc || hc || mc || sc || msc || nsc) {
        if (dc || hc || mc || sc || msc) { ss << std::setw(3); }
        ss << nanoseconds.count() << "ns";
    }

    return ss.str();
}

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
			matrice[y] = create_liste(x, valeur);
		}
		else
			inserer(matrice[y], x, valeur);
	}

	virtual double valeur(int x, int y) {
		struct liste_tag *curseur;
		curseur = matrice[y];
		while (curseur != nullptr) {
			if (curseur->sommet == x) {
				return curseur->valeur * alpha + (1-alpha) / n;
			}
			curseur = curseur->next;
		}
		if (degrees[x] == 0) {
			return 1.0 / n;
		}
		else
			return (1-alpha) / n;
	}

	void produitVecteur(double* vecteurEntree, double* vecteurSortie)
	{
		double alphabar = 0;
		liste_tag* curseur;
		for (int i = 0; i < n; i++)
		{
			alphabar+= vecteurEntree[i] * ((degrees[i] == 0)?((1.0)/n):((1.0-alpha)/n));
		}
		for (int i = 0; i < n; i++) 
		{
			vecteurSortie[i] = alphabar;
			curseur = matrice[i];
			while(curseur != nullptr)
			{
				vecteurSortie[i] += vecteurEntree[curseur->sommet] * curseur->valeur * alpha;
				curseur = curseur->next;
			}
		}
    }

	virtual double *calculerPi(double epsilon) {
		auto start = chrono::high_resolution_clock::now();
		double *Pi0 = new double[n];
		double *PiN = new double[n];
		double *tab;
		double alphabar;
		int a = 0;
		for (int i = 0; i < n; i++) Pi0[i] = 1.0 / n;
		for (int i = 0; i < n; i++) PiN[i] = 1.0 / n;		
		double maxE;
		do {
			a++;
			alphabar = (1.0-alpha)/n;
			liste_tag* curseur;
			for (int i = 0; i < n; i++)
			{
				alphabar+= ((degrees[i] == 0)?(alpha/n * Pi0[i]):0);
			}
			for (int i = 0; i < n; i++) 
			{
				PiN[i] = alphabar;
				curseur = matrice[i];
				while(curseur != nullptr)
				{
					PiN[i] += Pi0[curseur->sommet] * curseur->valeur * alpha;
					curseur = curseur->next;
				}
			}
			maxE = calcul_norme(PiN, Pi0, n);
			//cout << "itérations : "<< a << ":: résidus : " << maxE << endl;
			tab = Pi0;Pi0 = PiN;PiN = tab;
		} while (maxE > epsilon);
		auto stop = chrono::high_resolution_clock::now();
		cout << "itérations : "<< a << ":: résidus : " << maxE << ":: time :" << beautify_duration(chrono::duration_cast<chrono::nanoseconds>(stop-start)) << endl;
		return PiN;
	}

	double getMin(int i){
		if(matrice[i] == nullptr)
			return (1.0-alpha)/n;
		struct liste_tag *curseur;
		curseur = matrice[i];
		double res = (1.0-alpha)/n;
		while (curseur != nullptr) {
			if (curseur->valeur * alpha + (1.0-alpha)/n < res) {
				res = curseur->valeur *alpha + (1.0-alpha)/n;
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
			if (curseur->valeur * alpha + (1.0-alpha)/n > res) {
				res = curseur->valeur * alpha + (1.0-alpha)/n;
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
				/*if(x%10000 == 0){
					std::cout << (x- offset) << std::endl;
				}*/
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
				if(x%10000 == 0){
					std::cout << (x- offset) << std::endl;
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

void show_vecteur(double* vecteur, int size)
{
	int i = 0;
	
	for(i = 0; i < size; i++)
	{
		cout << vecteur[i] << " ";
	}
	cout << endl;
}

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

void produit_nabla_norme(double* nabla, double Xk, int size, double * nablap)
{
	for(int i = 0; i<size; i++)
	{
		nablap[i] = nabla[i] * (1.0-Xk);
	}
}

int main(int argc, char **argv)
{
	cout << argv[1] << endl;
	string line;
	ifstream file(argv[1]);
	
	/*int line_number = 0;
	int current_line = 0;
	double norme_x = 0;
	double norme_y = 0;*/

	
	//init matrice web
	Reader r(argv[1],atoi(argv[2]),atoi(argv[3]),atoi(argv[4]));
	r.readHeader();
	MatriceG matrice(r.getNbSommets(), EPS);
	auto startr = chrono::high_resolution_clock::now();
	r.read(&matrice);
	auto stopr = chrono::high_resolution_clock::now();
	
	//double* vecteur_max_delta = matrice.getVecteurMax();
	double* vecteur_min_nabla = matrice.getVecteurMin();

	double* vecteur_xkg = new double[matrice.size()];
	double* vecteur_ykg = new double[matrice.size()];

	double* vecteur_x = NULL;
	double* vecteur_y = NULL;
	

	vecteur_x = matrice.getVecteurMin();
	vecteur_y = matrice.getVecteurMax();
	
	int count = 0;
	double reste = 0; 
	double * nablax = new double[matrice.size()];
	double * nablay = new double[matrice.size()];
	int n = matrice.size();
	cout << "taille : "<< n << ":: time :" << beautify_duration(chrono::duration_cast<chrono::nanoseconds>(stopr-startr)) << endl;
	double * pi = matrice.calculerPi(1e-6);
	auto start = chrono::high_resolution_clock::now();
	while((reste = calcul_norme(vecteur_x, vecteur_y, n)) > 1e-6){
		//cout << "itération : "<< count << ":: résidus : " << reste << ":: x :" << calcul_norme(vecteur_x, n) << ":: y :" << calcul_norme(vecteur_y, n) << endl;

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
	auto stop = chrono::high_resolution_clock::now();
	cout << "itérations : "<< count << ":: résidus : " << reste << ":: time :" << beautify_duration(chrono::duration_cast<chrono::nanoseconds>(stop-start)) << endl;
	cout << "résidus X - produit succecifs :" << calcul_norme(vecteur_x, pi, n)  << endl << "résidus Y - produit succecifs :" << calcul_norme(vecteur_y, pi, n) << endl;
	
	return 0;
}
