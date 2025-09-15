#include "proj.h"

/**
 * @file utente.c
 * @author Beatriz Alves ist1114584
 * @date 2025-04-07
 * @brief Função principal do sistema
 */

/**
 * @brief Função principal do sistema de gestão de vacinas
 * 
 * @param argc Número de argumentos de linha de comando
 * @param argv Array de argumentos de linha de comando
 * @return int Código de saída do programa (0 para sucesso)
 * 
 * 1. Inicializa o sistema
 * 2. Configura idioma (português/inglês)
 * 3. Processa comandos de entrada até receber 'q'
 * 4. Liberta memória e termina
 * 
 * Comandos:
 * - q: Sair
 * - c: Cadastrar lote
 * - t: Alterar data
 * - l: Listar vacinas
 * - a: Aplicar vacina
 * - r: Remover lote
 * - u: Listar inoculações
 * - d: Remover inoculação
 */
int main(int argc, char *argv[]) {
    Sys sys;
    inicializar_sistema(&sys);
    const char *pt[] = {UTEINEX,SEMMEM,DEMAVACS,DATAINV,LOTEDUP,LOTEINV,NOMEINV,QUANTINV,VACINEX,ESGOTADO,JAVAC,LOTEINEX};
    const char *en[] = {NOUSER,NOMEMORY,MANYVACS,INVDATE,DUPBATCH,INVBATCH,INVNAME,INVQUANT,NOVACC,NOSTOCK,ALRVACC,NOBATCH};
    if (argc > 1 && strcmp(argv[1], "pt") == 0) 
        sys.comentarios = pt;
    else 
        sys.comentarios = en;
    char lote[MAXLOTE + 2], lote_temp[MAXLOTE + 2], nome[MAXNOME + 2], nome_temp[MAXNOME + 2];
    char temp_utente[MAXUTENTE], input[INPUTMAX], data_str[MAXDATA + 1];
    int doses;


    while (fgets(input, INPUTMAX, stdin)) {
        switch (input[0]) {
            case 'q': liberta_lista_inoculacoes(sys.inoculacoes); liberta_hashtable_inoculacoes(sys.hashtable_inoculacoes); return 0;
            case 'c': processar_comando_c (&sys, input, lote, data_str, &doses, nome, lote_temp, nome_temp); break;
            case 't': processar_comando_t(&sys, input, data_str); break;
            case 'l': processar_comando_l(&sys,input); break;
            case 'a': processar_comando_a(&sys, input, temp_utente, nome); break;
            case 'r': processar_comando_r(&sys, input, lote); break;
            case 'u': processar_comando_u(&sys, input, temp_utente); break;
            case 'd':processar_comando_d(&sys, input, temp_utente, lote_temp, data_str);
            default : break;
        }
    }
    liberta_lista_inoculacoes(sys.inoculacoes);           
    liberta_hashtable_inoculacoes(sys.hashtable_inoculacoes);
    return 0;
}