# 🏥 Sistema de Gerenciamento de Pronto-Socorro

Projeto desenvolvido em **C** com o objetivo de aplicar conceitos de **Algoritmos e Estruturas de Dados** em um sistema de gerenciamento de pacientes de um pronto-socorro.

O sistema permite cadastrar, consultar, remover e organizar pacientes, além de controlar uma fila de atendimento baseada em diferentes níveis de prioridade.

---

## 📌 Funcionalidades

O programa possui um menu interativo com as seguintes operações:

- Registrar um novo paciente;
- Remover um paciente dos registros;
- Listar todos os pacientes cadastrados;
- Buscar um paciente pelo ID;
- Visualizar a fila de espera;
- Chamar o próximo paciente para atendimento;
- Salvar os dados ao encerrar o programa.

Os pacientes são cadastrados com:

- ID;
- Nome;
- Nível de prioridade.

A prioridade de atendimento varia entre:

1. Emergência;
2. Muito urgente;
3. Urgente;
4. Pouco urgente;
5. Não urgente.

---

## 🧠 Estruturas de Dados Utilizadas

### Árvore Binária de Busca

Os pacientes cadastrados são armazenados em uma **Árvore Binária de Busca (BST)**, utilizando o ID do paciente como chave.

A estrutura permite realizar operações como:

- Inserção;
- Busca por ID;
- Remoção;
- Percurso em ordem para listagem dos pacientes.

---

### Fila Circular

Cada nível de prioridade utiliza uma **fila circular dinâmica**.

A fila mantém os índices de início e fim e utiliza aritmética modular para reaproveitar posições do vetor.

Caso sua capacidade seja atingida, a estrutura é realocada dinamicamente para comportar novos pacientes.

---

### Fila de Prioridade

A fila de atendimento é composta por **cinco filas**, uma para cada nível de prioridade.

O sistema sempre verifica primeiro os pacientes de maior prioridade:

`Emergência → Muito urgente → Urgente → Pouco urgente → Não urgente`

Dentro de um mesmo nível de prioridade, os pacientes são atendidos seguindo a ordem de chegada (**FIFO**).

---

### Pilha

Cada paciente possui uma **pilha associada ao histórico de atendimento**, permitindo armazenar procedimentos do mais recente para o mais antigo.

---

## 💾 Persistência de Dados

O sistema utiliza **arquivos binários** para manter os dados entre diferentes execuções do programa.

São utilizados dois arquivos:

- `pacientes.bin` — armazena os pacientes e seus respectivos dados;
- `fila.bin` — armazena os pacientes que ainda estão na fila de atendimento.

Ao iniciar o programa, os dados existentes são carregados automaticamente.

Ao encerrar o sistema, os dados são gravados novamente nos arquivos binários.

---

## 🛠️ Conceitos Aplicados

Durante o desenvolvimento foram utilizados conceitos como:

- Linguagem C;
- Ponteiros;
- Structs;
- Alocação dinâmica de memória;
- Pilhas;
- Filas;
- Filas circulares;
- Filas de prioridade;
- Árvores Binárias de Busca;
- Recursão;
- Manipulação de arquivos binários;
- Persistência de dados;
- Modularização de código.

---

## 📂 Estrutura do Projeto

```text
pronto-socorro-estruturas-de-dados/
│
├── HospitalMain.c
├── TADS.h
├── README.md
└── .gitignore