#include "proj.h"

/**
 * @file vacina.c
 * @author Beatriz Alves ist1114584
 * @date 2025-04-07
 * @brief Funçoes do sistema relacionadas com os lotes de vacinas.
 *
 */

/**
 * @brief Insere um novo lote de vacina no sistema
 * 
 * Adiciona um novo registro de vacina ao array de vacinas do sistema,
 * inicializando todos os campos e mantendo a lista ordenada.
 * 
 * @param sys Ponteiro para a estrutura principal do sistema
 * @param lote Código do lote da vacina 
 * @param dia Dia da validade da vacina
 * @param mes Mês da validade da vacina
 * @param ano Ano da validade da vacina
 * @param doses Quantidade de doses disponíveis
 * @param nome Nome da vacina
 */
void inserir_vacina(Sys *sys, char *lote, int dia, int mes, int ano, int doses, char *nome) {
    Lotevacina *v = &sys->vacinas[sys->indice_vacinas];
    
    strcpy(v->lote, lote);
    strcpy(v->nome, nome);
    v->validade.dia = dia;
    v->validade.mes = mes;
    v->validade.ano = ano;
    v->dose = doses;
    v->aplicacoes = 0;
    v->apagado = 0;
    

    ordenar_vacinas(sys->vacinas, 0, sys->indice_vacinas);
    sys->indice_vacinas++;
    
    printf("%s\n", lote);
}

/**
 * @brief Insere um novo lote de vacina no sistema após validações
 * 
 * Realiza uma série de validações antes de inserir um novo lote de vacina:
 * 1. Capacidade do sistema
 * 2. Formato da data
 * 3. Unicidade do lote
 * 4. Formato do lote
 * 5. Nome da vacina
 * 6. Validade da data
 * 7. Quantidade de doses
 * 
 * @param sys Ponteiro para a estrutura principal do sistema
 * @param lote Código do lote da vacina
 * @param data_str Data de validade
 * @param doses Número de doses disponíveis no lote
 * @param nome Nome da vacina
 */
void inserir_lote(Sys *sys, char *lote, char *data_str, int doses, char *nome) {
    int dia, mes, ano;
    
    if (!validar_capacidade_sistema(sys)) return;
    if (!validar_formato_data(sys, data_str)) return;
    
    sscanf(data_str, "%d-%d-%04d", &dia, &mes, &ano);
    
    if (!validar_lote_unico(sys, lote)) return;
    if (!validar_formato_lote(sys, lote)) return;
    if (!validar_nome_vacina(sys, nome)) return;
    if (!validar_data(sys, dia, mes, ano)) return;
    if (!validar_quantidade_doses(sys, doses)) return;
    
    inserir_vacina(sys, lote, dia, mes, ano, doses, nome);
}

/**
 * @brief Altera a data atual do sistema
 * 
 * Atualiza a data do sistema para os valores fornecidos, modificando diretamente
 * a estrutura sys->data_sys. Esta data é utilizada como referência para todas
 * as operações do sistema que necessitam da data atual.
 * 
 * @param sys Ponteiro para a estrutura principal do sistema
 * @param dia Novo dia do sistema 
 * @param mes Novo mês do sistema 
 * @param ano Novo ano do sistema 
 */
void alterar_data_sistema(Sys *sys, int dia, int mes, int ano) {
    sys->data_sys.dia = dia;
    sys->data_sys.mes = mes;
    sys->data_sys.ano = ano;
}

/**
 * @brief Ordena o array de vacinas usando insertion sort modificado
 * 
 * Ordena os lotes de vacinas primeiro por data de validade (ascendente)
 * e depois por número de lote (ascendente) quando as datas forem iguais.
 * 
 * @param vacina Array de estruturas Lotevacina a ser ordenado
 * @param left Índice inicial do intervalo a ordenar (inclusivo)
 * @param right Índice final do intervalo a ordenar (inclusivo)
 */
void ordenar_vacinas(Lotevacina vacina[], int left, int right) {
    // Percorre o array da posição left+1 até right
    for (int i = left + 1; i <= right; i++) {
        // Guarda o elemento atual numa variável temporária
        Lotevacina v = vacina[i];
        // Inicia a comparação com o elemento anterior
        int j = i - 1;
        int cmp;

        while (j >= left) {
            cmp = compara_datas(v.validade, vacina[j].validade);
            // Se a validade for menor OU (se for igual E o lote for menor)
            if (cmp < 0 || (cmp == 0 && compara_lotes(v.lote, vacina[j].lote) < 0)) {
                // Move o elemento para a direita para abrir espaço
                vacina[j + 1] = vacina[j];
                j--; // Avança para o elemento anterior
            } else {
                // Se encontrarmos a posição correta, saímos do loop
                break;
            }
        }
        // posiçao correta, cmp > 0, a validade de v é posterior à do elemento vacina[j]
        // As validades são iguais, mas o lote de v é maior ou igual.
        vacina[j + 1] = v;
    }
}

/**
 * @brief Lista todas as vacinas do sistema
 * 
 * Imprime na saída todas as vacinas ativas no sistema, mostrando:
 * - Nome da vacina
 * - Número do lote
 * - Data de validade (formato DD-MM-AAAA)
 * - Quantidade de doses disponíveis
 * - Número de aplicações realizadas
 * 
 * @param sys Ponteiro para a estrutura principal do sistema
 */
void listar_vacinas(Sys *sys) {
    for (int i = 0; i < sys->indice_vacinas; i++) {
        printf("%s %s %02d-%02d-%04d %d %d\n", sys->vacinas[i].nome, sys->vacinas[i].lote, 
               sys->vacinas[i].validade.dia, sys->vacinas[i].validade.mes, sys->vacinas[i].validade.ano, 
               sys->vacinas[i].dose, sys->vacinas[i].aplicacoes); 
    }
}

/**
 * @brief Lista todas as vacinas do sistema com o mesmo nome recebido
 * 
 * Imprime na saída todas as vacinas ativas no sistema, mostrando:
 * - Nome da vacina
 * - Número do lote
 * - Data de validade (formato DD-MM-AAAA)
 * - Quantidade de doses disponíveis
 * - Número de aplicações realizadas
 * 
 * @param sys Ponteiro para a estrutura principal do sistema
 */
void listar_vacinas_com_nome(Sys *sys, char *nome) {
    for (int i = 0; i < sys->indice_vacinas; i++) {
        if (strcmp(sys->vacinas[i].nome, nome) == 0) {
            printf("%s %s %02d-%02d-%04d %d %d\n", sys->vacinas[i].nome, sys->vacinas[i].lote, 
                    sys->vacinas[i].validade.dia, sys->vacinas[i].validade.mes, sys->vacinas[i].validade.ano, 
                    sys->vacinas[i].dose, sys->vacinas[i].aplicacoes);
        }
    }
}

/**
 * @brief Remove um lote de vacina do sistema
 * 
 * Esta função realiza a remoção de um lote de vacina do sistema de acordo com duas situações:
 * 1. Se não houve aplicações: remove fisicamente o lote do array
 * 2. Se houve aplicações: marca como apagado (apagado=1) e zera as doses disponíveis (dose=0)
 * 
 * @param sys Ponteiro para a estrutura principal do sistema
 * @param lote Número do lote a ser removido
 */
void retirar_vacina(Sys * sys, char *lote) {
    int i, j, lote_encontrado = 0, lote_index = - 1;
    for (i = 0; i < sys->indice_vacinas; i++){
        if(strcmp(sys->vacinas[i].lote, lote) == 0){
            lote_encontrado = 1;
            lote_index = i;
            break;
        }
    }
    if (lote_encontrado == 0){
        printf("%s: %s\n", lote, sys->comentarios[11]);
        return;
    }
    if(sys->vacinas[lote_index].aplicacoes == 0){
        for (j = lote_index; j < sys->indice_vacinas - 1; j++)
				sys->vacinas[j] = sys->vacinas[j + 1]; // Cada posição j recebe o valor da posição seguinte (j + 1).
			sys->indice_vacinas--;
    }
    else if(sys->vacinas[lote_index].aplicacoes > 0){
        sys->vacinas[lote_index].dose = 0;
        sys->vacinas[lote_index].apagado = 1;
    }
    printf("%d\n", sys->vacinas[lote_index].aplicacoes);
}