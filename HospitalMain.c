// Roberto Brostel Barroso 16899695


#include "TADS.h"

void menu() {
    printf("\n=== SISTEMA DE PRONTO SOCORRO ===\n");
    printf("1. Registrar paciente\n");
    printf("2. Remover paciente\n");
    printf("3. Listar pacientes\n");
    printf("4. Buscar paciente por ID\n");
    printf("5. Mostrar fila de espera\n");
    printf("6. Dar alta ao paciente\n");
    printf("7. Sair\n");
    printf("Escolha uma opcao: ");
}

int lerIntSeguro() {
    int x;
    while (scanf("%d", &x) != 1) {
        printf("Entrada invalida. Digite novamente: ");
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    return x;
}

void lerLinha(char *buffer, int tam) {
    int c;
    while ((c = getchar()) == '\n'); // consome quebras anteriores
    if (c == EOF) {
        buffer[0] = '\0';
        return;
    }
    int i = 0;
    buffer[i++] = (char) c;
    while (i < tam - 1) {
        c = getchar();
        if (c == '\n' || c == EOF) break;
        buffer[i++] = (char) c;
    }
    buffer[i] = '\0';
}

int main() {
    ListaPacientes *lista = criarLista();
    FilaPrioritaria *fila = criarFilaPrioritaria();

    /* tenta carregar dados anteriores */
    carregarDados(lista, fila);

    int opcao;

    do {
        menu();
        opcao = lerIntSeguro();

        switch (opcao) {
            case 1: {
                /* Registrar paciente */
                int id;
                char nome[MAX_NOME];
                int prioridade;

                printf("ID do paciente: ");
                id = lerIntSeguro();

                if (buscarPaciente(lista, id) != NULL) {
                    printf("Ja existe paciente com esse ID!\n");
                    break;
                }

                printf("Nome do paciente: ");
                lerLinha(nome, MAX_NOME);

                printf("Prioridade (1 - Emergencia, 2 - Muito urgente,\n"
                       "            3 - Urgente, 4 - Pouco urgente,\n"
                       "            5 - Nao urgente): ");
                prioridade = lerIntSeguro();
                if (prioridade < 1) prioridade = 1;
                if (prioridade > 5) prioridade = 5;

                Paciente *p = criarPaciente(id, nome, prioridade);
                if (!inserirPaciente(lista, p)) {
                    /* isso só aconteceria se outro com mesmo ID surgisse aqui */
                    printf("Erro: nao foi possivel inserir paciente (ID repetido).\n");
                    liberarPaciente(p);
                    break;
                }
                enfileirarPrioridade(fila, p);

                printf("Paciente %s (ID %d, prioridade %d) registrado e adicionado a fila.\n",
                       p->nome, p->id, p->prioridade);
                break;
            }

            case 2: {
                /* Remover paciente dos registros (se nao estiver na fila) */
                int id;
                printf("ID do paciente a remover: ");
                id = lerIntSeguro();

                Paciente *p = buscarPaciente(lista, id);
                if (!p) {
                    printf("Paciente nao encontrado.\n");
                    break;
                }

                if (pacienteNaFila(fila, id)) {
                    printf("Paciente ainda esta aguardando atendimento. Nao e possivel remover.\n");
                    break;
                }

                if (removerPaciente(lista, id)) {
                    printf("Paciente removido dos registros do hospital.\n");
                } else {
                    printf("Nao foi possivel remover o paciente.\n");
                }
                break;
            }

            case 3: {
                /* Listar pacientes */
                listarPacientes(lista);
                break;
            }

            case 4: {
                /* Buscar paciente por ID */
                int id;
                printf("ID do paciente: ");
                id = lerIntSeguro();

                Paciente *p = buscarPaciente(lista, id);
                if (!p) {
                    printf("Paciente nao encontrado.\n");
                } else {
                    printf("Dados do paciente:\n");
                    printf("ID: %d\n", p->id);
                    printf("Nome: %s\n", p->nome);
                    printf("Prioridade: %d\n", p->prioridade);
                    mostrarPilha(p->historico);
                }
                break;
            }

            case 5: {
                /* Mostrar fila de espera */
                mostrarFilaPrioritaria(fila);
                break;
            }

            case 6: {
                /* Dar alta ao paciente (tirar da fila, mas manter nos registros) */
                Paciente *p = desenfileirarPrioridade(fila);
                if (!p) {
                    printf("Nao ha pacientes na fila.\n");
                } else {
                    printf("Paciente chamado para atendimento e dado como alta da fila:\n");
                    printf("ID: %d | Nome: %s | Prioridade: %d\n",
                           p->id, p->nome, p->prioridade);
                }
                break;
            }

            case 7: {
                /* Sair: salvar e encerrar */
                printf("Salvando dados...\n");
                salvarDados(lista, fila);
                printf("Encerrando o sistema.\n");
                break;
            }

            default:
                printf("Opcao invalida!\n");
                break;
        }

    } while (opcao != 7);

    /* Libera memoria */
    liberarFilaPrioritaria(fila);
    liberarLista(lista);

    return 0;
}
