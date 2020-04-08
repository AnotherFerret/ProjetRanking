#include <string>
#include <iostream>
#include <fstream>

using namespace std;

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
	//int element_number = 0;
	int current_line = 0;
	double norme_x = 0;
	double norme_y = 0;

	
	double* vecteur_max_delta = NULL;
	double* vecteur_min_nabla = NULL;
	double* vecteur_x = NULL;
	double* vecteur_y = NULL;
	
	//variables tests à supprimer au final
	double* vecteur_x_tmp = NULL;
	double* vecteur_y_tmp = NULL;
	
	if(file)
	{
		while(getline(file, line))
		{
			if(current_line == 0)
			{
				//element_number = stoi(line);
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
