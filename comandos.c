#include "proj.h"

/**
 * @file comandos.c
 * @author Beatriz Alves ist1114584
 * @date 2025-04-07
 * @brief Funçoes, comandos, principais do sistema .
 *
 */

/**
 * @brief Inicializa a estrutura do sistema com valores padrão
 * 
 * @param sys Ponteiro para a estrutura do sistema a ser inicializada
 */
void inicializar_sistema(Sys *sys) {
    sys->numero_inoculacoes = 0;
    sys->indice_vacinas = 0;
    sys->hashtable_inoculacoes = create_hashtable();
    sys->inoculacoes = NULL;
    sys->tail_inoculacoes = NULL;
    sys->numero_inoculacoes = 0;
    sys->count_ht = 0;
    inicializar_data_sistema(sys);
}

/**
 * @brief Configura a data inicial do sistema
 * 
 * @param sys Ponteiro para a estrutura do sistema
 */
void inicializar_data_sistema(Sys *sys){
    sys->data_sys.dia = 1;
    sys->data_sys.mes = 1;
    sys->data_sys.ano = 2025;
}

/**
 * @brief Processa o comando 'c' para registos de novos lotes
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param input String contendo o comando completo
 * @param lote Buffer para armazenar o lote validado
 * @param data_str Buffer para armazenar a data
 * @param doses Ponteiro para armazenar a quantidade de doses
 * @param nome Buffer para armazenar o nome da vacina
 * @param lote_temp Buffer temporário para leitura do lote
 * @param nome_temp Buffer temporário para leitura do nome
 */
void processar_comando_c(Sys *sys, char *input, char *lote, char *data_str, int *doses, char *nome, char *lote_temp, char *nome_temp) {
    if (sscanf(input, "c %s %10s %d %s", lote_temp, data_str, doses, nome_temp) == 4) {
        if (strlen(lote_temp) > MAXLOTE) {
            printf("%s\n", sys->comentarios[5]);
        }
        else if (strlen(nome_temp) > MAXNOME) {
            printf("%s\n", sys->comentarios[6]);
        }
        else {
            strcpy(lote, lote_temp);
            strcpy(nome, nome_temp);
            inserir_lote(sys, lote, data_str, *doses, nome);
        }
    }
}

/**
 * @brief Processa o comando 't' para alterar a data do sistema
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param input String contendo o comando completo
 * @param data_str Buffer para armazenar a data temporariamente
 */
void processar_comando_t(Sys *sys, char *input, char *data_str) {
    int dia, mes, ano;
    if (sscanf(input, "t %10s", data_str) == 1){
        if (!eh_formato_data_valido(data_str)) {
            printf("%s\n", sys->comentarios[3]);
            return;
        }
    }
    sscanf(data_str, "%d-%d-%04d", &dia, &mes, &ano);
    if (!eh_data(sys, dia, mes, ano)) {
        printf("%s\n", sys->comentarios[3]);
        return;
    }
    alterar_data_sistema(sys, dia, mes, ano);
    printf("%02d-%02d-%04d\n", dia, mes, ano);
}

/**
 * @brief Processa o comando 'l' para a listagem de vacinas
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param input String contendo o comando completo
 */
void processar_comando_l(Sys *sys, char *input) {
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) == 1) {
        listar_vacinas(sys);
        return;
    }

    char *token = strtok(input + 2, " ");
    while (token != NULL) {
        if (existe_nome(sys, token)) {
            listar_vacinas_com_nome(sys, token); 
        } else {
            printf("%s: %s\n", token, sys->comentarios[8]);
        }
        token = strtok(NULL, " ");
    }
}

/**
 * @brief Processa o comando 'a' para aplicação de vacinas
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param input String contendo o comando completo
 * @param temp_utente Buffer para nome do utente 
 * @param nome Buffer para nome da vacina
 */
void processar_comando_a(Sys *sys, char *input, char *temp_utente, char *nome) {
    input[strcspn(input, "\n")] = 0;

    if (input[2] == '"') {
        if (sscanf(input, "a \"%[^\"]\" %51s", temp_utente, nome) == 2) {
            aplica_vacina(sys, temp_utente, nome);
        }
    } else {
        if (sscanf(input, "a %65532s %51s", temp_utente, nome) == 2) {
            aplica_vacina(sys, temp_utente, nome);
        }
    }
}

/**
 * @brief Processa o comando 'r' para remover lotes de vacina
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param input String contendo o comando completo
 * @param lote Buffer para armazenar o lote a ser removido
 */
void processar_comando_r(Sys *sys, char *input, char *lote){
    input[strcspn(input, "\n")] = 0;
    if (sscanf(input, "r %21s", lote) == 1){
        retirar_vacina(sys, lote);
    }
}

/**
 * @brief Processa o comando 'u' para a listagem de inoculações
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param input String contendo o comando completo
 * @param temp_utente Buffer para armazenar nome do utente
 */
void processar_comando_u (Sys *sys, char *input, char *temp_utente) {
    input[strcspn(input, "\n")] = 0;
    if (input[2] == '"') {
        if (sscanf(input, "u \"%[^\"]\" ", temp_utente) == 1) {
            listar_inoculacao_utente(sys, temp_utente);
        }
    } else  if (input[2] == '\0') {
            listar_inoculacoes_todas(sys);
    } else {
        if (sscanf(input, "u %65532s ", temp_utente) == 1) {
            listar_inoculacao_utente(sys, temp_utente);

        }
    }
}

/**
 * @brief Processa o comando 'd' para remover inoculações
 * 
 * @param sys Ponteiro para a estrutura do sistema
 * @param input String contendo o comando completo
 * @param temp_utente Buffer para nome do utente
 * @param lote_temp Buffer para lote da vacina
 * @param data_str Buffer para data da inoculação
 */
void processar_comando_d(Sys *sys, char *input, char *temp_utente, char *lote_temp, char *data_str) {
    input[strcspn(input, "\n")] = 0;
    int dia, mes, ano;
    int com_aspas = (input[2] == '"');
    int campos_lidos = 0;

    if (com_aspas) campos_lidos = sscanf(input, "d \"%[^\"]\" %10s %s", temp_utente, data_str, lote_temp);
    else campos_lidos = sscanf(input, "d %65000s %10s %s", temp_utente, data_str, lote_temp);

    int tem_lote = (campos_lidos == 3);
    int tem_data = (campos_lidos == 2);

    if (tem_lote) {
        if (!verificar_data(sys, data_str, &dia, &mes, &ano)) {
            printf("%s\n", sys->comentarios[3]);
            return;
        }
        if (!verifica_lote(sys, lote_temp, temp_utente)) {
            printf("%s: %s\n", lote_temp, sys->comentarios[11]);
            return;
        }
        eliminar_inoculacao_hashtable(sys, temp_utente, lote_temp, dia, mes, ano);
    } else if (tem_data) {
        if (!verificar_data(sys, data_str, &dia, &mes, &ano)) {
            printf("%s\n", sys->comentarios[3]);
            return;
        }
        eliminar_inoculacao_hashtable_sem_lote(sys, temp_utente, dia, mes, ano);
    } else {
        eliminar_inoculacao_hashtable_apenas_utente(sys, temp_utente);
    }
}



