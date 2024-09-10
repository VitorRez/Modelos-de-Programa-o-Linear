

#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN //MACRO - "using namespace" for ILOCPEX

//CPLEX Parameters
#define CPLEX_TIME_LIM 3600 //3600 segundos
//#define CPLEX_COMPRESSED_TREE_MEM_LIM 8128 //8GB
//#define CPLEX_WORK_MEM_LIM 4096 //4GB
//#define CPLEX_VARSEL_MODE 0
/*
* VarSel Modes:
* -1 Branch on variable with minimum infeasibility
* 0 Branch variable automatically selected
* 1 Branch on variable with maximum infeasibility
* 2 Branch based on pseudo costs
* 3 Strong branching
* 4 Branch based on pseudo reduced costs
*
* Default: 0
*/

//Struct para um item da mochila 01
struct aresta{
    int origem, destino, capacidade;
};

struct no {
	int numero, capacidade;
};

//Conjuntos do Problema
vector<vector<int>> V; //Matriz de adjacencia
vector<aresta> arestas; //Conjunto de arestas
vector<no> origens;
vector<no> destinos;
int N, M, o, d; //Numero de vertices, arestas, Origens e Destinos

void cplex(){
    //CPLEX
	IloEnv env; //Define o ambiente do CPLEX

	//Variaveis --------------------------------------------- 
	int i, j, k; //Auxiliares
	int numberVar = 0; //Total de Variaveis
	int numberRes = 0; //Total de Restricoes


	//---------- MODELAGEM ---------------

	//Definicao - Variaveis de decisao: Unidades transportadas de i para j
	IloArray<IloNumVarArray> x(env, N);
    for (i = 0; i < N; i++) {
        x[i] = IloNumVarArray(env, N, 0, IloInfinity);
        for (j = 0; j < N; j++) {
            if (V[i][j]) {            
                x[i][j] = IloNumVar(env, 0, IloInfinity);
                numberVar++;
            }
        }
    }
	

	//Definicao do ambiente modelo ------------------------------------------
	IloModel model ( env );
	
	//Definicao do ambiente expressoes, para os somatorios ---------------------------------
	//Nota: Os somatorios podem ser reaproveitados usando o .clear(),
	//com excecao de quando existe mais de um somatorio em uma mesma restricao
	IloExpr sum(env); /// Expression for Sum
	IloExpr sum2(env); /// Expression for Sum2

	//FUNCAO OBJETIVO ---------------------------------------------
	sum.clear();
	for( i = 0; i < M; i++ ){
		sum += (arestas[i].capacidade * x[arestas[i].origem][arestas[i].destino]);
		//printf("Arestas (%d -> %d) Capacidade: %d\n", arestas[i].origem, arestas[i].destino, arestas[i].capacidade);
	}
	
	//Modelo de Minimizacao
	model.add(IloMinimize(env, sum)); //Minimizacao

	//RESTRICOES ---------------------------------------------	
		 
	//R1 - Restrições da origem
	sum.clear();
	for( i = 0; i < o; i++ ){
		for(j = 0; j < M; j++){
			if(arestas[j].origem == origens[i].numero){
				sum += x[arestas[j].origem][arestas[j].destino];
				
			}	
		}
		model.add(sum <= origens[i].capacidade);
		sum.clear(); 
		numberRes++;
	}

	//Restrições do destino

	sum.clear();
	for( i = 0; i < d; i++ ){
		for(j = 0; j < M; j++){
			if(arestas[j].destino == destinos[i].numero){
				sum += x[arestas[j].origem][arestas[j].destino];
			}	
		}
		model.add(sum == destinos[i].capacidade);
		sum.clear(); 
		numberRes++;
	}
	



	//------ EXECUCAO do MODELO ----------
	time_t timer, timer2;
	IloNum value, objValue;
	double runTime;
	string status;
	
	//Informacoes ---------------------------------------------	
	printf("--------Informacoes da Execucao:----------\n\n");
	printf("#Var: %d\n", numberVar);
	printf("#Restricoes: %d\n", numberRes);
	cout << "Memory usage after variable creation:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
	
	IloCplex cplex(model);
	cout << "Memory usage after cplex(Model):  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

	//Setting CPLEX Parameters
	cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM);
	//cplex.setParam(IloCplex::TreLim, CPLEX_COMPRESSED_TREE_MEM_LIM);
	//cplex.setParam(IloCplex::WorkMem, CPLEX_WORK_MEM_LIM);
	//cplex.setParam(IloCplex::VarSel, CPLEX_VARSEL_MODE);

	time(&timer);
	cplex.solve();//COMANDO DE EXECUCAO
	time(&timer2);
	
	//cout << "Solution Status: " << cplex.getStatus() << endl;
	//Results
	bool sol = true;
	/*
	Possible Status:
	- Unknown	 
	- Feasible	 
	- Optimal	 
	- Infeasible	 
	- Unbounded	 
	- InfeasibleOrUnbounded	 
	- Error
	*/
	switch(cplex.getStatus()){
		case IloAlgorithm::Optimal: 
			status = "Optimal";
			break;
		case IloAlgorithm::Feasible: 
			status = "Feasible";
			break;
		default: 
			status = "No Solution";
			sol = false;
	}

	cout << endl << endl;
	cout << "Status da FO: " << status << endl;

	if(sol){ 

		//Results
		//int Nbin, Nint, Ncols, Nrows, Nnodes, Nnodes64;
		objValue = cplex.getObjValue();
		runTime = difftime(timer2, timer);
		//Informacoes Adicionais
		//Nbin = cplex.getNbinVars();
		//Nint = cplex.getNintVars();
		//Ncols = cplex.getNcols();
		//Nrows = cplex.getNrows();
		//Nnodes = cplex.getNnodes();
		//Nnodes64 = cplex.getNnodes64();
		//float gap; gap = cplex.getMIPRelativeGap();
		
		cout << "Variaveis de decisao: " << endl;
		for (i = 0; i < M; i++) {
            if (cplex.getValue(x[arestas[i].origem][arestas[i].destino]) > 0) {
                printf("Aresta (%d -> %d): Valor = %f\n", arestas[i].origem, arestas[i].destino, cplex.getValue(x[arestas[i].origem][arestas[i].destino]));
            }
        }


		printf("\n");
		
		cout << "Funcao Objetivo Valor = " << objValue << endl;
		printf("..(%.6lf seconds).\n\n", runTime);

	}else{
		printf("No Solution!\n");
	}

	//Free Memory
	cplex.end();
	sum.end();
	sum2.end();

	cout << "Memory usage before end:  " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;
	env.end();
}

int main(){

	//Leitura dos dados:
	//A partir de um arquivo (in.txt)
	int i;
	cin >> N >> M >> o >> d;
	arestas.resize(M);
	V.resize(N, vector<int>(N, 0));
	origens.resize(o);
	destinos.resize(d);
	for(i=0; i<o; i++){
		cin >> origens[i].numero >> origens[i].capacidade;
	}

	for(i=0; i<d; i++){
		cin >> destinos[i].numero >> destinos[i].capacidade;
	}

	for(i=0; i<M; i++){
		cin >> arestas[i].origem >> arestas[i].destino >> arestas[i].capacidade;
		V[arestas[i].origem][arestas[i].destino] = 1;
		printf("Origem e destinos (arestas): %d %d\n", arestas[i].origem, arestas[i].destino);

	}

	printf("Verificacao da leitura dos dados:\n");
	printf("Num. Vertices: %d\n", N);
	printf("Num. Arestas: %d\n", M);
	printf("Origens - id: capacidade\n");
    for(i=0; i<o; i++)
        printf("%d: %d\n", origens[i].numero, origens[i].capacidade);
	printf("Destinos - id: capacidade\n");
    for(i=0; i<d; i++)
        printf("%d: %d\n", destinos[i].numero, destinos[i].capacidade);
	printf("\n");

	cplex();

    return 0;
}