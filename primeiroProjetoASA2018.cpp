/*-------------------------------*/
//Primeiro Projeto de ASA - 2018 //
//Pedro Esteves 83541            //
//José  Carvalho 83495			 //
/*-------------------------------*/
#include <stdlib.h>  
#include <vector>
#include <stdio.h>
#include <algorithm>



int totalVertex,totalEdges;
int totalSCC=0;				//regista o numero de SCC encontradas
int numLigacoesTotal=0;		//regista o numero total de ligacoes entre as SCC
int rsIndex;      			//regista a ultima componente adicionada a stack;
vertex** grafo;				//Grafo
int dIndex = 0;				//Temporizador, armazena o tempo de descoberta mais recente.
int sccId;					//Regista o id duma componente(vertice mais baixo da componente)
std::vector<int> sccs;		//Armazena os ids das SCC.
std::vector< std::vector<int> > vertexsBySCCs;  //armazena os vertices por scc.
												//quando se faz pop no tarjanVisit();


/*--Funcoes do Projeto--*/
vertex** inicializa();
void dfs(vertex** grafo,bool visited[],int lowkey[],int cv[]);
void tarjanVisit(int vertexID,vertex** grafo,bool visited[],
int* lowkey,bool stack[],int rs[],int dtime[]);
void atualizaLowKeys(std::vector<int>verticesdaMESMASCC,int lowkey[],int sccId);
void descobreConnects(int lowkey[]);
std::vector<int> descobreConnectsAux(std::vector<int> scc,int lowkey[],bool visited[]);
int min(int a,int b);

/*--Funcoes Auxiliares(Para Debugging) --*/
void printGrafo(vertex** grafo);
void printlowkeys(int lowkey[]);

int main(){

	scanf("%d",&totalVertex);
	scanf("%d",&totalEdges);

	int lowkey[totalVertex];    //Guarda o id da componente de cada vertice
	int rs[totalVertex];       //rs(realStack) armazena os vertices na stack
	grafo= inicializa();
	bool visited[totalVertex];

	//inicialmente nenhum vertice foi visitado
	for(int i = 0; i < totalVertex;i++){
		visited[i] = 0;
	}	
	
	//inicialmente a stack(rs) esta vazia e cada vertice pertence a um scc
	//com o seu id
	for(int i =0; i < totalVertex;i++){ 
		lowkey[i] = i;
		rs[i] = -1;
	}
	dfs(grafo,visited,lowkey,rs);
	return 0;
}

void dfs(vertex** grafo,bool visited[],int lowkey[],int rs[]){

	bool stack[totalVertex];
	int dtime[totalVertex];

	for(int i = 0; i < totalVertex;i++){
		visited[i] = 0;
	}	
	
	rsIndex = -1;

	for(int vertexID= 0; vertexID < totalVertex;vertexID++){
		if(visited[vertexID] == false){ 
			tarjanVisit(vertexID,grafo,visited, lowkey,stack,rs,dtime);
		}
	}
	printlowkeys(lowkey);
	descobreConnects(lowkey);
}

/*
-->vertexId: id do Vertice a ser iterado.
-->grafo: Grafo usando representacao de lista de adjacencias.
-->lowkey: Indica a que scc pertence um vertice(Inicialmente cada vertice tem a sua scc).
-->stack: Se o vertice esta NAO esta na stack, significa que:
	-Ja foi processado e tem a sua SCC(no lowkey).
	-Ainda nao foi descoberto.
-->visited:Indica se o vertice ja  foi o nao descoberto.
-->rs:(rs=Real Stack)Estrutura onde estao efetivamente guardados os vertices.
-->dtime: regista o tempo de descoberta de um vertice.	
*/

void atualizaLowKeys(std::vector<int>verticesdaMESMASCC,int lowkey[],int sccId){
	for(int i = verticesdaMESMASCC.size() -1; i >=0;i--){
		lowkey[verticesdaMESMASCC[i]] = sccId;
	}
}

void tarjanVisit(int vertexID,vertex** grafo,bool visited[], int* lowkey,bool stack[], int rs[],int dtime[]){
	visited[vertexID] = true;
	std::vector<int> verticesQuePertencemAMESMASCC;

	rsIndex++;				  			//indice da stack.(aumenta ao adicionar vertices)
	dIndex++;				  			//altura em que descobri o vertice.(discoveryIndex)
	dtime[vertexID] = dIndex; 			//o tempo de descoberta do vertice
	lowkey[vertexID] = dIndex;

	stack[vertexID] = true; 			//indico que o vertex esta na stack;
	rs[rsIndex] = vertexID; 			//coloco o vertex efetivamente na stack;

	vertex* ptr = grafo[vertexID];

	//verifico se ha vizinhos nao visitados
	for(;ptr!=NULL;ptr=ptr->next){
		int newVertexId = ptr->vertexID;

		//Caso o vizinho nao tenha sido visitado,visito-o
		if(visited[newVertexId] == false){ 
			tarjanVisit(newVertexId,grafo,visited,lowkey,stack,rs,dtime);
		}

		if(stack[newVertexId] == true)
			lowkey[vertexID] = min(lowkey[vertexID],lowkey[newVertexId]);
	}

	// fechar a componente
	if(lowkey[vertexID] == dtime[vertexID]){
		sccId = vertexID;
		std::vector<int> temp ;
		while(rsIndex>=0){ 

			int atualvertex = rs[rsIndex];					//atualiza vertice na stack
			stack[atualvertex] = false;	 					//indica que o vertex nao esta na stack
			temp.push_back(atualvertex);

			verticesQuePertencemAMESMASCC.push_back(rs[rsIndex]); 

			rs[rsIndex] = -1;			 				   //tiro o vertex na stack

			if (atualvertex < sccId) sccId = atualvertex; //atualiza id da componente
			rsIndex--;

			if(lowkey[atualvertex] == dtime[atualvertex]) break;
		}
		totalSCC++;
		atualizaLowKeys(verticesQuePertencemAMESMASCC,lowkey,sccId);
		vertexsBySCCs.push_back(temp);				//adiciona uma scc(os vertices que lhe pertencem)
		sccs.push_back(sccId);						
	}  
}

int min(int a,int b){
	if(a < b)
		return a;
	return b;
}

/*Associa o id da SCC
e o id mais pequeno do node que faz parte dela*/
//-> Id do node mais pequeno que constitui a componente.
//-> O index e a componente a que pertence o node.
//->sccsTraduzidos e para ter um acesso eficiente ao valor da scc.


void descobreConnects(int lowkey[]){
	bool visited[totalVertex];

	for(int i = 0; i < totalVertex; i++){
		visited[i] = false;
	}
	std::vector< std::vector<int> > allConnecs;

	//Para cada componente fortemente ligada quero ver todas as ligacoes
	for(int i = 0; i < totalSCC;i++){ 
		allConnecs.push_back(descobreConnectsAux(vertexsBySCCs[i],lowkey,visited));
	}
	sort(allConnecs.begin(), allConnecs.end());

	printf("%d\n",totalSCC); 

	int y; 
	printf("%d\n",numLigacoesTotal);
	for(int i = 0; i < totalSCC;i++){ 
		y = 0; 
		if(allConnecs[i].size() > 2){ 
			while(allConnecs[i][y+1]+1 != 0){
				printf("%d %d\n",allConnecs[i][0]+1,allConnecs[i][y+1]+1);
				y++;
			}
		}
	}
}

//sccsVertexs = vertices que corresponde a uma determinada scc.
//lowkey = vector que indica a scc a que pertencem os vertices.(importante para checkar neibors)
//visited-> Para saber se um dado vertice ja foi visitado ou nao 
std::vector<int> descobreConnectsAux(std::vector<int> sccVertexs,int lowkey[],bool visited[]){
	vertex* ptr;
	int sccNeibor = -1;
	int scc = lowkey[sccVertexs[0]];		//Regista scc a ser iterada
	std::vector<int> connecs;
	for(unsigned int x = 0; x < sccVertexs.size(); x++){ 

		ptr=grafo[sccVertexs[x]];
		visited[x] = true;

		//itero cada vizinho
		for(; ptr!=NULL;ptr=ptr->next){

			//se o vizinho nao pertencer a scc atual registo a sua scc
			if(lowkey[ptr->vertexID] != scc){
			 	sccNeibor = lowkey[ptr->vertexID];
				
			 	bool repetido = false;

				for(unsigned int i = 0; i < connecs.size(); i++){ 
					if(connecs[i] == sccNeibor){
						repetido = true;
						break;
					}
				}
				if(repetido == false){ 
			 		//adiciona a scc
					connecs.push_back(sccNeibor);
					numLigacoesTotal++;
				}
			}
		}
	}
	std::sort (connecs.begin(), connecs.end());
	connecs.insert(connecs.begin(),scc); //serve para marcar qual a scc do vector
	connecs.push_back(-1); 				//funciona como limite quando tiver a imprimir;
	return connecs;
}

//Funcao auxiliar(para debugging)
void printGrafo(vertex** grafo){
	for(int i = 0; i < totalVertex; i++){
		printf("O Vertice %d tem arestas para:" "\n",i);
	      vertex *ptr =grafo[i];
		  for(; ptr != NULL; ptr = ptr->next) {
		  	printf("  %d\n",ptr->vertexID);
		  }
	}
}

void printlowkeys(int lowkey[]){
	for(int i = 0; i < totalVertex;i++){
	}
}

vertex** inicializa(){
	 //aloca memoria para o grafo(lista de adjacencias)
	 vertex** grafo = (vertex**) malloc(sizeof(vertex*) * totalVertex);

	 int origem,destino;

	 //adiciona todas as arestas ao grafo
	 for(int i = 0; i < totalEdges;i++){
	 	scanf("%d",&origem);
	 	scanf("%d",&destino);
	 	origem--;				//para os vertex comecarem no 0
	 	destino--;				//para os vertex comecarem no 0

	 	vertex* v = (vertex*) malloc(sizeof(vertex));
	 	v->vertexID = destino;
	 	v->next = NULL;

	 	//Caso em que adiciono a primeira aresta a um vertex
	 	if(grafo[origem] == NULL)
	 		grafo[origem] = v; 

	 	//Caso em que o vertex ja tem arestas
	 	else{
	 		v->next = grafo[origem];
	 		grafo[origem] = v;
	 	}
	 }
	 return grafo;
}