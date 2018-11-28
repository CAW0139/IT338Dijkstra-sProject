#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define INFINITY 9999

void Read_size();
void Read_matrix(int **matrix);
void Dijkstra(int **matrix, int source, int dest);
int Find_min_dist(int dist[], bool visited[]);
void Print_path(int dist[], int source, int dest);

int size = 0, thread_count = 0, source = 0, dest = -1;
FILE *fileptr;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Expecting number of threads. Exiting\n");
		exit(0);
	}
	
	int thread_count = strtol(argv[1], NULL, 10);
	
	if (thread_count < 1)
	{
		printf("Invalid number of threads. Exiting\n");
		exit(0);
	}

	Read_size();
	
	int *matrix[size], i;

	# pragma omp parallel for num_threads(thread_count)
	for (i=0; i<size; i++)
		matrix[i] = (int*)malloc(size*sizeof(int));
	
	Read_matrix(matrix);
	
	//Just to view the final destination's distance
	dest = 4;
	
	Dijkstra(matrix, source, dest);
	
	return 1;
}	/* end main */

/*
 * Reads the first line in the matrix file to get the number of verticies
 * and thus the size of the double array to create
 */
void Read_size()
{
	char *filename = "matrix";
	
	/*printf("Enter filename of input: ");
	if (scanf("%s", &filename) == 0)
	{
		printf("Did not set the filename correctly\n");
		exit(0);
	}*/
	
	fileptr = fopen(filename, "r");
	fscanf(fileptr, "%d", &size);
}	/* end Read_size */

/*
 * Reads the remainder of the matrix file to create the double array (matrix)
 */
void Read_matrix(int **matrix)
{
	int i, j, val;
	
	for (i=0; i<size; i++)
	{
		for (j=0; j<size; j++)
		{
			
			fscanf(fileptr, "%d", &val);
			matrix[i][j] = val;
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}
	
	printf("\n");
	fclose(fileptr);
}	 /* end Read_matrix */

/*
 * Basic Dijkstra algorithm that calls Print_path at the end
 * Uses Find_min_dist, Print_path
 */
void Dijkstra(int **matrix, int source, int dest)
{
	int dist[size], i, j;
	bool visited[size];
	
	# pragma omp parallel for num_threads(thread_count)
	for (i=0; i<size; i++)
	{
		dist[i] = INFINITY;
		visited[i] = false;
	}
	
	dist[source] = 0;
	
	//Finds the shortest path for all verticies in the matrix
	for (i=0; i<size-1; i++)
	{
		//Finds the min distance vertex from those not visited yet.
		//Always equal to source in the first iteration
		int v = Find_min_dist(dist, visited);
		visited[v] = true;
		
		//Update the dist value for the adjacent verticies of the selected vertex
		# pragma omp parallel for num_threads(thread_count)
		for (j=0; j<size; j++)
		{
			//Update dist only if it is not visited, there is an edge from i to j,
			//and that the total length of the path is less than the current value of dist[j]
			if (!visited[j] && matrix[i][j] && dist[i] != INFINITY && dist[i]+matrix[i][j] < dist[j])
			{
				dist[j] = dist[i] + matrix[i][j];
			}
		}
	}
	
	Print_path(dist, source, dest);
}	/* end Dijkstra */

/*
 * Finds the min distance by the vertexes that have not been visited yet
 */
int Find_min_dist(int dist[], bool visited[])
{
	int min = INFINITY, min_index, i;
	
	for (i=0; i<size; i++)
	{
		if (visited[i] == false && dist[i] <= min)
		{
			min = dist[i];
			min_index = i;
		}
	}
	
	return min_index;
}	/* end Find_min_dist */

/*
 * Prints the paths to all verticies (currently) as well as the
 * shortest path in the matrix to any node and the path to the 
 * destination node (if it exists)
 */
void Print_path(int dist[], int source, int dest)
{
	int i, shortest = INFINITY, short_index;
	
	printf("Vertex   Distance from Source\n");
	for (i=0; i<size; i++)
		printf("%d \t %d\n", i, dist[i]);
	
	//Possible TODO - idk how to parallelize this (if possible because I can't figure
	//	out how to get pragma omp critical to do multiple lines)
	for (i=0; i<size; i++)
	{
		if (dist[i]<shortest && i!=source)
		{
			shortest = dist[i];
			short_index = i;
		}
	}
	
	printf("\nThe shortest path is to node: %d at length: %d\n", short_index, shortest);
	if (dist[dest] != INFINITY)
		printf("The destination's (%d) path length is: %d\n", dest, dist[dest]);
	else
		printf("The destination could not be reached however\n");
}	/* end Print_path */