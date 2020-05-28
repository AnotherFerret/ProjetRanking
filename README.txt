Compilation :

commande "make"

execution : 

./output "fichier contenant la matrice" "offset" "order" "header"

offset = 0 si le premier sommet de la matrice vaut 0, 1 sinon
order = 0 si le format des arcs est "$sommet $valeur", 1 si le format des arcs est "$valeur $sommet"
header = 0 si le nombre de sommet est en première ligne et le nombre d'arcs en seconde, 1 sinon

Pour Stanford : 
./a.out Stanford.txt 1 0 1
Avec alpha = 0.45
Méthode classique : 4.91s
Méthode du vecteur nabla : 19.10s

Pour les graphes petit moyen et large :
./a.out petit.txt 0 0 1
./a.out moyen.txt 0 0 1
./a.out large.txt 0 0 1

