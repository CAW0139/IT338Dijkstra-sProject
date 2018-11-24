#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int numV, *todo, numNodes, chunk;
int start, end, myNode;
unsigned maxInt, localMin[2], globalMin[2], **graph, *minDistance;

double startTime, endTime;

void init()
{
	int i, j, buf;
	FILE *fileptr;
	fileptr = fopen("matrix.txt", "r");
	
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &numNodes);
	MPI_Comm_rank(MPI_COMM_WORLD, &myNode);

	fscanf(fileptr, "%d", &numV);

	chunk = numV/ numNodes;
	start = myNode * chunk;
	end = start + chunk - 1;

	maxInt = -1 >> 1;
	graph = malloc(chunk * numV * sizeof(int));
	minDistance = malloc(numV * sizeof(int));
	todo = malloc(numV * sizeof(int));
		
	for(i = 0; i < start; i++)
	{
		for(j = 0; j < numV; j++)	
			fscanf(fileptr, "%d", &buf);
	}
	for(i = 0; i < chunk; i++)
	{	
		for(j = 0; j < numV; j++)
		{
			fscanf(fileptr, "%d", &buf);
			graph[i*numV+j] = buf;
		}
	}
	for(i = 0; i < numV; i++)
	{
		todo[i] = 1;
		minDistance[i] = maxInt;
	}
	minDistance[0] = 0;
	fclose(fileptr);
}

int main(int argc, char **argv)
{
	int i, j, k;
	init();
	printf("Initialized\n");
	if(myNode == 0)
	{
		startTime = MPI_Wtime();
	}
	
	for(k = 0; k < numV; k++)
	{
		for(i = start; i <= end; i++)
		{
			if(todo[i] && minDistance[i] < maxInt)
			{
				localMin[0] = minDistance[i];
				localMin[1] = i;
			}
		}
		MPI_Reduce(localMin, globalMin, 1, MPI_2INT, MPI_MINLOC, 0, MPI_COMM_WORLD);
		MPI_Bcast(globalMin, 1, MPI_2INT, 0, MPI_COMM_WORLD);
		todo[globalMin[1]] = 0;
		for(j = 0; j < chunk; j++)
		{
			if(globalMin[0] + graph[globalMin[1] * chunk + j] < minDistance[j + (myNode * chunk)])
			{
				minDistance[j + (myNode * chunk)] = globalMin[0] + graph[globalMin[1] * chunk +j];
			}
		}
	}
		
	MPI_Gather(minDistance + start, chunk, MPI_INT, minDistance, chunk, MPI_INT, 0, MPI_COMM_WORLD);

	if(myNode == 0)
	{
		endTime = MPI_Wtime();
		for(k = 1; k < numV; k++)
		{
			printf("node[%d] = %u\n", k, minDistance[k]);
		}
		printf("Time elapsed: %f\n", (float)(endTime - startTime));
	}
	MPI_Finalize();
	return 0;
}

