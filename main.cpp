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

int main(int argc, char **argv)
{
	string line;
	ifstream file(argv[1]);
	
	int line_number = 0;
	//int element_number = 0;
	int current_line = 0;

	
	double* vecteur_max = NULL;
	double* vecteur_min = NULL;
	
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
				vecteur_max = init_vecteur(line_number, 0.0);
				vecteur_min = init_vecteur(line_number, 0.0);
			}
			else if(current_line >= 2)
			{
				fill_vecteur_min(vecteur_min, line, current_line-2);
				fill_vecteur_max(vecteur_max, line, current_line-2);
			}
			current_line++;
		}	
	}
	
	show_vecteur(vecteur_max, line_number);
	cout << endl;
	show_vecteur(vecteur_min, line_number);
	
	return 0;
}
