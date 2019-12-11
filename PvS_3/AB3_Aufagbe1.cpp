1 #include<stdio.h>
2 #include<mpi.h> // Importiert die Library
3
4 int main(int argc, char** argv){
5 int nodeID, numNodes;
6
7 /* Grundfunktionen */
8 MPI_Init(&argc, &argv); // Initialisert das MPI-Laufzeitsystem
9 MPI_Comm_size(MPI_COMM_WORLD, &numNodes); // Asnzahl aller Prozesse im Kommunikator
10 MPI_Comm_rank(MPI_COMM_WORLD, &nodeID); // Rank/Nummer
11
12 /* Ausgabe der ID und der Anzahl aller IDs */
13 printf("Hello world from process %d of %d\n", nodeID, numNodes);
14
15 /* Meldet Prozesse beim MPI-Laufzeitsystem ab */
16 MPI_Finalize();
17
18 return 0;
19 }