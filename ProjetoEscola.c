#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAX_ALUNOS      10
#define MAX_PROFESSORES 10
#define MAX_DISCIPLINAS 8
#define MAX_ALUNOS_DISC 10



typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

typedef struct {
    int    matricula;
    char   nome[100];
    char   sexo;
    Data   nascimento;
    char   cpf[15];
    int    ativo;
} Aluno;

typedef struct {
    int    matricula;
    char   nome[100];
    char   sexo;
    Data   nascimento;
    char   cpf[15];
    int    ativo;
} Professor;

typedef struct {
    int  codigo;
    char nome[100];
    int  semestre;
    int  matricula_professor;
    int  alunos[MAX_ALUNOS_DISC];
    int  total_alunos;
    int  ativo;
} Disciplina;


Aluno      alunos[MAX_ALUNOS];
Professor  professores[MAX_PROFESSORES];
Disciplina disciplinas[MAX_DISCIPLINAS];

int total_alunos      = 0;
int total_professores = 0;
int total_disciplinas = 0;

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void ler_string(char *destino, int tamanho) {
    fgets(destino, tamanho, stdin);
    /* Remove o '\n' do final */
    int len = strlen(destino);
    if (len > 0 && destino[len - 1] == '\n') {
        destino[len - 1] = '\0';
    }
}

int data_valida(Data d) {
    int dias_mes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    /* Ano bissexto */
    if (d.ano % 4 == 0 && (d.ano % 100 != 0 || d.ano % 400 == 0))
        dias_mes[2] = 29;

    if (d.mes < 1 || d.mes > 12) return 0;
    if (d.dia < 1 || d.dia > dias_mes[d.mes]) return 0;
    if (d.ano < 1900 || d.ano > 2025) return 0;
    return 1;
}

int cpf_valido(char *cpf) {

    if (strlen(cpf) != 14) return 0;
    if (cpf[3] != '.' || cpf[7] != '.' || cpf[11] != '-') return 0;
    int i;
    for (i = 0; i < 14; i++) {
        if (i == 3 || i == 7 || i == 11) continue;
        if (cpf[i] < '0' || cpf[i] > '9') return 0;
    }
    return 1;
}

Data ler_data() {
    Data d;
    do {
        printf("  Data de nascimento (dd mm aaaa): ");
        scanf("%d %d %d", &d.dia, &d.mes, &d.ano);
        limpar_buffer();
        if (!data_valida(d))
            printf("  Data invalida! Tente novamente.\n");
    } while (!data_valida(d));
    return d;
}

void ler_cpf(char *cpf) {
    do {
        printf("  CPF (formato 000.000.000-00): ");
        ler_string(cpf, 15);
        if (!cpf_valido(cpf))
            printf("  CPF invalido! Use o formato 000.000.000-00.\n");
    } while (!cpf_valido(cpf));
}

int comparar_data(Data a, Data b) {
    if (a.ano  != b.ano)  return (a.ano  < b.ano)  ? -1 : 1;
    if (a.mes  != b.mes)  return (a.mes  < b.mes)  ? -1 : 1;
    if (a.dia  != b.dia)  return (a.dia  < b.dia)  ? -1 : 1;
    return 0;
}

void imprimir_data(Data d) {
    printf("%02d/%02d/%04d", d.dia, d.mes, d.ano);
}

int buscar_aluno(int matricula) {
    int i;
    for (i = 0; i < total_alunos; i++) {
        if (alunos[i].ativo && alunos[i].matricula == matricula)
            return i;
    }
    return -1;
}

int buscar_professor(int matricula) {
    int i;
    for (i = 0; i < total_professores; i++) {
        if (professores[i].ativo && professores[i].matricula == matricula)
            return i;
    }
    return -1;
}

int buscar_disciplina(int codigo) {
    int i;
    for (i = 0; i < total_disciplinas; i++) {
        if (disciplinas[i].ativo && disciplinas[i].codigo == codigo)
            return i;
    }
    return -1;
}

void cadastrar_aluno() {
    if (total_alunos >= MAX_ALUNOS) {
        printf("Limite de alunos atingido!\n");
        return;
    }

    Aluno a;
    printf("\n--- CADASTRAR ALUNO ---\n");

    printf("  Matricula: ");
    scanf("%d", &a.matricula);
    limpar_buffer();

    if (buscar_aluno(a.matricula) != -1) {
        printf("  Matricula ja cadastrada!\n");
        return;
    }

    printf("  Nome: ");
    ler_string(a.nome, 100);

    char sexo_str[5];
    do {
        printf("  Sexo (M/F): ");
        ler_string(sexo_str, 5);
        a.sexo = sexo_str[0];
    } while (a.sexo != 'M' && a.sexo != 'F' && a.sexo != 'm' && a.sexo != 'f');
    a.sexo = (a.sexo == 'm') ? 'M' : (a.sexo == 'f') ? 'F' : a.sexo;

    a.nascimento = ler_data();
    ler_cpf(a.cpf);

    a.ativo = 1;
    alunos[total_alunos] = a;
    total_alunos++;
    printf("  Aluno cadastrado com sucesso!\n");
}

void excluir_aluno() {
    int mat;
    printf("\n--- EXCLUIR ALUNO ---\n");
    printf("  Matricula do aluno: ");
    scanf("%d", &mat);
    limpar_buffer();

    int i = buscar_aluno(mat);
    if (i == -1) {
        printf("  Aluno nao encontrado!\n");
        return;
    }
    alunos[i].ativo = 0;
    printf("  Aluno removido com sucesso!\n");
}

void atualizar_aluno() {
    int mat;
    printf("\n--- ATUALIZAR ALUNO ---\n");
    printf("  Matricula do aluno: ");
    scanf("%d", &mat);
    limpar_buffer();

    int i = buscar_aluno(mat);
    if (i == -1) {
        printf("  Aluno nao encontrado!\n");
        return;
    }

    printf("  Novo nome (%s): ", alunos[i].nome);
    char tmp[100];
    ler_string(tmp, 100);
    if (strlen(tmp) > 0) strcpy(alunos[i].nome, tmp);

    printf("  Novo sexo (%c) (M/F, Enter para manter): ", alunos[i].sexo);
    char sexo_str[5];
    ler_string(sexo_str, 5);
    if (strlen(sexo_str) > 0) {
        char s = sexo_str[0];
        if (s == 'M' || s == 'F' || s == 'm' || s == 'f')
            alunos[i].sexo = (s == 'm') ? 'M' : (s == 'f') ? 'F' : s;
    }

    printf("  Deseja atualizar data de nascimento? (S/N): ");
    char resp[5];
    ler_string(resp, 5);
    if (resp[0] == 'S' || resp[0] == 's')
        alunos[i].nascimento = ler_data();

    printf("  Deseja atualizar CPF? (S/N): ");
    ler_string(resp, 5);
    if (resp[0] == 'S' || resp[0] == 's')
        ler_cpf(alunos[i].cpf);

    printf("  Aluno atualizado com sucesso!\n");
}

void cadastrar_professor() {
    if (total_professores >= MAX_PROFESSORES) {
        printf("Limite de professores atingido!\n");
        return;
    }

    Professor p;
    printf("\n--- CADASTRAR PROFESSOR ---\n");

    printf("  Matricula: ");
    scanf("%d", &p.matricula);
    limpar_buffer();

    if (buscar_professor(p.matricula) != -1) {
        printf("  Matricula ja cadastrada!\n");
        return;
    }

    printf("  Nome: ");
    ler_string(p.nome, 100);

    char sexo_str[5];
    do {
        printf("  Sexo (M/F): ");
        ler_string(sexo_str, 5);
        p.sexo = sexo_str[0];
    } while (p.sexo != 'M' && p.sexo != 'F' && p.sexo != 'm' && p.sexo != 'f');
    p.sexo = (p.sexo == 'm') ? 'M' : (p.sexo == 'f') ? 'F' : p.sexo;

    p.nascimento = ler_data();
    ler_cpf(p.cpf);

    p.ativo = 1;
    professores[total_professores] = p;
    total_professores++;
    printf("  Professor cadastrado com sucesso!\n");
}

void excluir_professor() {
    int mat;
    printf("\n--- EXCLUIR PROFESSOR ---\n");
    printf("  Matricula do professor: ");
    scanf("%d", &mat);
    limpar_buffer();

    int i = buscar_professor(mat);
    if (i == -1) {
        printf("  Professor nao encontrado!\n");
        return;
    }
    professores[i].ativo = 0;
    printf("  Professor removido com sucesso!\n");
}

void atualizar_professor() {
    int mat;
    printf("\n--- ATUALIZAR PROFESSOR ---\n");
    printf("  Matricula do professor: ");
    scanf("%d", &mat);
    limpar_buffer();

    int i = buscar_professor(mat);
    if (i == -1) {
        printf("  Professor nao encontrado!\n");
        return;
    }

    printf("  Novo nome (%s): ", professores[i].nome);
    char tmp[100];
    ler_string(tmp, 100);
    if (strlen(tmp) > 0) strcpy(professores[i].nome, tmp);

    char sexo_str[5];
    printf("  Novo sexo (%c) (M/F, Enter para manter): ", professores[i].sexo);
    ler_string(sexo_str, 5);
    if (strlen(sexo_str) > 0) {
        char s = sexo_str[0];
        if (s == 'M' || s == 'F' || s == 'm' || s == 'f')
            professores[i].sexo = (s == 'm') ? 'M' : (s == 'f') ? 'F' : s;
    }

    char resp[5];
    printf("  Deseja atualizar data de nascimento? (S/N): ");
    ler_string(resp, 5);
    if (resp[0] == 'S' || resp[0] == 's')
        professores[i].nascimento = ler_data();

    printf("  Deseja atualizar CPF? (S/N): ");
    ler_string(resp, 5);
    if (resp[0] == 'S' || resp[0] == 's')
        ler_cpf(professores[i].cpf);

    printf("  Professor atualizado com sucesso!\n");
}

void cadastrar_disciplina() {
    if (total_disciplinas >= MAX_DISCIPLINAS) {
        printf("Limite de disciplinas atingido!\n");
        return;
    }

    Disciplina d;
    d.total_alunos = 0;
    printf("\n--- CADASTRAR DISCIPLINA ---\n");

    printf("  Codigo: ");
    scanf("%d", &d.codigo);
    limpar_buffer();

    if (buscar_disciplina(d.codigo) != -1) {
        printf("  Codigo ja cadastrado!\n");
        return;
    }

    printf("  Nome: ");
    ler_string(d.nome, 100);

    printf("  Semestre: ");
    scanf("%d", &d.semestre);
    limpar_buffer();

    printf("  Matricula do professor responsavel: ");
    scanf("%d", &d.matricula_professor);
    limpar_buffer();

    if (buscar_professor(d.matricula_professor) == -1) {
        printf("  Professor nao encontrado! Disciplina nao cadastrada.\n");
        return;
    }

    d.ativo = 1;
    disciplinas[total_disciplinas] = d;
    total_disciplinas++;
    printf("  Disciplina cadastrada com sucesso!\n");
}

void excluir_disciplina() {
    int cod;
    printf("\n--- EXCLUIR DISCIPLINA ---\n");
    printf("  Codigo da disciplina: ");
    scanf("%d", &cod);
    limpar_buffer();

    int i = buscar_disciplina(cod);
    if (i == -1) {
        printf("  Disciplina nao encontrada!\n");
        return;
    }
    disciplinas[i].ativo = 0;
    printf("  Disciplina removida com sucesso!\n");
}

void atualizar_disciplina() {
    int cod;
    printf("\n--- ATUALIZAR DISCIPLINA ---\n");
    printf("  Codigo da disciplina: ");
    scanf("%d", &cod);
    limpar_buffer();

    int i = buscar_disciplina(cod);
    if (i == -1) {
        printf("  Disciplina nao encontrada!\n");
        return;
    }

    printf("  Novo nome (%s): ", disciplinas[i].nome);
    char tmp[100];
    ler_string(tmp, 100);
    if (strlen(tmp) > 0) strcpy(disciplinas[i].nome, tmp);

    printf("  Novo semestre (%d, 0 para manter): ", disciplinas[i].semestre);
    int sem;
    scanf("%d", &sem);
    limpar_buffer();
    if (sem > 0) disciplinas[i].semestre = sem;

    printf("  Nova matricula do professor (%d, 0 para manter): ", disciplinas[i].matricula_professor);
    int mat;
    scanf("%d", &mat);
    limpar_buffer();
    if (mat > 0) {
        if (buscar_professor(mat) == -1)
            printf("  Professor nao encontrado, mantendo o anterior.\n");
        else
            disciplinas[i].matricula_professor = mat;
    }

    printf("  Disciplina atualizada com sucesso!\n");
}

void inserir_aluno_disciplina() {
    int cod, mat;
    printf("\n--- INSERIR ALUNO NA DISCIPLINA ---\n");
    printf("  Codigo da disciplina: ");
    scanf("%d", &cod);
    limpar_buffer();

    int di = buscar_disciplina(cod);
    if (di == -1) { printf("  Disciplina nao encontrada!\n"); return; }

    if (disciplinas[di].total_alunos >= MAX_ALUNOS_DISC) {
        printf("  Disciplina ja esta com vagas cheias (%d alunos)!\n", MAX_ALUNOS_DISC);
        return;
    }

    printf("  Matricula do aluno: ");
    scanf("%d", &mat);
    limpar_buffer();

    if (buscar_aluno(mat) == -1) { printf("  Aluno nao encontrado!\n"); return; }

    int j;
    for (j = 0; j < disciplinas[di].total_alunos; j++) {
        if (disciplinas[di].alunos[j] == mat) {
            printf("  Aluno ja matriculado nesta disciplina!\n");
            return;
        }
    }

    disciplinas[di].alunos[disciplinas[di].total_alunos] = mat;
    disciplinas[di].total_alunos++;
    printf("  Aluno inserido na disciplina com sucesso!\n");
}

void excluir_aluno_disciplina() {
    int cod, mat;
    printf("\n--- EXCLUIR ALUNO DA DISCIPLINA ---\n");
    printf("  Codigo da disciplina: ");
    scanf("%d", &cod);
    limpar_buffer();

    int di = buscar_disciplina(cod);
    if (di == -1) { printf("  Disciplina nao encontrada!\n"); return; }

    printf("  Matricula do aluno: ");
    scanf("%d", &mat);
    limpar_buffer();

    int j, encontrou = 0;
    for (j = 0; j < disciplinas[di].total_alunos; j++) {
        if (disciplinas[di].alunos[j] == mat) {
            encontrou = 1;
            /* Desloca os demais */
            int k;
            for (k = j; k < disciplinas[di].total_alunos - 1; k++)
                disciplinas[di].alunos[k] = disciplinas[di].alunos[k + 1];
            disciplinas[di].total_alunos--;
            break;
        }
    }

    if (encontrou) printf("  Aluno removido da disciplina com sucesso!\n");
    else           printf("  Aluno nao esta matriculado nesta disciplina!\n");
}

void imprimir_linha_aluno(Aluno a) {
    printf("  Matricula: %d | Nome: %-30s | Sexo: %c | Nasc: ",
           a.matricula, a.nome, a.sexo);
    imprimir_data(a.nascimento);
    printf(" | CPF: %s\n", a.cpf);
}

void imprimir_linha_professor(Professor p) {
    printf("  Matricula: %d | Nome: %-30s | Sexo: %c | Nasc: ",
           p.matricula, p.nome, p.sexo);
    imprimir_data(p.nascimento);
    printf(" | CPF: %s\n", p.cpf);
}

void listar_alunos() {
    printf("\n--- LISTA DE ALUNOS ---\n");
    int i, achou = 0;
    for (i = 0; i < total_alunos; i++) {
        if (alunos[i].ativo) {
            imprimir_linha_aluno(alunos[i]);
            achou = 1;
        }
    }
    if (!achou) printf("  Nenhum aluno cadastrado.\n");
}

void listar_professores() {
    printf("\n--- LISTA DE PROFESSORES ---\n");
    int i, achou = 0;
    for (i = 0; i < total_professores; i++) {
        if (professores[i].ativo) {
            imprimir_linha_professor(professores[i]);
            achou = 1;
        }
    }
    if (!achou) printf("  Nenhum professor cadastrado.\n");
}

void listar_disciplinas() {
    printf("\n--- LISTA DE DISCIPLINAS ---\n");
    int i, achou = 0;
    for (i = 0; i < total_disciplinas; i++) {
        if (!disciplinas[i].ativo) continue;
        achou = 1;
        int pi = buscar_professor(disciplinas[i].matricula_professor);
        printf("  Cod: %d | Nome: %-20s | Semestre: %d | Professor: %s | Alunos: %d\n",
               disciplinas[i].codigo,
               disciplinas[i].nome,
               disciplinas[i].semestre,
               (pi != -1) ? professores[pi].nome : "N/A",
               disciplinas[i].total_alunos);
    }
    if (!achou) printf("  Nenhuma disciplina cadastrada.\n");
}

void listar_uma_disciplina() {
    int cod;
    printf("\n--- LISTAR UMA DISCIPLINA ---\n");
    printf("  Codigo da disciplina: ");
    scanf("%d", &cod);
    limpar_buffer();

    int di = buscar_disciplina(cod);
    if (di == -1) { printf("  Disciplina nao encontrada!\n"); return; }

    int pi = buscar_professor(disciplinas[di].matricula_professor);
    printf("  Codigo: %d\n", disciplinas[di].codigo);
    printf("  Nome:   %s\n", disciplinas[di].nome);
    printf("  Semestre: %d\n", disciplinas[di].semestre);
    printf("  Professor: %s\n", (pi != -1) ? professores[pi].nome : "N/A");
    printf("  Total de alunos: %d\n", disciplinas[di].total_alunos);
    printf("  Alunos matriculados:\n");

    int j;
    for (j = 0; j < disciplinas[di].total_alunos; j++) {
        int ai = buscar_aluno(disciplinas[di].alunos[j]);
        if (ai != -1)
            printf("    - %s (mat: %d)\n", alunos[ai].nome, alunos[ai].matricula);
    }
}

void listar_alunos_por_sexo() {
    char sexo;
    printf("\n--- LISTAR ALUNOS POR SEXO ---\n");
    printf("  Sexo (M/F): ");
    char tmp[5];
    ler_string(tmp, 5);
    sexo = tmp[0];

    int i, achou = 0;
    for (i = 0; i < total_alunos; i++) {
        if (alunos[i].ativo && alunos[i].sexo == sexo) {
            imprimir_linha_aluno(alunos[i]);
            achou = 1;
        }
    }
    if (!achou) printf("  Nenhum aluno encontrado para o sexo informado.\n");
}

void listar_alunos_por_nome() {
    printf("\n--- ALUNOS ORDENADOS POR NOME ---\n");

    /* Copia indices dos ativos */
    int indices[MAX_ALUNOS], qtd = 0, i, j;
    for (i = 0; i < total_alunos; i++)
        if (alunos[i].ativo) indices[qtd++] = i;

    /* Bubble sort pelo nome */
    for (i = 0; i < qtd - 1; i++) {
        for (j = 0; j < qtd - i - 1; j++) {
            if (strcmp(alunos[indices[j]].nome, alunos[indices[j+1]].nome) > 0) {
                int tmp = indices[j];
                indices[j] = indices[j+1];
                indices[j+1] = tmp;
            }
        }
    }

    if (qtd == 0) { printf("  Nenhum aluno cadastrado.\n"); return; }
    for (i = 0; i < qtd; i++)
        imprimir_linha_aluno(alunos[indices[i]]);
}

void listar_alunos_por_data() {
    printf("\n--- ALUNOS ORDENADOS POR DATA DE NASCIMENTTO ---\n");

    int indices[MAX_ALUNOS], qtd = 0, i, j;
    for (i = 0; i < total_alunos; i++)
        if (alunos[i].ativo) indices[qtd++] = i;

    for (i = 0; i < qtd - 1; i++) {
        for (j = 0; j < qtd - i - 1; j++) {
            if (comparar_data(alunos[indices[j]].nascimento, alunos[indices[j+1]].nascimento) > 0) {
                int tmp = indices[j];
                indices[j] = indices[j+1];
                indices[j+1] = tmp;
            }
        }
    }

    if (qtd == 0) { printf("  Nenhum aluno cadastrado.\n"); return; }
    for (i = 0; i < qtd; i++)
        imprimir_linha_aluno(alunos[indices[i]]);
}

void listar_professores_por_sexo() {
    char sexo;
    printf("\n--- LISTAR PROFESSORES POR SEXO ---\n");
    printf("  Sexo (M/F): ");
    char tmp[5];
    ler_string(tmp, 5);
    sexo = tmp[0];

    int i, achou = 0;
    for (i = 0; i < total_professores; i++) {
        if (professores[i].ativo && professores[i].sexo == sexo) {
            imprimir_linha_professor(professores[i]);
            achou = 1;
        }
    }
    if (!achou) printf("  Nenhum professor encontrado para o sexo informado.\n");
}

void listar_professores_por_nome() {
    printf("\n--- PROFESSORES ORDENADOS POR NOME ---\n");

    int indices[MAX_PROFESSORES], qtd = 0, i, j;
    for (i = 0; i < total_professores; i++)
        if (professores[i].ativo) indices[qtd++] = i;

    for (i = 0; i < qtd - 1; i++) {
        for (j = 0; j < qtd - i - 1; j++) {
            if (strcmp(professores[indices[j]].nome, professores[indices[j+1]].nome) > 0) {
                int tmp = indices[j];
                indices[j] = indices[j+1];
                indices[j+1] = tmp;
            }
        }
    }

    if (qtd == 0) { printf("  Nenhum professor cadastrado.\n"); return; }
    for (i = 0; i < qtd; i++)
        imprimir_linha_professor(professores[indices[i]]);
}

void listar_professores_por_data() {
    printf("\n--- PROFESSORES ORDENADOS POR DATA DE NASCIMENTO ---\n");

    int indices[MAX_PROFESSORES], qtd = 0, i, j;
    for (i = 0; i < total_professores; i++)
        if (professores[i].ativo) indices[qtd++] = i;

    for (i = 0; i < qtd - 1; i++) {
        for (j = 0; j < qtd - i - 1; j++) {
            if (comparar_data(professores[indices[j]].nascimento, professores[indices[j+1]].nascimento) > 0) {
                int tmp = indices[j];
                indices[j] = indices[j+1];
                indices[j+1] = tmp;
            }
        }
    }

    if (qtd == 0) { printf("  Nenhum professor cadastrado.\n"); return; }
    for (i = 0; i < qtd; i++)
        imprimir_linha_professor(professores[indices[i]]);
}

void aniversariantes_do_mes() {
    int mes;
    printf("\n--- ANIVERSARIANTES DO MES ---\n");
    printf("  Mes (1-12): ");
    scanf("%d", &mes);
    limpar_buffer();

    if (mes < 1 || mes > 12) { printf("  Mes invalido!\n"); return; }

    int i, achou = 0;
    printf("  >> Alunos:\n");
    for (i = 0; i < total_alunos; i++) {
        if (alunos[i].ativo && alunos[i].nascimento.mes == mes) {
            imprimir_linha_aluno(alunos[i]);
            achou = 1;
        }
    }

    printf("  >> Professores:\n");
    for (i = 0; i < total_professores; i++) {
        if (professores[i].ativo && professores[i].nascimento.mes == mes) {
            imprimir_linha_professor(professores[i]);
            achou = 1;
        }
    }

    if (!achou) printf("  Nenhum aniversariante no mes informado.\n");
}

void buscar_por_string() {
    char busca[100];
    printf("\n--- BUSCA POR NOME ---\n");

    do {
        printf("  Digite ao menos 3 letras para buscar: ");
        ler_string(busca, 100);
        if (strlen(busca) < 3)
            printf("  Informe ao menos 3 letras!\n");
    } while (strlen(busca) < 3);

    int i, achou = 0;

    /* strstr faz a busca de substring */
    printf("  >> Alunos encontrados:\n");
    for (i = 0; i < total_alunos; i++) {
        if (alunos[i].ativo && strstr(alunos[i].nome, busca) != NULL) {
            imprimir_linha_aluno(alunos[i]);
            achou = 1;
        }
    }

    printf("  >> Professores encontrados:\n");
    for (i = 0; i < total_professores; i++) {
        if (professores[i].ativo && strstr(professores[i].nome, busca) != NULL) {
            imprimir_linha_professor(professores[i]);
            achou = 1;
        }
    }

    if (!achou) printf("  Nenhuma pessoa encontrada com esse nome.\n");
}

void alunos_menos_3_disciplinas() {
    printf("\n--- ALUNOS MATRICULADOS EM MENOS DE 3 DISCIPLINAS ---\n");

    int i, j, achou = 0;
    for (i = 0; i < total_alunos; i++) {
        if (!alunos[i].ativo) continue;

        int count = 0;
        for (j = 0; j < total_disciplinas; j++) {
            if (!disciplinas[j].ativo) continue;
            int k;
            for (k = 0; k < disciplinas[j].total_alunos; k++) {
                if (disciplinas[j].alunos[k] == alunos[i].matricula) {
                    count++;
                    break;
                }
            }
        }

        if (count < 3) {
            printf("  ");
            imprimir_linha_aluno(alunos[i]);
            printf("     (matriculado em %d disciplina(s))\n", count);
            achou = 1;
        }
    }
    if (!achou) printf("  Todos os alunos estao em 3 ou mais disciplinas.\n");
}

void disciplinas_com_mais_de_40_vagas() {
    printf("\n--- DISCIPLINAS COM MAIS DE 40 ALUNOS ---\n");
    int i, achou = 0;
    for (i = 0; i < total_disciplinas; i++) {
        if (!disciplinas[i].ativo) continue;
        if (disciplinas[i].total_alunos > 40) {
            int pi = buscar_professor(disciplinas[i].matricula_professor);
            printf("  Cod: %d | Nome: %-20s | Alunos: %d | Professor: %s\n",
                   disciplinas[i].codigo,
                   disciplinas[i].nome,
                   disciplinas[i].total_alunos,
                   (pi != -1) ? professores[pi].nome : "N/A");
            achou = 1;
        }
    }
    if (!achou) printf("  Nenhuma disciplina com mais de 40 alunos.\n");
}

void menu_aluno() {
    int op;
    do {
        printf("\n=== MENU ALUNOS ===\n");
        printf("1. Cadastrar aluno\n");
        printf("2. Excluir aluno\n");
        printf("3. Atualizar aluno\n");
        printf("4. Listar todos os alunos\n");
        printf("5. Listar alunos por sexo\n");
        printf("6. Listar alunos ordenados por nome\n");
        printf("7. Listar alunos ordenados por data de nascimento\n");
        printf("8. Alunos em menos de 3 disciplinas\n");
        printf("0. Voltar\n");
        printf("Opcao: ");
        scanf("%d", &op);
        limpar_buffer();

        switch (op) {
            case 1: cadastrar_aluno();              break;
            case 2: excluir_aluno();                break;
            case 3: atualizar_aluno();              break;
            case 4: listar_alunos();                break;
            case 5: listar_alunos_por_sexo();       break;
            case 6: listar_alunos_por_nome();       break;
            case 7: listar_alunos_por_data();       break;
            case 8: alunos_menos_3_disciplinas();   break;
            case 0: break;
            default: printf("Opcao invalida!\n");
        }
    } while (op != 0);
}

void menu_professor() {
    int op;
    do {
        printf("\n=== MENU PROFESSORES ===\n");
        printf("1. Cadastrar professor\n");
        printf("2. Excluir professor\n");
        printf("3. Atualizar professor\n");
        printf("4. Listar todos os professores\n");
        printf("5. Listar professores por sexo\n");
        printf("6. Listar professores ordenados por nome\n");
        printf("7. Listar professores ordenados por data de nascimento\n");
        printf("0. Voltar\n");
        printf("Opcao: ");
        scanf("%d", &op);
        limpar_buffer();

        switch (op) {
            case 1: cadastrar_professor();              break;
            case 2: excluir_professor();                break;
            case 3: atualizar_professor();              break;
            case 4: listar_professores();               break;
            case 5: listar_professores_por_sexo();      break;
            case 6: listar_professores_por_nome();      break;
            case 7: listar_professores_por_data();      break;
            case 0: break;
            default: printf("Opcao invalida!\n");
        }
    } while (op != 0);
}

void menu_disciplina() {
    int op;
    do {
        printf("\n=== MENU DISCIPLINAS ===\n");
        printf("1. Cadastrar disciplina\n");
        printf("2. Excluir disciplina\n");
        printf("3. Atualizar disciplina\n");
        printf("4. Inserir aluno na disciplina\n");
        printf("5. Excluir aluno da disciplina\n");
        printf("6. Listar todas as disciplinas\n");
        printf("7. Listar uma disciplina (com alunos)\n");
        printf("8. Disciplinas com mais de 40 alunos\n");
        printf("0. Voltar\n");
        printf("Opcao: ");
        scanf("%d", &op);
        limpar_buffer();

        switch (op) {
            case 1: cadastrar_disciplina();            break;
            case 2: excluir_disciplina();              break;
            case 3: atualizar_disciplina();            break;
            case 4: inserir_aluno_disciplina();        break;
            case 5: excluir_aluno_disciplina();        break;
            case 6: listar_disciplinas();              break;
            case 7: listar_uma_disciplina();           break;
            case 8: disciplinas_com_mais_de_40_vagas(); break;
            case 0: break;
            default: printf("Opcao invalida!\n");
        }
    } while (op != 0);
}

void menu_relatorios() {
    int op;
    do {
        printf("\n=== MENU RELATORIOS ===\n");
        printf("1. Aniversariantes do mes\n");
        printf("2. Buscar pessoa por nome\n");
        printf("0. Voltar\n");
        printf("Opcao: ");
        scanf("%d", &op);
        limpar_buffer();

        switch (op) {
            case 1: aniversariantes_do_mes(); break;
            case 2: buscar_por_string();      break;
            case 0: break;
            default: printf("Opcao invalida!\n");
        }
    } while (op != 0);
}

int main() {
    int op;
    do {
        printf("\n==============================\n");
        printf(" BEM-VINDO AO PROJETO ESCOLA      \n");
        printf("==============================\n");
        printf("1. Gerenciar Alunos\n");
        printf("2. Gerenciar Professores\n");
        printf("3. Gerenciar Disciplinas\n");
        printf("4. Relatorios\n");
        printf("0. Sair\n");
        printf("Opcao: ");
        scanf("%d", &op);
        limpar_buffer();

        switch (op) {
            case 1: menu_aluno();       break;
            case 2: menu_professor();   break;
            case 3: menu_disciplina();  break;
            case 4: menu_relatorios();  break;
            case 0: printf("Encerrando o sistema...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    } while (op != 0);

    return 0;
}
