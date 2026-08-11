#ifndef TADS_H
#define TADS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NOME   100
#define MAX_DESC   100

/* ======================= PILHA (HISTÓRICO) ======================= */

typedef struct nodo_pilha {
    char descricao[MAX_DESC];
    struct nodo_pilha *prox;
} NodoPilha;

typedef struct {
    NodoPilha *topo;
} Pilha;

Pilha *criarPilha() {
    Pilha *p = (Pilha *) malloc(sizeof(Pilha));
    if (!p) {
        fprintf(stderr, "Erro ao alocar pilha.\n");
        exit(1);
    }
    p->topo = NULL;
    return p;
}

void push(Pilha *p, const char *descricao) {
    if (!p) return;
    NodoPilha *novo = (NodoPilha *) malloc(sizeof(NodoPilha));
    if (!novo) {
        fprintf(stderr, "Erro ao alocar nodo da pilha.\n");
        exit(1);
    }
    strncpy(novo->descricao, descricao, MAX_DESC - 1);
    novo->descricao[MAX_DESC - 1] = '\0';
    novo->prox = p->topo;
    p->topo = novo;
}

void pop(Pilha *p) {
    if (!p || !p->topo) {
        printf("Historico vazio, nada a desfazer.\n");
        return;
    }
    NodoPilha *rem = p->topo;
    p->topo = rem->prox;
    free(rem);
}

void mostrarPilha(Pilha *p) {
    if (!p) return;
    printf("Historico de atendimento (do mais recente para o mais antigo):\n");
    if (!p->topo) {
        printf("(sem procedimentos registrados)\n");
        return;
    }
    NodoPilha *aux = p->topo;
    int i = 1;
    while (aux) {
        printf("%d) %s\n", i, aux->descricao);
        aux = aux->prox;
        i++;
    }
}

void liberarPilha(Pilha *p) {
    if (!p) return;
    NodoPilha *aux = p->topo;
    while (aux) {
        NodoPilha *tmp = aux;
        aux = aux->prox;
        free(tmp);
    }
    free(p);
}

/* ======================= PACIENTE ======================= */

typedef struct paciente {
    int id;
    char nome[MAX_NOME];
    int prioridade;   /* 1 = Emergência ... 5 = Não urgente */
    Pilha *historico;
} Paciente;

Paciente *criarPaciente(int id, const char nome[], int prioridade) {
    Paciente *p = (Paciente *) malloc(sizeof(Paciente));
    if (!p) {
        fprintf(stderr, "Erro ao alocar paciente.\n");
        exit(1);
    }
    p->id = id;
    strncpy(p->nome, nome, MAX_NOME - 1);
    p->nome[MAX_NOME - 1] = '\0';
    p->prioridade = prioridade;
    p->historico = criarPilha();
    return p;
}

void liberarPaciente(Paciente *p) {
    if (!p) return;
    liberarPilha(p->historico);
    free(p);
}

/* ======================= "LISTA" DE PACIENTES (ÁRVORE) ======================= */

typedef struct nodo_paciente {
    Paciente *paciente;
    struct nodo_paciente *esq;
    struct nodo_paciente *dir;
} NodoPaciente;

typedef struct {
    NodoPaciente *raiz;
} ListaPacientes;

ListaPacientes *criarLista() {
    ListaPacientes *l = (ListaPacientes *) malloc(sizeof(ListaPacientes));
    if (!l) {
        fprintf(stderr, "Erro ao alocar lista de pacientes.\n");
        exit(1);
    }
    l->raiz = NULL;
    return l;
}

static NodoPaciente *criarNodoPaciente(Paciente *p) {
    NodoPaciente *n = (NodoPaciente *) malloc(sizeof(NodoPaciente));
    if (!n) {
        fprintf(stderr, "Erro ao alocar nodo de paciente.\n");
        exit(1);
    }
    n->paciente = p;
    n->esq = n->dir = NULL;
    return n;
}

/* Inserção em árvore binária de busca.
   Retorna false se ID já existia. */
static bool inserirPacienteRec(NodoPaciente **raiz, Paciente *p) {
    if (*raiz == NULL) {
        *raiz = criarNodoPaciente(p);
        return true;
    }
    if (p->id < (*raiz)->paciente->id) {
        return inserirPacienteRec(&((*raiz)->esq), p);
    } else if (p->id > (*raiz)->paciente->id) {
        return inserirPacienteRec(&((*raiz)->dir), p);
    } else {
        /* ID já existe */
        return false;
    }
}

bool inserirPaciente(ListaPacientes *lista, Paciente *p) {
    if (!lista || !p) return false;
    return inserirPacienteRec(&(lista->raiz), p);
}

static Paciente *buscarPacienteRec(NodoPaciente *raiz, int id) {
    if (!raiz) return NULL;
    if (id < raiz->paciente->id) return buscarPacienteRec(raiz->esq, id);
    if (id > raiz->paciente->id) return buscarPacienteRec(raiz->dir, id);
    return raiz->paciente;
}

Paciente *buscarPaciente(ListaPacientes *lista, int id) {
    if (!lista) return NULL;
    return buscarPacienteRec(lista->raiz, id);
}

static void listarPacientesRec(NodoPaciente *raiz) {
    if (!raiz) return;
    listarPacientesRec(raiz->esq);
    printf("ID: %d | Nome: %s | Prioridade: %d\n",
           raiz->paciente->id,
           raiz->paciente->nome,
           raiz->paciente->prioridade);
    listarPacientesRec(raiz->dir);
}

void listarPacientes(ListaPacientes *lista) {
    if (!lista) return;
    if (!lista->raiz) {
        printf("Nao ha pacientes cadastrados.\n");
        return;
    }
    listarPacientesRec(lista->raiz);
}

/* Remover paciente da árvore (por ID).
   Retorna true se removeu. */
static NodoPaciente *removerPacienteRec(NodoPaciente *raiz, int id, bool *removido) {
    if (!raiz) return NULL;

    if (id < raiz->paciente->id) {
        raiz->esq = removerPacienteRec(raiz->esq, id, removido);
    } else if (id > raiz->paciente->id) {
        raiz->dir = removerPacienteRec(raiz->dir, id, removido);
    } else {
        /* Encontrou o nó a remover */
        *removido = true;
        if (!raiz->esq && !raiz->dir) {
            liberarPaciente(raiz->paciente);
            free(raiz);
            return NULL;
        } else if (!raiz->esq) {
            NodoPaciente *tmp = raiz->dir;
            liberarPaciente(raiz->paciente);
            free(raiz);
            return tmp;
        } else if (!raiz->dir) {
            NodoPaciente *tmp = raiz->esq;
            liberarPaciente(raiz->paciente);
            free(raiz);
            return tmp;
        } else {
            /* Dois filhos: pegar sucessor em-ordem (menor da direita) */
            NodoPaciente *succParent = raiz;
            NodoPaciente *succ = raiz->dir;
            while (succ->esq) {
                succParent = succ;
                succ = succ->esq;
            }
            /* Copia dados do sucessor */
            Paciente *pTmp = raiz->paciente;
            raiz->paciente = succ->paciente;
            succ->paciente = pTmp;
            /* Remove sucessor (que agora tem os dados antigos) */
            if (succParent->esq == succ)
                succParent->esq = removerPacienteRec(succ, id, removido);
            else
                succParent->dir = removerPacienteRec(succ, id, removido);
        }
    }
    return raiz;
}

bool removerPaciente(ListaPacientes *lista, int id) {
    if (!lista) return false;
    bool removido = false;
    lista->raiz = removerPacienteRec(lista->raiz, id, &removido);
    return removido;
}

static void liberarArvoreRec(NodoPaciente *raiz) {
    if (!raiz) return;
    liberarArvoreRec(raiz->esq);
    liberarArvoreRec(raiz->dir);
    liberarPaciente(raiz->paciente);
    free(raiz);
}

void liberarLista(ListaPacientes *lista) {
    if (!lista) return;
    liberarArvoreRec(lista->raiz);
    free(lista);
}

/* ======================= FILA SIMPLES ======================= */

typedef struct {
    Paciente **dados;
    int inicio;
    int fim;
    int qtd;
    int capacidade;
} Fila;

Fila *criarFila() {
    Fila *f = (Fila *) malloc(sizeof(Fila));
    if (!f) {
        fprintf(stderr, "Erro ao alocar fila.\n");
        exit(1);
    }
    f->capacidade = 10;
    f->dados = (Paciente **) malloc(f->capacidade * sizeof(Paciente *));
    if (!f->dados) {
        fprintf(stderr, "Erro ao alocar dados da fila.\n");
        exit(1);
    }
    f->inicio = 0;
    f->fim = 0;
    f->qtd = 0;
    return f;
}

static void realocarFila(Fila *f) {
    int novaCap = f->capacidade * 2;
    Paciente **novo = (Paciente **) malloc(novaCap * sizeof(Paciente *));
    if (!novo) {
        fprintf(stderr, "Erro ao realocar fila.\n");
        exit(1);
    }
    for (int i = 0; i < f->qtd; i++) {
        int idx = (f->inicio + i) % f->capacidade;
        novo[i] = f->dados[idx];
    }
    free(f->dados);
    f->dados = novo;
    f->capacidade = novaCap;
    f->inicio = 0;
    f->fim = f->qtd;
}

bool filaVazia(Fila *f) {
    return !f || f->qtd == 0;
}

bool enfileirarSimples(Fila *f, Paciente *p) {
    if (!f || !p) return false;
    if (f->qtd == f->capacidade) {
        realocarFila(f);
    }
    f->dados[f->fim] = p;
    f->fim = (f->fim + 1) % f->capacidade;
    f->qtd++;
    return true;
}

Paciente *desenfileirarSimples(Fila *f) {
    if (!f || f->qtd == 0) return NULL;
    Paciente *p = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % f->capacidade;
    f->qtd--;
    return p;
}

void liberarFila(Fila *f) {
    if (!f) return;
    free(f->dados);
    free(f);
}

/* ======================= FILA PRIORITÁRIA ======================= */

typedef struct {
    Fila *niveis[5];  /* 0 -> prioridade 1, 4 -> prioridade 5 */
} FilaPrioritaria;

FilaPrioritaria *criarFilaPrioritaria() {
    FilaPrioritaria *fp = (FilaPrioritaria *) malloc(sizeof(FilaPrioritaria));
    if (!fp) {
        fprintf(stderr, "Erro ao alocar fila prioritaria.\n");
        exit(1);
    }
    for (int i = 0; i < 5; i++) {
        fp->niveis[i] = criarFila();
    }
    return fp;
}

bool enfileirarPrioridade(FilaPrioritaria *fp, Paciente *p) {
    if (!fp || !p) return false;
    if (p->prioridade < 1) p->prioridade = 1;
    if (p->prioridade > 5) p->prioridade = 5;
    int idx = p->prioridade - 1;
    return enfileirarSimples(fp->niveis[idx], p);
}

Paciente *desenfileirarPrioridade(FilaPrioritaria *fp) {
    if (!fp) return NULL;
    for (int i = 0; i < 5; i++) {
        if (!filaVazia(fp->niveis[i])) {
            return desenfileirarSimples(fp->niveis[i]);
        }
    }
    return NULL;
}

void mostrarFilaPrioritaria(FilaPrioritaria *fp) {
    if (!fp) return;
    printf("Fila de espera (da MAIOR para a MENOR prioridade):\n");
    bool vazia = true;
    for (int i = 0; i < 5; i++) {
        Fila *f = fp->niveis[i];
        for (int j = 0; j < f->qtd; j++) {
            int idx = (f->inicio + j) % f->capacidade;
            Paciente *p = f->dados[idx];
            printf("Prio %d - ID: %d - %s\n",
                   p->prioridade, p->id, p->nome);
            vazia = false;
        }
    }
    if (vazia) {
        printf("(fila vazia)\n");
    }
}

bool pacienteNaFila(FilaPrioritaria *fp, int id) {
    if (!fp) return false;
    for (int i = 0; i < 5; i++) {
        Fila *f = fp->niveis[i];
        for (int j = 0; j < f->qtd; j++) {
            int idx = (f->inicio + j) % f->capacidade;
            if (f->dados[idx]->id == id) {
                return true;
            }
        }
    }
    return false;
}

void liberarFilaPrioritaria(FilaPrioritaria *fp) {
    if (!fp) return;
    for (int i = 0; i < 5; i++) {
        liberarFila(fp->niveis[i]);
    }
    free(fp);
}

/* ======================= PERSISTÊNCIA ======================= */
/* Formato de pacientes.bin:
   Para cada paciente:
       int id;
       int prioridade;
       char nome[100];
       int qtdProcedimentos;
       (qtdProcedimentos vezes) char descricao[100];
   Lido até EOF. */

bool salvarDados(ListaPacientes *lista, FilaPrioritaria *fila) {
    /* Salvar pacientes */
    FILE *fp_lista = fopen("pacientes.bin", "wb");
    if (!fp_lista) {
        perror("Erro ao abrir pacientes.bin para escrita");
        return false;
    }

    /* percorrer árvore em-ordem e salvar */
    NodoPaciente *pilha[256];
    int topo = -1;
    NodoPaciente *atual = lista ? lista->raiz : NULL;

    while (atual || topo >= 0) {
        while (atual) {
            pilha[++topo] = atual;
            atual = atual->esq;
        }
        atual = pilha[topo--];

        Paciente *p = atual->paciente;
        fwrite(&p->id, sizeof(int), 1, fp_lista);
        fwrite(&p->prioridade, sizeof(int), 1, fp_lista);
        fwrite(p->nome, sizeof(char), MAX_NOME, fp_lista);

        /* contar procedimentos */
        int count = 0;
        NodoPilha *np = p->historico->topo;
        while (np) {
            count++;
            np = np->prox;
        }
        fwrite(&count, sizeof(int), 1, fp_lista);

        if (count > 0) {
            /* para manter ordem do mais antigo pro mais novo na leitura,
               vamos guardar em um vetor temporário */
            char (*buffer)[MAX_DESC] = malloc(count * sizeof(*buffer));
            int i = 0;
            np = p->historico->topo;
            while (np) {
                strncpy(buffer[i], np->descricao, MAX_DESC);
                buffer[i][MAX_DESC - 1] = '\0';
                np = np->prox;
                i++;
            }
            /* salvar do último para o primeiro -> na leitura vamos dar push na ordem certa */
            for (int j = count - 1; j >= 0; j--) {
                fwrite(buffer[j], sizeof(char), MAX_DESC, fp_lista);
            }
            free(buffer);
        }

        atual = atual->dir;
    }

    fclose(fp_lista);

    /* Salvar fila (apenas IDs + prioridade) */
    FILE *fp_fila = fopen("fila.bin", "wb");
    if (!fp_fila) {
        perror("Erro ao abrir fila.bin para escrita");
        return false;
    }

    for (int i = 0; i < 5; i++) {
        Fila *f = fila->niveis[i];
        for (int j = 0; j < f->qtd; j++) {
            int idx = (f->inicio + j) % f->capacidade;
            Paciente *p = f->dados[idx];
            fwrite(&p->id, sizeof(int), 1, fp_fila);
            fwrite(&p->prioridade, sizeof(int), 1, fp_fila);
        }
    }

    fclose(fp_fila);
    return true;
}

bool carregarDados(ListaPacientes *lista, FilaPrioritaria *fila) {
    /* Carregar pacientes */
    FILE *fp_lista = fopen("pacientes.bin", "rb");
    if (fp_lista) {
        while (1) {
            int id;
            int prioridade;
            char nome[MAX_NOME];
            int count;

            size_t lidos = fread(&id, sizeof(int), 1, fp_lista);
            if (lidos != 1) break;

            if (fread(&prioridade, sizeof(int), 1, fp_lista) != 1) break;
            if (fread(nome, sizeof(char), MAX_NOME, fp_lista) != MAX_NOME) break;
            if (fread(&count, sizeof(int), 1, fp_lista) != 1) break;

            Paciente *p = criarPaciente(id, nome, prioridade);

            for (int i = 0; i < count; i++) {
                char desc[MAX_DESC];
                if (fread(desc, sizeof(char), MAX_DESC, fp_lista) != MAX_DESC) break;
                push(p->historico, desc);
            }

            inserirPaciente(lista, p);
        }
        fclose(fp_lista);
    }

    /* Carregar fila */
    FILE *fp_fila = fopen("fila.bin", "rb");
    if (fp_fila) {
        while (1) {
            int id, prioridade;
            if (fread(&id, sizeof(int), 1, fp_fila) != 1) break;
            if (fread(&prioridade, sizeof(int), 1, fp_fila) != 1) break;
            Paciente *p = buscarPaciente(lista, id);
            if (p) {
                p->prioridade = prioridade;
                enfileirarPrioridade(fila, p);
            }
        }
        fclose(fp_fila);
    }

    return true;
}

#endif
