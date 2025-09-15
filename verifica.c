#include "proj.h"
/**
 * @file utente.c
 * @author Beatriz Alves ist1114584
 * @date 2025-04-07
 * @brief Funçoes do sistema relacionadas com verificações.
 *
 */


/**
 * @brief Verifica se uma alocação de memória foi bem-sucedida
 * 
 * Esta função verifica ponteiros retornados por funções
 * de alocação de memória (malloc, calloc, etc). Em caso de falha:
 * 1. Exibe mensagem de erro do sistema
 * 2. Encerra o programa com EXIT_FAILURE
 * 
 * @param sys Ponteiro para a estrutura principal do sistema (para mensagens)
 * @param ptr Ponteiro a ser verificado (retorno de malloc/calloc/etc)
 * @return Retorna o mesmo ponteiro recebido se a alocação foi bem-sucedida
 */
void* verifica_memoria(Sys *sys, void* ptr) {
    if (ptr == NULL) {
        printf("%s\n",sys->comentarios[1]);
        exit(EXIT_FAILURE);
    }
    return ptr;
}

/**
 * @brief Verifica se uma string possui o formato de uma data
 * 
 * Esta função realiza uma verificação do formato de uma string
 * que representa uma data, analisando apenas o comprimento da string.
 * 
 * @param data_str String contendo a data a ser verificada
 * @return 1 se o comprimento está dentro dos limites aceitáveis, 0 caso contrário
 */
int eh_formato_data_valido(const char *data_str) {
    int len = strlen(data_str);
    if (len < 8 || len > 10) {
        return 0;
    }
    return 1;
}

/**
 * @brief Verifica se uma string representa um lote válido
 * 
 * Valida se a string contém apenas caracteres hexadecimais maiúsculos
 * 
 * @param lote String a ser validada
 * @return 1 se o lote é válido, 0 caso contrário
 */
int eh_lote(char *lote){
    int len = strlen(lote);
    for (int i = 0; i < len; i++){
        if (!isxdigit(lote[i]) || islower(lote[i]))
            return 0;
    }
    return 1;
}

/**
 * @brief Verifica se um nome de vacina é válido
 * 
 * Valida se a string contém apenas caracteres sem espaços, tabs ou quebras de linha
 * 
 * @param nome String contendo o nome a ser validado
 * @return 1 se o nome é válido, 0 caso contrário
 */
int eh_nome(char *nome) {
    int len = strlen(nome);
    for (int i = 0; i < len; i++) {
        if (nome[i] == ' ' || nome[i] == '\n' || nome[i] == '\t' ) {
            return 0;
        }
    }

    return 1;
}

/**
 * @brief Verifica se uma data é válida e não está expirada
 * 
 * Realiza duas verificações:
 * 1. Se a data é posterior ou igual à data atual do sistema
 * 2. Se a data é cronologicamente válida (dias/meses corretos)
 * 
 * @param sys Ponteiro para a estrutura do sistema (contém data atual)
 * @param dia Dia a ser verificado
 * @param mes Mês a ser verificado
 * @param ano Ano a ser verificado
 * @return 1 se a data é válida e não expirada, 0 caso contrário
 */
int eh_data(Sys *sys, int dia, int mes, int ano) {
    if (ano < sys->data_sys.ano || 
        (ano == sys->data_sys.ano && mes < sys->data_sys.mes) || 
        (ano == sys->data_sys.ano && mes == sys->data_sys.mes && dia < sys->data_sys.dia)) {
        return 0;
    }
    if(!eh_data_valida(dia, mes, ano)) return 0;
    else return 1;
}

/**
 * @brief Verifica se o sistema possui capacidade para mais vacinas
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @return 1 se há espaço disponível, 0 caso contrário
 */
int validar_capacidade_sistema(Sys *sys) {
    if (sys->indice_vacinas == MAXVACINAS) {
        printf("%s\n", sys->comentarios[2]);
        return 0;
    }
    return 1;
}

/**
 * @brief Valida o formato básico de uma string de data
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param data_str String no formato "DD-MM-AAAA"
 * @return 1 se o formato é válido, 0 caso contrário
 */
int validar_formato_data(Sys *sys, char *data_str) {
    if (!eh_formato_data_valido(data_str)) {
        printf("%s\n", sys->comentarios[3]);
        return 0;
    }
    return 1;
}

/**
 * @brief Verifica se um lote já existe no sistema
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param lote Código do lote a verificar
 * @return 1 se o lote é único, 0 se já existe
 */
int validar_lote_unico(Sys *sys, char *lote) {
    for (int i = 0; i < sys->indice_vacinas; i++) {
        if (strcmp(sys->vacinas[i].lote, lote) == 0) {
            printf("%s\n", sys->comentarios[4]);
            return 0;
        }
    }
    return 1;
}

/**
 * @brief Valida o formato de um código de lote
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param lote Código do lote a validar
 * @return 1 se o formato é válido, 0 caso contrário
 */
int validar_formato_lote(Sys *sys, char *lote) {
    if (!eh_lote(lote)) {
        printf("%s\n", sys->comentarios[5]);
        return 0;
    }
    return 1;
}

/**
 * @brief Valida o formato do nome de uma vacina
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param nome Nome a ser validado
 * @return 1 se válido, 0 caso contrário
 */
int validar_nome_vacina(Sys *sys, char *nome) {
    if (!eh_nome(nome)) {
        printf("%s\n", sys->comentarios[6]);
        return 0;
    }
    int len = strlen(nome);
    for (int i = 0; i < len; i++) {
        if (nome[i] == '_' ) {
            printf("invalid character in vaccine name\n");
            return 0;
        }
    }
    return 1;
}

/**
 * @brief Valida uma data considerando o sistema
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param dia Dia a validar
 * @param mes Mês a validar
 * @param ano Ano a validar
 * @return 1 se a data é válida e não expirada, 0 caso contrário
 */
int validar_data(Sys *sys, int dia, int mes, int ano) {
    if (!eh_data(sys, dia, mes, ano)) {
        printf("%s\n", sys->comentarios[3]);
        return 0;
    }
    return 1;
}

/**
 * @brief Valida o número de doses de um lote
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param doses Quantidade de doses a validar
 * @return 1 se válido (doses > 0), 0 caso contrário
 */
int validar_quantidade_doses(Sys *sys, int doses) {
    if (doses <= 0) {
        printf("%s\n", sys->comentarios[7]);
        return 0;
    }
    return 1;
}

/**
 * @brief Compara duas datas cronologicamente
 * 
 * @param d1 Primeira data a comparar
 * @param d2 Segunda data a comparar
 * @return Diferença entre as datas:
 *         - >0 se d1 é mais recente que d2
 *         - 0 se as datas são iguais
 *         - <0 se d1 é mais antiga que d2
 */
int compara_datas(Data d1, Data d2) {
    if (d1.ano != d2.ano)
        return d1.ano - d2.ano;
    if (d1.mes != d2.mes)
        return d1.mes - d2.mes;
    return d1.dia - d2.dia;
}

/**
 * @brief Compara dois códigos de lote com regras específicas
 * 
 * @param lote1 Primeiro lote a comparar
 * @param lote2 Segundo lote a comparar
 * @return Resultado da comparação:
 *         - -1 se lote1 começa com dígito e lote2 não
 *         - 1 se lote2 começa com dígito e lote1 não
 *         - strcmp(lote1, lote2) caso contrário
 */
int compara_lotes(const char *lote1, const char *lote2) {
    int lote1_numero = (isdigit(lote1[0])) ? 1 : 0;
    int lote2_numero = (isdigit(lote2[0])) ? 1 : 0;

    if (lote1_numero && !lote2_numero) {
        return -1;
    }
    else if (!lote1_numero && lote2_numero) {
        return 1;
    }
    else {
        return strcmp(lote1, lote2);
    }
}

/**
 * @brief Verifica se um nome de vacina já existe no sistema
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param nome Nome da vacina a verificar
 * @return 1 se o nome existe, 0 caso contrário
 */
int existe_nome(Sys *sys, char *nome){
    for (int i = 0; i < sys->indice_vacinas; i++) {
        if (strcmp(sys->vacinas[i].nome, nome) == 0)
            return 1;
    }
    return 0;
}

/**
 * @brief Verifica e converte uma string de data no formato "DD-MM-AAAA"
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param data_str String contendo a data no formato "DD-MM-AAAA"
 * @param dia Ponteiro para armazenar o dia extraído
 * @param mes Ponteiro para armazenar o mês extraído
 * @param ano Ponteiro para armazenar o ano extraído
 * @return 1 se a data é válida e anterior à data do sistema, 0 caso contrário
 */
int verificar_data(Sys *sys, char *data_str, int *dia, int *mes, int *ano) {
    if (!eh_formato_data_valido(data_str)) {
        return 0;
    }

    if (sscanf(data_str, "%d-%d-%04d", dia, mes, ano) != 3) {
        return 0;
    }

    if (!eh_data_comando_d(sys, *dia, *mes, *ano)) {
        return 0;
    }

    return 1;
}


/**
 * @brief Verifica se uma data é válida e anterior à data do sistema
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param dia Dia a verificar
 * @param mes Mês a verificar
 * @param ano Ano a verificar
 * @return 1 se a data é válida e anterior à data do sistema, 0 caso contrário
 */
int eh_data_comando_d (Sys *sys, int dia, int mes, int ano) {
    if (ano > sys->data_sys.ano || 
        (ano == sys->data_sys.ano && mes > sys->data_sys.mes) || 
        (ano == sys->data_sys.ano && mes == sys->data_sys.mes && dia > sys->data_sys.dia)) {
        return 0;
    }
    if(!eh_data_valida(dia, mes, ano)) return 0;
    else return 1;
}

/**
 * @brief Verifica se um lote foi aplicado a um utente específico
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param lote Código do lote a verificar
 * @param utente Nome do utente a verificar
 * @return 1 se o lote foi aplicado ao utente, 0 caso contrário
 */
int verifica_lote(Sys *sys, char *lote, char *utente) {
    unsigned int key = hash(utente) % sys->hashtable_inoculacoes->size;
    Entry *entry = sys->hashtable_inoculacoes->entries[key];
    
    while (entry != NULL) {
        if (strcmp(entry->utente, utente) == 0) {
            Lista_inoculacoes *lista = entry->inocs;
            while (lista != NULL) {
                if (strcmp(lista->inoc->lote, lote) == 0) {
                    return 1;
                }
                lista = lista->next;
            }
            break;
        }
        entry = entry->next;
    }
    return 0;
}

/**
 * @brief Verifica se uma data é cronologicamente válida
 * 
 * @param dia Dia a verificar (1-31)
 * @param mes Mês a verificar (1-12)
 * @param ano Ano a verificar (4 dígitos)
 * @return 1 se a data é válida, 0 caso contrário
 */
int eh_data_valida(int dia, int mes, int ano) {
    if (mes < 1 || mes > 12) {
        return 0;
    }
    int dias_no_mes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (mes == 2) {
        int is_leap = (ano % 4 == 0 && (ano % 100 != 0 || ano % 400 == 0));
        dias_no_mes[1] = is_leap ? 29 : 28;
    }
    if (dia < 1 || dia > dias_no_mes[mes - 1]) {
        return 0;
    }
    return 1;
}
