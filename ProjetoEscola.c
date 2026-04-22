#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ===================== CONSTANTES ===================== */
#define MAX_ALUNOS       100
#define MAX_PROFESSORES  50
#define MAX_DISCIPLINAS  50
#define MAX_ALUNOS_DISC  60
#define TAM_NOME         100
#define TAM_CPF          15
#define TAM_MATRICULA    15
#define TAM_CODIGO       15

/* ===================== STRUCTS ===================== */
typedef struct {
    int dia, mes, ano;
} Data;

typedef struct {
    char matricula[TAM_MATRICULA];
    char nome[TAM_NOME];
    char sexo;          /* 'M' ou 'F' */
    Data nascimento;
    char cpf[TAM_CPF];
    int ativo;
} Aluno;

typedef struct {
    char matricula[TAM_MATRICULA];
    char nome[TAM_NOME];
    char sexo;
    Data nascimento;
    char cpf[TAM_CPF];
    int ativo;
} Professor;

typedef struct {
    char codigo[TAM_CODIGO];
    char nome[TAM_NOME];
    int semestre;
    char matricula_prof[TAM_MATRICULA];
    char alunos[MAX_ALUNOS_DISC][TAM_MATRICULA];
    int num_alunos;
    int ativo;
} Disciplina;

/* ===================== DADOS GLOBAIS ===================== */
Aluno      alunos[MAX_ALUNOS];
Professor  professores[MAX_PROFESSORES];
Disciplina disciplinas[MAX_DISCIPLINAS];
int        num_alunos      = 0;
int        num_professores = 0;
int        num_disciplinas = 0;

/* ===================== UTILITÁRIOS ===================== */

void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void ler_string(const char *prompt, char *dest, int tamanho) {
    printf("%s", prompt);
    fgets(dest, tamanho, stdin);
    dest[strcspn(dest, "\n")] = '\0';
}

void str_upper(char *s, char *out, int n) {
    int i;
    for (i = 0; i < n - 1 && s[i]; i++)
        out[i] = (char)toupper((unsigned char)s[i]);
    out[i] = '\0';
}

/* Verifica se o CPF tem formato válido (XXX.XXX.XXX-XX ou 11 dígitos) */
int validar_cpf(const char *cpf) {
    char digits[12];
    int j = 0;
    for (int i = 0; cpf[i] && j < 11; i++)
        if (isdigit((unsigned char)cpf[i]))
            digits[j++] = cpf[i];
    if (j != 11) return 0;
    /* Rejeita sequências repetidas */
    int all_same = 1;
    for (int i = 1; i < 11; i++)
        if (digits[i] != digits[0]) { all_same = 0; break; }
    if (all_same) return 0;
    /* Dígito verificador 1 */
    int soma = 0;
    for (int i = 0; i < 9; i++) soma += (digits[i] - '0') * (10 - i);
    int r = (soma * 10) % 11;
    if (r == 10) r = 0;
    if (r != (digits[9] - '0')) return 0;
    /* Dígito verificador 2 */
    soma = 0;
    for (int i = 0; i < 10; i++) soma += (digits[i] - '0') * (11 - i);
    r = (soma * 10) % 11;
    if (r == 10) r = 0;
    if (r != (digits[10] - '0')) return 0;
    return 1;
}

int ano_bissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

int dias_no_mes(int mes, int ano) {
    int dias[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    if (mes == 2 && ano_bissexto(ano)) return 29;
    return dias[mes];
}

int validar_data(Data d) {
    if (d.ano < 1900 || d.ano > 2025) return 0;
    if (d.mes < 1 || d.mes > 12) return 0;
    if (d.dia < 1 || d.dia > dias_no_mes(d.mes, d.ano)) return 0;
    return 1;
}

Data ler_data(const char *prompt) {
    Data d = {0, 0, 0};
    char buf[30];
    do {
        printf("%s (DD/MM/AAAA): ", prompt);
        fgets(buf, sizeof(buf), stdin);
        sscanf(buf, "%d/%d/%d", &d.dia, &d.mes, &d.ano);
        if (!validar_data(d))
            printf("  Data inválida. Tente novamente.\n");
    } while (!validar_data(d));
    return d;
}

char ler_sexo(void) {
    char c;
    do {
        printf("Sexo (M/F): ");
        scanf(" %c", &c);
        limpar_buffer();
        c = (char)toupper((unsigned char)c);
        if (c != 'M' && c != 'F')
            printf("  Valor inválido. Digite M ou F.\n");
    } while (c != 'M' && c != 'F');
    return c;
}

void imprimir_data(Data d) {
    printf("%02d/%02d/%04d", d.dia, d.mes, d.ano);
}

int comparar_datas(Data a, Data b) {
    if (a.ano != b.ano) return a.ano - b.ano;
    if (a.mes != b.mes) return a.mes - b.mes;
    return a.dia - b.dia;
}

/* ===================== BUSCA ===================== */

int buscar_aluno_por_matricula(const char *mat) {
    for (int i = 0; i < num_alunos; i++)
        if (alunos[i].ativo && strcmp(alunos[i].matricula, mat) == 0)
            return i;
    return -1;
}

int buscar_professor_por_matricula(const char *mat) {
    for (int i = 0; i < num_professores; i++)
        if (professores[i].ativo && strcmp(professores[i].matricula, mat) == 0)
            return i;
    return -1;
}

int buscar_disciplina_por_codigo(const char *cod) {
    for (int i = 0; i < num_disciplinas; i++)
        if (disciplinas[i].ativo && strcmp(disciplinas[i].codigo, cod) == 0)
            return i;
    return -1;
}

/* ===================== ALUNOS ===================== */

void cadastrar_aluno(void) {
    if (num_alunos >= MAX_ALUNOS) {
        printf("Limite de alunos atingido.\n");
        return;
    }
    Aluno a;
    memset(&a, 0, sizeof(a));

    /* Matrícula única */
    do {
        ler_string("Matrícula: ", a.matricula, TAM_MATRICULA);
        if (strlen(a.matricula) == 0) { printf("  Campo obrigatório.\n"); continue; }
        if (buscar_aluno_por_matricula(a.matricula) >= 0)
            printf("  Matrícula já cadastrada.\n");
        else break;
    } while (1);

    ler_string("Nome: ", a.nome, TAM_NOME);
    while (strlen(a.nome) == 0) {
        printf("  Campo obrigatório.\n");
        ler_string("Nome: ", a.nome, TAM_NOME);
    }

    a.sexo = ler_sexo();
    a.nascimento = ler_data("Data de Nascimento");

    do {
        ler_string("CPF: ", a.cpf, TAM_CPF);
        if (!validar_cpf(a.cpf)) printf("  CPF inválido.\n");
    } while (!validar_cpf(a.cpf));

    a.ativo = 1;
    alunos[num_alunos++] = a;
    printf("Aluno cadastrado com sucesso!\n");
}

void excluir_aluno(void) {
    char mat[TAM_MATRICULA];
    ler_string("Matrícula do aluno: ", mat, TAM_MATRICULA);
    int idx = buscar_aluno_por_matricula(mat);
    if (idx < 0) { printf("Aluno não encontrado.\n"); return; }

    /* Remove de disciplinas */
    for (int i = 0; i < num_disciplinas; i++) {
        if (!disciplinas[i].ativo) continue;
        for (int j = 0; j < disciplinas[i].num_alunos; j++) {
            if (strcmp(disciplinas[i].alunos[j], mat) == 0) {
                for (int k = j; k < disciplinas[i].num_alunos - 1; k++)
                    strcpy(disciplinas[i].alunos[k], disciplinas[i].alunos[k+1]);
                disciplinas[i].num_alunos--;
                break;
            }
        }
    }
    alunos[idx].ativo = 0;
    printf("Aluno removido.\n");
}

void atualizar_aluno(void) {
    char mat[TAM_MATRICULA];
    ler_string("Matrícula do aluno: ", mat, TAM_MATRICULA);
    int idx = buscar_aluno_por_matricula(mat);
    if (idx < 0) { printf("Aluno não encontrado.\n"); return; }

    Aluno *a = &alunos[idx];
    printf("Deixe em branco para manter o valor atual.\n");

    char buf[TAM_NOME];
    ler_string("Novo nome (atual: ", buf, 1); /* só para exibir */
    printf("Novo nome (atual: %s): ", a->nome);
    fgets(buf, TAM_NOME, stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) > 0) strcpy(a->nome, buf);

    printf("Novo sexo (atual: %c) (M/F ou Enter para manter): ", a->sexo);
    char linha[5];
    fgets(linha, sizeof(linha), stdin);
    if (linha[0] != '\n' && linha[0] != '\0') {
        char ns = (char)toupper((unsigned char)linha[0]);
        if (ns == 'M' || ns == 'F') a->sexo = ns;
    }

    printf("Atualizar data de nascimento? (s/N): ");
    fgets(linha, sizeof(linha), stdin);
    if (toupper((unsigned char)linha[0]) == 'S')
        a->nascimento = ler_data("Nova Data de Nascimento");

    printf("Atualizar CPF? (s/N): ");
    fgets(linha, sizeof(linha), stdin);
    if (toupper((unsigned char)linha[0]) == 'S') {
        char novo_cpf[TAM_CPF];
        do {
            ler_string("Novo CPF: ", novo_cpf, TAM_CPF);
            if (!validar_cpf(novo_cpf)) printf("  CPF inválido.\n");
        } while (!validar_cpf(novo_cpf));
        strcpy(a->cpf, novo_cpf);
    }
    printf("Aluno atualizado.\n");
}

void imprimir_aluno(const Aluno *a) {
    printf("  Matrícula: %s | Nome: %-30s | Sexo: %c | Nasc: ",
           a->matricula, a->nome, a->sexo);
    imprimir_data(a->nascimento);
    printf(" | CPF: %s\n", a->cpf);
}

/* ===================== PROFESSORES ===================== */

void cadastrar_professor(void) {
    if (num_professores >= MAX_PROFESSORES) {
        printf("Limite de professores atingido.\n");
        return;
    }
    Professor p;
    memset(&p, 0, sizeof(p));

    do {
        ler_string("Matrícula: ", p.matricula, TAM_MATRICULA);
        if (strlen(p.matricula) == 0) { printf("  Campo obrigatório.\n"); continue; }
        if (buscar_professor_por_matricula(p.matricula) >= 0)
            printf("  Matrícula já cadastrada.\n");
        else break;
    } while (1);

    ler_string("Nome: ", p.nome, TAM_NOME);
    while (strlen(p.nome) == 0) {
        printf("  Campo obrigatório.\n");
        ler_string("Nome: ", p.nome, TAM_NOME);
    }

    p.sexo = ler_sexo();
    p.nascimento = ler_data("Data de Nascimento");

    do {
        ler_string("CPF: ", p.cpf, TAM_CPF);
        if (!validar_cpf(p.cpf)) printf("  CPF inválido.\n");
    } while (!validar_cpf(p.cpf));

    p.ativo = 1;
    professores[num_professores++] = p;
    printf("Professor cadastrado com sucesso!\n");
}

void excluir_professor(void) {
    char mat[TAM_MATRICULA];
    ler_string("Matrícula do professor: ", mat, TAM_MATRICULA);
    int idx = buscar_professor_por_matricula(mat);
    if (idx < 0) { printf("Professor não encontrado.\n"); return; }

    /* Verifica se tem disciplina ativa */
    for (int i = 0; i < num_disciplinas; i++) {
        if (disciplinas[i].ativo &&
            strcmp(disciplinas[i].matricula_prof, mat) == 0) {
            printf("Não é possível excluir: professor possui disciplinas ativas.\n");
            return;
        }
    }
    professores[idx].ativo = 0;
    printf("Professor removido.\n");
}

void atualizar_professor(void) {
    char mat[TAM_MATRICULA];
    ler_string("Matrícula do professor: ", mat, TAM_MATRICULA);
    int idx = buscar_professor_por_matricula(mat);
    if (idx < 0) { printf("Professor não encontrado.\n"); return; }

    Professor *p = &professores[idx];
    printf("Deixe em branco para manter o valor atual.\n");

    char buf[TAM_NOME];
    printf("Novo nome (atual: %s): ", p->nome);
    fgets(buf, TAM_NOME, stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) > 0) strcpy(p->nome, buf);

    printf("Novo sexo (atual: %c) (M/F ou Enter para manter): ", p->sexo);
    char linha[5];
    fgets(linha, sizeof(linha), stdin);
    if (linha[0] != '\n' && linha[0] != '\0') {
        char ns = (char)toupper((unsigned char)linha[0]);
        if (ns == 'M' || ns == 'F') p->sexo = ns;
    }

    printf("Atualizar data de nascimento? (s/N): ");
    fgets(linha, sizeof(linha), stdin);
    if (toupper((unsigned char)linha[0]) == 'S')
        p->nascimento = ler_data("Nova Data de Nascimento");

    printf("Atualizar CPF? (s/N): ");
    fgets(linha, sizeof(linha), stdin);
    if (toupper((unsigned char)linha[0]) == 'S') {
        char novo_cpf[TAM_CPF];
        do {
            ler_string("Novo CPF: ", novo_cpf, TAM_CPF);
            if (!validar_cpf(novo_cpf)) printf("  CPF inválido.\n");
        } while (!validar_cpf(novo_cpf));
        strcpy(p->cpf, novo_cpf);
    }
    printf("Professor atualizado.\n");
}

void imprimir_professor(const Professor *p) {
    printf("  Matrícula: %s | Nome: %-30s | Sexo: %c | Nasc: ",
           p->matricula, p->nome, p->sexo);
    imprimir_data(p->nascimento);
    printf(" | CPF: %s\n", p->cpf);
}

/* ===================== DISCIPLINAS ===================== */

void cadastrar_disciplina(void) {
    if (num_disciplinas >= MAX_DISCIPLINAS) {
        printf("Limite de disciplinas atingido.\n");
        return;
    }
    Disciplina d;
    memset(&d, 0, sizeof(d));

    do {
        ler_string("Código: ", d.codigo, TAM_CODIGO);
        if (strlen(d.codigo) == 0) { printf("  Campo obrigatório.\n"); continue; }
        if (buscar_disciplina_por_codigo(d.codigo) >= 0)
            printf("  Código já cadastrado.\n");
        else break;
    } while (1);

    ler_string("Nome: ", d.nome, TAM_NOME);
    while (strlen(d.nome) == 0) {
        printf("  Campo obrigatório.\n");
        ler_string("Nome: ", d.nome, TAM_NOME);
    }

    printf("Semestre: ");
    scanf("%d", &d.semestre);
    limpar_buffer();

    do {
        ler_string("Matrícula do Professor: ", d.matricula_prof, TAM_MATRICULA);
        if (buscar_professor_por_matricula(d.matricula_prof) < 0)
            printf("  Professor não encontrado.\n");
        else break;
    } while (1);

    d.ativo = 1;
    disciplinas[num_disciplinas++] = d;
    printf("Disciplina cadastrada com sucesso!\n");
}

void excluir_disciplina(void) {
    char cod[TAM_CODIGO];
    ler_string("Código da disciplina: ", cod, TAM_CODIGO);
    int idx = buscar_disciplina_por_codigo(cod);
    if (idx < 0) { printf("Disciplina não encontrada.\n"); return; }
    disciplinas[idx].ativo = 0;
    printf("Disciplina removida.\n");
}

void atualizar_disciplina(void) {
    char cod[TAM_CODIGO];
    ler_string("Código da disciplina: ", cod, TAM_CODIGO);
    int idx = buscar_disciplina_por_codigo(cod);
    if (idx < 0) { printf("Disciplina não encontrada.\n"); return; }

    Disciplina *d = &disciplinas[idx];
    char buf[TAM_NOME];

    printf("Novo nome (atual: %s): ", d->nome);
    fgets(buf, TAM_NOME, stdin);
    buf[strcspn(buf, "\n")] = '\0';
    if (strlen(buf) > 0) strcpy(d->nome, buf);

    printf("Novo semestre (atual: %d, 0 para manter): ", d->semestre);
    int s;
    scanf("%d", &s);
    limpar_buffer();
    if (s > 0) d->semestre = s;

    printf("Atualizar professor? (s/N): ");
    char linha[5];
    fgets(linha, sizeof(linha), stdin);
    if (toupper((unsigned char)linha[0]) == 'S') {
        char nova_mat[TAM_MATRICULA];
        do {
            ler_string("Matrícula do novo professor: ", nova_mat, TAM_MATRICULA);
            if (buscar_professor_por_matricula(nova_mat) < 0)
                printf("  Professor não encontrado.\n");
            else break;
        } while (1);
        strcpy(d->matricula_prof, nova_mat);
    }
    printf("Disciplina atualizada.\n");
}

void inserir_aluno_disciplina(void) {
    char cod[TAM_CODIGO], mat[TAM_MATRICULA];
    ler_string("Código da disciplina: ", cod, TAM_CODIGO);
    int di = buscar_disciplina_por_codigo(cod);
    if (di < 0) { printf("Disciplina não encontrada.\n"); return; }
    if (disciplinas[di].num_alunos >= MAX_ALUNOS_DISC) {
        printf("Disciplina com vagas esgotadas (%d/%d).\n",
               disciplinas[di].num_alunos, MAX_ALUNOS_DISC);
        return;
    }

    ler_string("Matrícula do aluno: ", mat, TAM_MATRICULA);
    if (buscar_aluno_por_matricula(mat) < 0) { printf("Aluno não encontrado.\n"); return; }

    /* Verifica duplicata */
    for (int j = 0; j < disciplinas[di].num_alunos; j++) {
        if (strcmp(disciplinas[di].alunos[j], mat) == 0) {
            printf("Aluno já matriculado nessa disciplina.\n");
            return;
        }
    }
    strcpy(disciplinas[di].alunos[disciplinas[di].num_alunos++], mat);
    printf("Aluno inserido na disciplina.\n");
}

void excluir_aluno_disciplina(void) {
    char cod[TAM_CODIGO], mat[TAM_MATRICULA];
    ler_string("Código da disciplina: ", cod, TAM_CODIGO);
    int di = buscar_disciplina_por_codigo(cod);
    if (di < 0) { printf("Disciplina não encontrada.\n"); return; }

    ler_string("Matrícula do aluno: ", mat, TAM_MATRICULA);
    for (int j = 0; j < disciplinas[di].num_alunos; j++) {
        if (strcmp(disciplinas[di].alunos[j], mat) == 0) {
            for (int k = j; k < disciplinas[di].num_alunos - 1; k++)
                strcpy(disciplinas[di].alunos[k], disciplinas[di].alunos[k+1]);
            disciplinas[di].num_alunos--;
            printf("Aluno removido da disciplina.\n");
            return;
        }
    }
    printf("Aluno não está matriculado nessa disciplina.\n");
}

void imprimir_disciplina_resumo(const Disciplina *d) {
    int pi = buscar_professor_por_matricula(d->matricula_prof);
    printf("  Código: %-10s | Nome: %-30s | Semestre: %d | Professor: %s | Alunos: %d\n",
           d->codigo, d->nome, d->semestre,
           (pi >= 0) ? professores[pi].nome : "N/A",
           d->num_alunos);
}

/* ===================== RELATÓRIOS ===================== */

/* Cópia de índices para ordenação (não modifica arrays originais) */

void listar_alunos(void) {
    printf("\n=== Lista de Alunos ===\n");
    int found = 0;
    for (int i = 0; i < num_alunos; i++) {
        if (alunos[i].ativo) { imprimir_aluno(&alunos[i]); found = 1; }
    }
    if (!found) printf("  Nenhum aluno cadastrado.\n");
}

void listar_professores(void) {
    printf("\n=== Lista de Professores ===\n");
    int found = 0;
    for (int i = 0; i < num_professores; i++) {
        if (professores[i].ativo) { imprimir_professor(&professores[i]); found = 1; }
    }
    if (!found) printf("  Nenhum professor cadastrado.\n");
}

void listar_disciplinas(void) {
    printf("\n=== Lista de Disciplinas ===\n");
    int found = 0;
    for (int i = 0; i < num_disciplinas; i++) {
        if (disciplinas[i].ativo) { imprimir_disciplina_resumo(&disciplinas[i]); found = 1; }
    }
    if (!found) printf("  Nenhuma disciplina cadastrada.\n");
}

void listar_disciplina_detalhe(void) {
    char cod[TAM_CODIGO];
    ler_string("Código da disciplina: ", cod, TAM_CODIGO);
    int di = buscar_disciplina_por_codigo(cod);
    if (di < 0) { printf("Disciplina não encontrada.\n"); return; }

    Disciplina *d = &disciplinas[di];
    printf("\n=== Disciplina: %s ===\n", d->nome);
    imprimir_disciplina_resumo(d);
    printf("  Alunos matriculados (%d):\n", d->num_alunos);
    if (d->num_alunos == 0) {
        printf("    Nenhum aluno matriculado.\n");
        return;
    }
    for (int j = 0; j < d->num_alunos; j++) {
        int ai = buscar_aluno_por_matricula(d->alunos[j]);
        if (ai >= 0) imprimir_aluno(&alunos[ai]);
        else printf("    Matrícula %s (removido)\n", d->alunos[j]);
    }
}

void listar_alunos_por_sexo(void) {
    char s;
    do {
        printf("Sexo (M/F): ");
        scanf(" %c", &s);
        limpar_buffer();
        s = (char)toupper((unsigned char)s);
        if (s != 'M' && s != 'F') printf("  Inválido.\n");
    } while (s != 'M' && s != 'F');

    printf("\n=== Alunos %s ===\n", s == 'M' ? "Masculino" : "Feminino");
    int found = 0;
    for (int i = 0; i < num_alunos; i++) {
        if (alunos[i].ativo && alunos[i].sexo == s) {
            imprimir_aluno(&alunos[i]); found = 1;
        }
    }
    if (!found) printf("  Nenhum aluno encontrado.\n");
}

void listar_alunos_por_nome(void) {
    /* Bubble sort em cópias de índices */
    int idx[MAX_ALUNOS], n = 0;
    for (int i = 0; i < num_alunos; i++)
        if (alunos[i].ativo) idx[n++] = i;

    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (strcmp(alunos[idx[j]].nome, alunos[idx[j+1]].nome) > 0) {
                int tmp = idx[j]; idx[j] = idx[j+1]; idx[j+1] = tmp;
            }

    printf("\n=== Alunos ordenados por Nome ===\n");
    if (n == 0) { printf("  Nenhum aluno.\n"); return; }
    for (int i = 0; i < n; i++) imprimir_aluno(&alunos[idx[i]]);
}

void listar_alunos_por_nascimento(void) {
    int idx[MAX_ALUNOS], n = 0;
    for (int i = 0; i < num_alunos; i++)
        if (alunos[i].ativo) idx[n++] = i;

    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (comparar_datas(alunos[idx[j]].nascimento, alunos[idx[j+1]].nascimento) > 0) {
                int tmp = idx[j]; idx[j] = idx[j+1]; idx[j+1] = tmp;
            }

    printf("\n=== Alunos ordenados por Data de Nascimento ===\n");
    if (n == 0) { printf("  Nenhum aluno.\n"); return; }
    for (int i = 0; i < n; i++) imprimir_aluno(&alunos[idx[i]]);
}

void listar_professores_por_sexo(void) {
    char s;
    do {
        printf("Sexo (M/F): ");
        scanf(" %c", &s);
        limpar_buffer();
        s = (char)toupper((unsigned char)s);
        if (s != 'M' && s != 'F') printf("  Inválido.\n");
    } while (s != 'M' && s != 'F');

    printf("\n=== Professores %s ===\n", s == 'M' ? "Masculino" : "Feminino");
    int found = 0;
    for (int i = 0; i < num_professores; i++) {
        if (professores[i].ativo && professores[i].sexo == s) {
            imprimir_professor(&professores[i]); found = 1;
        }
    }
    if (!found) printf("  Nenhum professor encontrado.\n");
}

void listar_professores_por_nome(void) {
    int idx[MAX_PROFESSORES], n = 0;
    for (int i = 0; i < num_professores; i++)
        if (professores[i].ativo) idx[n++] = i;

    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (strcmp(professores[idx[j]].nome, professores[idx[j+1]].nome) > 0) {
                int tmp = idx[j]; idx[j] = idx[j+1]; idx[j+1] = tmp;
            }

    printf("\n=== Professores ordenados por Nome ===\n");
    if (n == 0) { printf("  Nenhum professor.\n"); return; }
    for (int i = 0; i < n; i++) imprimir_professor(&professores[idx[i]]);
}

void listar_professores_por_nascimento(void) {
    int idx[MAX_PROFESSORES], n = 0;
    for (int i = 0; i < num_professores; i++)
        if (professores[i].ativo) idx[n++] = i;

    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (comparar_datas(professores[idx[j]].nascimento, professores[idx[j+1]].nascimento) > 0) {
                int tmp = idx[j]; idx[j] = idx[j+1]; idx[j+1] = tmp;
            }

    printf("\n=== Professores ordenados por Data de Nascimento ===\n");
    if (n == 0) { printf("  Nenhum professor.\n"); return; }
    for (int i = 0; i < n; i++) imprimir_professor(&professores[idx[i]]);
}

void listar_aniversariantes(void) {
    printf("Mês (1-12): ");
    int mes;
    scanf("%d", &mes);
    limpar_buffer();
    if (mes < 1 || mes > 12) { printf("Mês inválido.\n"); return; }

    printf("\n=== Aniversariantes do mês %02d ===\n", mes);
    int found = 0;
    printf("  -- Alunos --\n");
    for (int i = 0; i < num_alunos; i++) {
        if (alunos[i].ativo && alunos[i].nascimento.mes == mes) {
            imprimir_aluno(&alunos[i]); found = 1;
        }
    }
    printf("  -- Professores --\n");
    for (int i = 0; i < num_professores; i++) {
        if (professores[i].ativo && professores[i].nascimento.mes == mes) {
            imprimir_professor(&professores[i]); found = 1;
        }
    }
    if (!found) printf("  Nenhum aniversariante neste mês.\n");
}

void buscar_por_string(void) {
    char termo[TAM_NOME];
    do {
        ler_string("Digite no mínimo 3 letras para busca: ", termo, TAM_NOME);
        if (strlen(termo) < 3) printf("  Mínimo de 3 caracteres.\n");
    } while (strlen(termo) < 3);

    /* Converte para minúsculo */
    char termo_lower[TAM_NOME];
    for (int i = 0; termo[i]; i++)
        termo_lower[i] = (char)tolower((unsigned char)termo[i]);
    termo_lower[strlen(termo)] = '\0';

    printf("\n=== Busca por \"%s\" ===\n", termo);
    int found = 0;

    printf("  -- Alunos --\n");
    for (int i = 0; i < num_alunos; i++) {
        if (!alunos[i].ativo) continue;
        char nome_lower[TAM_NOME];
        for (int j = 0; alunos[i].nome[j]; j++)
            nome_lower[j] = (char)tolower((unsigned char)alunos[i].nome[j]);
        nome_lower[strlen(alunos[i].nome)] = '\0';
        if (strstr(nome_lower, termo_lower)) {
            imprimir_aluno(&alunos[i]); found = 1;
        }
    }

    printf("  -- Professores --\n");
    for (int i = 0; i < num_professores; i++) {
        if (!professores[i].ativo) continue;
        char nome_lower[TAM_NOME];
        for (int j = 0; professores[i].nome[j]; j++)
            nome_lower[j] = (char)tolower((unsigned char)professores[i].nome[j]);
        nome_lower[strlen(professores[i].nome)] = '\0';
        if (strstr(nome_lower, termo_lower)) {
            imprimir_professor(&professores[i]); found = 1;
        }
    }

    if (!found) printf("  Nenhum resultado encontrado.\n");
}

void listar_alunos_poucas_disciplinas(void) {
    printf("\n=== Alunos matriculados em menos de 3 disciplinas ===\n");
    int found = 0;
    for (int i = 0; i < num_alunos; i++) {
        if (!alunos[i].ativo) continue;
        int count = 0;
        for (int d = 0; d < num_disciplinas; d++) {
            if (!disciplinas[d].ativo) continue;
            for (int j = 0; j < disciplinas[d].num_alunos; j++) {
                if (strcmp(disciplinas[d].alunos[j], alunos[i].matricula) == 0) {
                    count++; break;
                }
            }
        }
        if (count < 3) {
            printf("  ");
            imprimir_aluno(&alunos[i]);
            printf("    Disciplinas matriculadas: %d\n", count);
            found = 1;
        }
    }
    if (!found) printf("  Nenhum aluno nessa condição.\n");
}

void listar_disciplinas_cheias(void) {
    printf("\n=== Disciplinas com mais de 40 alunos ===\n");
    int found = 0;
    for (int i = 0; i < num_disciplinas; i++) {
        if (disciplinas[i].ativo && disciplinas[i].num_alunos > 40) {
            imprimir_disciplina_resumo(&disciplinas[i]);
            found = 1;
        }
    }
    if (!found) printf("  Nenhuma disciplina extrapolou 40 vagas.\n");
}

/* ===================== MENUS ===================== */

void menu_alunos(void) {
    int op;
    do {
        printf("\n--- Gerenciar Alunos ---\n");
        printf("1. Cadastrar\n2. Excluir\n3. Atualizar\n0. Voltar\n> ");
        scanf("%d", &op); limpar_buffer();
        switch (op) {
            case 1: cadastrar_aluno();   break;
            case 2: excluir_aluno();     break;
            case 3: atualizar_aluno();   break;
            case 0: break;
            default: printf("Opção inválida.\n");
        }
    } while (op != 0);
}

void menu_professores(void) {
    int op;
    do {
        printf("\n--- Gerenciar Professores ---\n");
        printf("1. Cadastrar\n2. Excluir\n3. Atualizar\n0. Voltar\n> ");
        scanf("%d", &op); limpar_buffer();
        switch (op) {
            case 1: cadastrar_professor();   break;
            case 2: excluir_professor();     break;
            case 3: atualizar_professor();   break;
            case 0: break;
            default: printf("Opção inválida.\n");
        }
    } while (op != 0);
}

void menu_disciplinas(void) {
    int op;
    do {
        printf("\n--- Gerenciar Disciplinas ---\n");
        printf("1. Cadastrar\n2. Excluir\n3. Atualizar\n");
        printf("4. Inserir aluno na disciplina\n5. Excluir aluno da disciplina\n0. Voltar\n> ");
        scanf("%d", &op); limpar_buffer();
        switch (op) {
            case 1: cadastrar_disciplina();       break;
            case 2: excluir_disciplina();         break;
            case 3: atualizar_disciplina();       break;
            case 4: inserir_aluno_disciplina();   break;
            case 5: excluir_aluno_disciplina();   break;
            case 0: break;
            default: printf("Opção inválida.\n");
        }
    } while (op != 0);
}

void menu_relatorios(void) {
    int op;
    do {
        printf("\n--- Relatórios ---\n");
        printf(" 1. Listar Alunos\n");
        printf(" 2. Listar Professores\n");
        printf(" 3. Listar Disciplinas (resumo)\n");
        printf(" 4. Listar uma Disciplina (com alunos)\n");
        printf(" 5. Alunos por sexo\n");
        printf(" 6. Alunos ordenados por nome\n");
        printf(" 7. Alunos ordenados por nascimento\n");
        printf(" 8. Professores por sexo\n");
        printf(" 9. Professores ordenados por nome\n");
        printf("10. Professores ordenados por nascimento\n");
        printf("11. Aniversariantes do mês\n");
        printf("12. Buscar pessoas por string\n");
        printf("13. Alunos em menos de 3 disciplinas\n");
        printf("14. Disciplinas com mais de 40 alunos\n");
        printf(" 0. Voltar\n> ");
        scanf("%d", &op); limpar_buffer();
        switch (op) {
            case  1: listar_alunos();                    break;
            case  2: listar_professores();               break;
            case  3: listar_disciplinas();               break;
            case  4: listar_disciplina_detalhe();        break;
            case  5: listar_alunos_por_sexo();           break;
            case  6: listar_alunos_por_nome();           break;
            case  7: listar_alunos_por_nascimento();     break;
            case  8: listar_professores_por_sexo();      break;
            case  9: listar_professores_por_nome();      break;
            case 10: listar_professores_por_nascimento(); break;
            case 11: listar_aniversariantes();           break;
            case 12: buscar_por_string();                break;
            case 13: listar_alunos_poucas_disciplinas(); break;
            case 14: listar_disciplinas_cheias();        break;
            case  0: break;
            default: printf("Opção inválida.\n");
        }
    } while (op != 0);
}

/* ===================== MAIN ===================== */

int main(void) {
    int op;
    printf("============================================\n");
    printf("  Sistema de Controle Escolar - IFBA/ADS\n");
    printf("============================================\n");
    do {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1. Gerenciar Alunos\n");
        printf("2. Gerenciar Professores\n");
        printf("3. Gerenciar Disciplinas\n");
        printf("4. Relatórios\n");
        printf("0. Sair\n> ");
        scanf("%d", &op); limpar_buffer();
        switch (op) {
            case 1: menu_alunos();       break;
            case 2: menu_professores();  break;
            case 3: menu_disciplinas();  break;
            case 4: menu_relatorios();   break;
            case 0: printf("Encerrando...\n"); break;
            default: printf("Opção inválida.\n");
        }
    } while (op != 0);
    return 0;
}
