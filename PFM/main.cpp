#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

#define CPLEX_TIME_LIM 3600

struct aresta {
    int i, j, c;
};

int N, M, s, d; // quantidade de vertices, arestas, vertice de origem e destino
vector<vector<int>> V; // matriz de adjacencia
vector<aresta> A; // conjunto de arestas

void cplex() {
    IloEnv env;

    // Variáveis
    int i, j;
    int numberVar = 0;
    int numberRes = 0;

    // Variáveis de decisão : fluxo em cada aresta
    IloArray<IloNumVarArray> x(env, N);
    for (i = 0; i < N; i++) {
        x[i] = IloNumVarArray(env, N, 0, IloInfinity);
        for (j = 0; j < N; j++) {
            if (V[i][j]) {
                // Inicializa a variável de fluxo com limites de capacidade
                x[i][j] = IloNumVar(env, 0, V[i][j]);
                numberVar++;
            }
        }
    }

    // Definição do ambiente modelo
    IloModel model(env);

    // Função objetivo: maximizar o fluxo total que sai da origem s
    IloExpr sum(env);
    for (j = 0; j < N; j++) {
        if (V[s][j]) {
            sum += x[s][j]; // fluxo saindo da origem
        }
    }
    model.add(IloMaximize(env, sum));

    // Conservação de fluxo para todos os nós intermediários
    for (i = 0; i < N; i++) {
        if (i != s && i != d) {
            IloExpr in(env), out(env);
            for (j = 0; j < N; j++) {
                if (V[i][j]) out += x[i][j]; // fluxo saindo do nó i
                if (V[j][i]) in += x[j][i];  // fluxo entrando no nó i
            }
            model.add(in == out); // fluxo entrando = fluxo saindo
            in.end();
            out.end();
            numberRes++;
        }
    }

    // Restrições de capacidade das arestas
    for (i = 0; i < M; i++) {
        model.add(x[A[i].i][A[i].j] <= A[i].c);
        numberRes++;
    }

    // Execução
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

        for (i = 0; i < M; i++) {
            if (cplex.getValue(x[A[i].i][A[i].j]) > 0) {
                printf("Aresta (%d -> %d): Fluxo = %.2f\n", A[i].i, A[i].j, cplex.getValue(x[A[i].i][A[i].j]));
            }
        }

        cout << "Valor da funcao objetivo (fluxo maximo) = " << objValue << endl;
        printf("Tempo de execucao: %.6lf segundos.\n\n", runTime);

    } else {
        printf("Sem solucao viavel.\n");
    }

    cplex.end();
    env.end();
}

int main() {
    // Leitura de dados
    cin >> N >> M >> s >> d;
    A.resize(M);
    V.resize(N, vector<int>(N, 0));

    // Leitura das arestas
    for (int i = 0; i < M; i++) {
        aresta a;
        cin >> a.i >> a.j >> a.c;
        A[i] = a;
        V[a.i][a.j] = a.c; // Define a capacidade na matriz de adjacência
    }

    cplex();

    return 0;
}

        