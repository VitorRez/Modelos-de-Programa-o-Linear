#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

#define CPLEX_TIME_LIM 3600

struct aresta{
    int i, j, c;
};

int N, M, s, d; //quantidade de vertices, arestas, vertice de origem e destino
vector<vector<int>> V; //matriz de adjecencia
vector<aresta> A; //conjunto de arestas

void cplex(){
    IloEnv env;

    //Variaveis
    int i, j, c;
    int numberVar = 0;
    int numberRes = 0;

    //Variaveis de decisão : vertices
    IloArray<IloBoolVarArray> x(env, N);
    for(i = 0; i < N; i++){
        x[i] = IloBoolVarArray(env, N);
        for(j = 0; j < N; j++){
            if(V[i][j]){
                x[i][j] = IloBoolVar(env);
                numberVar++;
            }
        }
    }
    
    //Definição do ambiente modelo
    IloModel model(env);

    //Função objetivo
    IloExpr sum(env);
    for(i = 0; i < M; i++){
        sum += A[i].c * x[A[i].i][A[i].j];
    }
    model.add(IloMinimize(env, sum));

    //Restricoes
    for(i = 0; i < N; i++){
        IloExpr in(env), out(env);

        for(j = 0; j < N; j++){
            if(V[i][j]){
                in += x[i][j];
                out += x[j][i];
            }
        }

        if(i == s){
            model.add(in - out == 1);
        } else if(i == d){
            model.add(in - out == -1);
        } else {
            model.add(in - out == 0);
        }
        numberRes++;
        out.end();
        in.end();
    }
    
    //Execução
    time_t timer, timer2;
    IloNum objValue;
    double runTime;
    string status;

    printf("--------Informações da Execução:----------\n\n");
    printf("#Variáveis: %d\n", numberVar);
    printf("#Restrições: %d\n", numberRes);
    cout << "Uso de memória após criação das variáveis: " << env.getMemoryUsage() / (1024. * 1024.) << " MB" << endl;

    IloCplex cplex(model);
    cplex.setParam(IloCplex::TiLim, CPLEX_TIME_LIM); // Definindo limite de tempo

    time(&timer);
    cplex.solve();
    time(&timer2);

    bool sol = true;
    switch(cplex.getStatus()) {
        case IloAlgorithm::Optimal:
            status = "Otima";
            break;
        case IloAlgorithm::Feasible:
            status = "Viavel";
            break;
        default:
            status = "Sem solucao";
            sol = false;
    }

    cout << "\nStatus da solucao: " << status << endl;

    if(sol){
        objValue = cplex.getObjValue();
        runTime = difftime(timer2, timer);

        for(i = 0; i < M; i++){
            if(cplex.getValue(x[A[i].i][A[i].j]) > 0.5){
                printf("Aresta (%d - > %d) com distancia %d\n", A[i].i, A[i].j, A[i].c);
            }
        }

        cout << "Valor da função objetivo = " << objValue << endl;
        printf("Tempo de execução: %.6lf segundos.\n\n", runTime);

    } else {
        printf("Sem solucao viavel.\n");
    }

    cplex.end();
    env.end();
}

int main(){
    //Leitura de dados
    cin >> N >> M >> s >> d;
    A.resize(M);
    V.resize(N, vector<int>(N, 0));

    //Leitura das arestas
    for(int i = 0; i < M; i++){
        aresta a;
        cin >> a.i >> a.j >> a.c;
        A[i] = a;
        V[a.i][a.j] = 1;
    }

    cplex();

    return 0;
}

