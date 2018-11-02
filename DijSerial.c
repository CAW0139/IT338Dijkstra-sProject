#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INFINITY 9999	//Unvisited node weight
#define MAX 10			//Max size of the matrix
#define VERTICIES 5		//Number of verticies for testing
#define STARTINGNODE 0	//Starting node for testing

//-----Serial Functions-----//
void readFile();
const char* getField(char* line, int num);

//-----Parallel Functions-----//
void dijkstra(int numVerticies, int startnode);

//-----Global Integers-----//
int matrix[MAX][MAX];
int thread_count;

int main(int argc, char* argv[]) 
{
//	thread_count = strtol(argv[1], NULL, 10);
	printf("The adjacency matrix:\n");
	readFile();
//	dijkstra(VERTICIES, STARTINGNODE);
	printf("\n");
	
	return 0;
}

void readFile() 
{
	printf("Reading file\n");
	char line[80], field1[32], field2[32];
	FILE *fp = fopen("as20000102.txt", "r");
	
	if (fp == NULL)
	{
		printf("Could not open the file\n");
		exit(0);
	}
	
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		//File needs to be modified else errors are thrown
		sscanf(line, "%s%s", field1, field2);
		printf("%s %s\n", field1, field2);
	}
}

void dijkstra(int n,int startnode)
{
	int cost[MAX][MAX],distance[MAX],pred[MAX];
	int visited[MAX],count,mindistance,nextnode,i,j;

	//pred[] stores the predecessor of each node
	//count gives the number of nodes seen so far
	//create the cost matrix
	# pragma omp parallel for num_threads(thread_count)
	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			if(matrix[i][j]==0)
				cost[i][j]=INFINITY;
			else
				cost[i][j]=matrix[i][j];

	//initialize pred[],distance[] and visited[]
	# pragma omp parallel for num_threads(thread_count)
	for(i=0;i<n;i++)
	{
		distance[i]=cost[startnode][i];
        pred[i]=startnode;
		visited[i]=0;
	}

	distance[startnode]=0;
	visited[startnode]=1;
	count=1;

	while(count<n-1)
	{
		mindistance=INFINITY;
		
		//nextnode gives the node at minimum distance
		# pragma omp parallel for num_threads(thread_count) private(mindistance, nextnode)
		for(i=0;i<n;i++)
			if(distance[i]<mindistance&&!visited[i])
			{
				mindistance=distance[i];
				nextnode=i;
			}

			//check if a better path exists through nextnode            
			visited[nextnode]=1;
			for(i=0;i<n;i++)
				if(!visited[i])
					if(mindistance+cost[nextnode][i]<distance[i])
					{
						distance[i]=mindistance+cost[nextnode][i];
						pred[i]=nextnode;
					}
		count++;
	}

	//print the path and distance of each node
	for(i=0;i<n;i++)
		if(i!=startnode)
		{
			printf("\nDistance of node%d=%d",i,distance[i]);
			printf("\nPath=%d",i);
            
			j=i;
			do
			{
				j=pred[j];
				printf("<-%d",j);
			}while(j!=startnode);
	}
}