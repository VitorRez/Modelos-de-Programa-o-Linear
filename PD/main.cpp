#include <bits/stdc++.h>
#include <ilcplex/ilocplex.h>

using namespace std;
ILOSTLBEGIN

#define CPLEX_TIME_LIM 3600

int N, M; //Quantidade de tarefas e agentes
vector<vector<int>> cost; //Matriz de custo de designação

void cplex(){
    IloEnv env;

    //Variaveis PD
    int i, j;
    int numberVar = 0;
    int numberRes = 0;

    //Modelagem
    //Variaveis de decisao
    IloArray<IloBoolVarArray> x(env, N);
    for(i = 0; i < N; i++){
        x[i] = IloBoolVarArray(env, N);
        for(j = 0; j < N; j++){
            x[i][j] = IloBoolVar(env);
            numberVar++;
        }
    }

    //Definicao do ambiente modelo

    IloModel model(env);

    //Funcao objetivo

    IloExpr sum(env);
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            printf("i: %d j: %d\n", i, j);
            sum += cost[i][j] * x[i][j];
        }
    }
    model.add(IloMinimize(env, sum));

    //Restricoes
    //Cada agente pode ser designado a uma tarefa
    
    for (i = 0; i < N; i++) {
        IloExpr sum(env);
        for (j = 0; j < N; j++) {
            sum += x[i][j];
        }
        model.add(sum == 1); // Cada agente designa uma tarefa
        numberRes++;
        sum.end();
    }

    //Cada tarefa deve ser designada a um agente

    for (j = 0; j < N; j++) {
        IloExpr sum(env);
        for (i = 0; i < N; i++) {
            sum += x[i][j];
        }
        model.add(sum == 1);
        numberRes++;
        sum.end();
    }

    //Execucao
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

    // Verificando o status da solução
    bool sol = true;
    switch (cplex.getStatus()) {
        case IloAlgorithm::Optimal:
            status = "Otima";
            break;
        case IloAlgorithm::Feasible:
            status = "Viavel";
            break;
        default:
            status = "Sem solução";
            sol = false;
    }

    cout << "\nStatus da solução: " << status << endl;

    // Exibindo resultados
    if (sol) {
        objValue = cplex.getObjValue();
        runTime = difftime(timer2, timer);

        cout << "Designações: " << endl;
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                if (cplex.getValue(x[i][j]) > 0.5) {
                    printf("Agente %d designado à tarefa %d\n", i + 1, j + 1);
                }
            }
        }

        cout << "Valor da função objetivo = " << objValue << endl;
        printf("Tempo de execução: %.6lf segundos.\n\n", runTime);
    } else {
        printf("Sem solução viável!\n");
    }

    // Liberando memória
    cplex.end();
    env.end();

}

int main() {
    // Leitura dos dados
    cin >> N >> M;
    cost.resize(N, vector<int>(N));

    // Leitura da matriz de custos
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            cin >> cost[i][j];
        }
    }

    // Verificação da leitura dos dados
    printf("Verificação da leitura dos dados:\n");
    printf("Número de agentes/tarefas: %d\n", N);
    printf("Matriz de custos (agentes x tarefas):\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", cost[i][j]);
        }
        printf("\n");
    }

    // Chamando a função de solução usando CPLEX
    cplex();

    return 0;
}