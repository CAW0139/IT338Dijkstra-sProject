#include<stdio.h>
#define max 20

int adj[max][max];
int n;
int main()
{
        int max_edges,i,j,origin,destin;
        char graph_type;
	char * fileName;
        
	printf("Enter file name of input: ");
        scanf("%s",fileName);

        fflush(stdin);

	FILE *fileptr;
	fileptr = fopen(fileName, "r");
	fscanf(fileptr, "%c\n", &graph_type);
	fscanf(fileptr, "%d %d\n", &n, &max_edges); 

	printf("Number of Nodes: %d \nNumber of Edges: %d\nType of Graph: %c\n", n, max_edges, graph_type);	
//	int adj[n][n];	
	for(i=1;i<=max_edges;i++)
        {     
                fscanf(fileptr, "%d %d\n",&origin,&destin);
       //       	printf("Pair: %d %d\n", origin, destin); 
		 if( (origin==0) && (destin==0) )
                        break;
                if( origin >= n || destin >= n || origin<0 || destin<0)
                {
                        printf("Invalid edge!\n");
                        i--;
                }
                else
                {
                        adj[origin][destin]=1;
                        if( graph_type=='u')
                                adj[destin][origin]=1;
                }
        }/*End of for*/

        printf("The adjacency matrix is :\n");
        for(i=0;i<n;i++)
        {
                for(j=0;j<n;j++)
                        printf("%4d",adj[i][j]);
                printf("\n");
        }
	return 0;
}
