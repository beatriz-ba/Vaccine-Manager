#include "proj.h"

/**
 * @file utente.c
 * @author Beatriz Alves ist1114584
 * @date 2025-04-07
 * @brief Funçoes do sistema relacionadas com os utentes.
 *
 */


/**
 * @brief Cria e inicializa uma hashtable.
 *
 * Aloca memória para a hashtable e inicializa todos os ponteiros
 * para entradas NULL. A hashtable tem um tamanho definido por TABLESIZE.
 *
 * @return Ponteiro para a estrutura Hashtable criada, ou NULL se a alocação falhar.
 */
Hashtable* create_hashtable(void){
    Hashtable *ht = malloc(sizeof(Hashtable));
    ht->entries = malloc(sizeof(Entry*) * TABLESIZE);
    ht->size = TABLESIZE;
    int i = 0;
    for (; i < TABLESIZE; i++) {
        ht->entries[i] = NULL;
    }
    return ht;
}

/**
 * @brief Calcula o valor de hash de uma string usando o algoritmo FNV-1a.
 *
 * Esta função recebe uma string e devolve um número do tipo unsigned long
 * que representa o valor de hash dessa string.
 *
 * @param str string a ser transformada em hash.
 * @return Valor de hash correspondente à string fornecida.
 */
unsigned long hash(const char *str) {
    unsigned long hash = 14695981039346656037UL;
    
    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= 1099511628211UL;
    }
    
    return hash;
}

/**
 * @brief Insere um registo de inoculação na hashtable do sistema.
 *
 * Esta função verifica se a hashtable precisa de ser redimensionada antes de inserir.
 * Em seguida, calcula a posição da tabela usando a função de hash do nome do utente.
 * Se o utente já existir na hashtable, adiciona a nova inoculação à sua lista.
 * Caso contrário, cria uma nova entrada para o utente e insere a inoculação.
 *
 * A função também atualiza a lista global de inoculações no sistema.
 *
 * @param sys Ponteiro para a estrutura principal do sistema.
 * @param inoculacao Ponteiro para a estrutura, do tipo inoculação, a ser inserida.
 */
void inserir_hashtable(Sys *sys, Inoculacao *inoculacao) {
    if (sys->count_ht >= sys->hashtable_inoculacoes->size * 0.5) redimensionar_hashtable(sys);
    Hashtable *ht = sys->hashtable_inoculacoes;
    if (inoculacao->utente == NULL) return;
    unsigned int key = hash(inoculacao->utente) % ht->size;
    Entry *entry = ht->entries[key]; Entry *prev = NULL;
    // Percorre a hashtable  procurando pelo utente
    while (entry != NULL) {
        if (strcmp(entry->utente, inoculacao->utente) == 0) {
            break;
        }
        prev = entry;
        entry = entry->next;
    }
    /* Criação de nova entrada se utente não existir */
    if (entry == NULL) {
        entry = verifica_memoria(sys, malloc(sizeof(Entry)));
        entry->utente = verifica_memoria(sys, strdup(inoculacao->utente));
        entry->inocs = NULL; 
        entry->tail_inocs = NULL;
        entry->next = NULL;
        // Insere na posição correta da lhashtable
        if (prev == NULL) {
            // Insere no início da lista (caso seja o primeiro elemento)
            ht->entries[key] = entry;
        } else {
            // Insere no final da lista (mantém ordem de chegada)
            prev->next = entry;
        }
        sys->count_ht++;
    }
    inserir_inoculacao_lista(sys, &entry->inocs, &entry->tail_inocs, inoculacao);
    sys->numero_inoculacoes++;
    inserir_inoculacao_lista(sys, &sys->inoculacoes, &sys->tail_inoculacoes, inoculacao);
}

/**
 * @brief Insere uma inoculação no final de uma lista ligada de inoculações.
 *
 * Esta função cria um novo nó com a inoculação fornecida e adiciona-o
 * no fim da lista ligada. Se a lista estiver vazia, o novo nó torna-se o
 * primeiro e o último elemento.
 *
 * @param sys Ponteiro para a estrutura principal do sistema, usado para verificação de memória.
 * @param head Ponteiro para o início da lista ligada de inoculações.
 * @param tail Ponteiro para o fim da lista ligada de inoculações.
 * @param inoculacao Ponteiro para a inoculação a ser inserida.
 */
void inserir_inoculacao_lista(Sys *sys, Lista_inoculacoes **head, Lista_inoculacoes **tail, Inoculacao *inoculacao) {
    Lista_inoculacoes *new = verifica_memoria(sys, malloc(sizeof(Lista_inoculacoes)));
    new->inoc = inoculacao;
    new->next = *head;
    *head = new;
    
    if (*tail == NULL) {
        *tail = new;
    }
}

/**
 * @brief Procura um utente na hashtable e devolve a entrada correspondente.
 *
 * Esta função calcula a chave hash com base no utente
 * e percorre a lista ligada da posição correspondente na hashtable para encontrar
 * a entrada com o utente indicado.
 *
 * @param sys Ponteiro para a estrutura principal do sistema.
 * @param utente String correspondente ao utente a procurar.
 * @return Ponteiro para a entrada da hashtable correspondente ao utente, ou NULL se não for encontrada.
 */
Entry* encontrar_hashtable (Sys *sys, char *utente) {
    Hashtable *ht = sys->hashtable_inoculacoes;
    unsigned int key = hash(utente) % ht->size;

    Entry *entry = ht->entries[key];
    while (entry != NULL) {
        if(strcmp(entry->utente, utente) == 0 ){
            return entry;
        }
        entry = entry->next;
    }
    return NULL;
}

/**
 * @brief Lista todas as inoculações associadas a um determinado utente.
 *
 * A função procura na hashtable a entrada correspondente ao utente. 
 * No caso de não existir nenhuma aplicação de vacina ao utente indicado
 * imprime uma mensagem de erro.
 * Caso existam inoculações associadas, imprime cada uma com o formato:
 * utente lote dd-mm-aaaa.
 *
 * @param sys Ponteiro para a estrutura principal do sistema.
 * @param utente String correspondente ao identificador do utente.
 */
void listar_inoculacao_utente (Sys *sys, char *utente) {
    Entry *entry = encontrar_hashtable(sys,utente);

    if (entry == NULL) {
        printf("%s: %s\n", utente, sys->comentarios[0]);
        return;
    }

    Lista_inoculacoes *curr = entry->inocs;
    if (curr == NULL) {
        printf("%s: %s\n", utente, sys->comentarios[0]);
        return;
    }
    while (curr != NULL) {
        printf("%s %s %02d-%02d-%04d\n", curr->inoc->utente, curr->inoc->lote, curr->inoc->validade.dia, 
        curr->inoc->validade.mes, curr->inoc->validade.ano);

        curr = curr->next;
    }
}

/**
 * @brief Lista todas as inoculações registadas no sistema.
 *
 * A função percorre a lista global de inoculações do sistema e imprime
 * cada registo no formato:
 * utente lote dd-mm-aaaa.
 * 
 * Se não existirem inoculações registadas, não imprime nada.
 *
 * @param sys Ponteiro para a estrutura principal do sistema.
 */
void listar_inoculacoes_todas (Sys *sys) {
    Lista_inoculacoes *curr = sys->inoculacoes;
    if (curr == NULL) {
        return;
    }

    while(curr != NULL) {
        printf("%s %s %02d-%02d-%04d\n", curr->inoc->utente, curr->inoc->lote, curr->inoc->validade.dia, 
        curr->inoc->validade.mes, curr->inoc->validade.ano);

        curr= curr->next;
    }
}

/**
 * @brief Redimensiona a hashtable para um tamanho maior
 * 
 * Aumenta o tamanho da hashtable para mais AUTABLE posições e realoca
 * todas as entradas existentes para a nova hashtable. Esta operação
 * é necessária para manter a eficiência da hashtable quando
 * o número de elementos cresce.
 * 
 * @param sys Ponteiro para a estrutura do sistema que contém
 *             a hashtable a ser redimensionada
 */
void redimensionar_hashtable(Sys *sys) {
    Hashtable *ht = sys->hashtable_inoculacoes;
    size_t new_size = ht->size + AUTABLE;
    

    Entry **new_entries = verifica_memoria(sys, calloc(new_size, sizeof(Entry*)));
    if (!new_entries) return;
    
    for (size_t i = 0; i < ht->size; i++) {
        Entry *entry = ht->entries[i];
        while (entry) {
            Entry *next = entry->next; 
            
            unsigned long hashed = hash(entry->utente);
            unsigned int new_key = hashed % new_size;
            
            entry->next = new_entries[new_key];
            new_entries[new_key] = entry;
            
            entry = next; 
        }
    }
    
    free(ht->entries);
    ht->entries = new_entries;
    ht->size = new_size;
}

/**
 * @brief Liberta a memória alocada para uma estrutura Inoculacao
 * 
 * Esta função liberta toda a memória alocada para uma estrutura Inoculacao,
 * incluindo a string do utente e a própria estrutura. Verifica se o ponteiro
 * é válido antes de efetuar a libertação.
 * 
 * @param inoc Ponteiro para a estrutura Inoculacao a ser libertada
 */
void liberta_inoculacao(Inoculacao *inoc) {
    if (inoc == NULL) return;
    free(inoc->utente);
    free(inoc);
}

/**
 * @brief Liberta toda a memória alocada para uma lista ligada de inoculações
 * 
 * Percorre a lista ligada de inoculações e liberta a memoria alocada 
 * cada nó e a estrutura Inoculacao associada a cada um.
 * 
 * @param head Ponteiro para o primeiro elemento da lista ligada (cabeça da lista)
 */
void liberta_lista_inoculacoes(Lista_inoculacoes *head) {
    Lista_inoculacoes *curr = head;
    while (curr != NULL) {
        Lista_inoculacoes *temp = curr;
        curr = curr->next;

        liberta_inoculacao(temp->inoc);
        free(temp);                  
    }
}

/**
 * @brief Liberta toda a memória alocada para a lista de nós de inoculações.
 *
 * A função percorre a lista de nós de inoculações e liberta a memória
 * alocada para cada nó. Ela não liberta os dados contidos no nó, apenas
 * os próprios nós da lista.
 * 
 * @param head Apontador para o primeiro nó da lista de inoculações a ser libertada.
 */
void liberta_lista_nos_inoculacoes(Lista_inoculacoes *head) {
    Lista_inoculacoes *curr = head;
    while (curr != NULL) {
        Lista_inoculacoes *temp = curr;
        curr = curr->next;
        free(temp);  
    }
}

/**
 * @brief Liberta toda a memória alocada para a hashtable de inoculações.
 *
 * A função percorre a hashtable e liberta a memória de cada entrada,
 * incluindo os dados associados a cada `Entry`, como o nome do utente e
 * a lista de inoculações. Após a liberação de todos os elementos, a própria
 * hashtable é libertada.
 * 
 * @param ht Ponteiro para a tabela hash a ser libertada.
 */
void liberta_hashtable_inoculacoes(Hashtable *ht) {
    for (size_t i = 0; i < ht->size; i++) {
        Entry *entry = ht->entries[i];
        while (entry != NULL) {
            Entry *temp = entry;
            entry = entry->next;
            free(temp->utente);
            liberta_lista_nos_inoculacoes(temp->inocs);
            free(temp);
        }
    }
    free(ht->entries);
    free(ht);
}

/**
 * @brief Encontra uma vacina disponível no sistema pelo nome
 * 
 * Percorre o array de vacinas do sistema para encontrar uma vacina que
 * possui o nome especifico, que está dentro da data de validade e
 * que tem doses disponíveis (dose > 0).
 * 
 * @param sys Ponteiro para a estrutura do sistema que contém o array de vacinas
 * @param nome Nome da vacina a ser procurada
 * @return Índice da vacina no array se encontrada, -1 caso contrário
 */
int encontra_vacina_disponivel(Sys *sys, const char *nome) {
    for (int i = 0; i < sys->indice_vacinas; i++) {
        if (strcmp(sys->vacinas[i].nome, nome) == 0 &&
            eh_data(sys, sys->vacinas[i].validade.dia, 
                   sys->vacinas[i].validade.mes, 
                   sys->vacinas[i].validade.ano) &&
            sys->vacinas[i].dose > 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Verifica se um utente já recebeu uma inoculação no mesmo dia
 * 
 * Esta função verifica se um determinado utente já foi inoculado com uma vacina
 * do mesmo tipo no dia atual do sistema. A verificação é feita através
 * do acesso à hashtable de inoculações usando hash do nome do utente
 * da comparação das inoculações existentes para o utente
 * e da verificação da data e tipo de vacina
 * 
 * @param sys Ponteiro para a estrutura do sistema contendo os dados
 * @param utente Nome do utente a verificar
 * @param nome Nome da vacina a verificar
 * @return 1 se encontrar se já foi inoculado hoje, 0 caso contrário
 */
int verifica_inoculacao_duplicada(Sys *sys, const char *utente, const char *nome) {
    unsigned int key = hash(utente) % sys->hashtable_inoculacoes->size;
    Entry *entry = sys->hashtable_inoculacoes->entries[key];
    
    while (entry != NULL) {
        if (strcmp(entry->utente, utente) == 0) {
            Lista_inoculacoes *lista = entry->inocs;
            while (lista != NULL) {
                if (lista->inoc->validade.dia == sys->data_sys.dia &&
                    lista->inoc->validade.mes == sys->data_sys.mes &&
                    lista->inoc->validade.ano == sys->data_sys.ano) {
                    
                    for (int j = 0; j < sys->indice_vacinas; j++) {
                        if (strcmp(sys->vacinas[j].lote, lista->inoc->lote) == 0 &&
                            strcmp(sys->vacinas[j].nome, nome) == 0) {
                            return 1;
                        }
                    }
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
 * @brief Cria um novo registro de vacinação no sistema
 * 
 * Aloca e inicializa uma nova estrutura Inoculacao com:
 * - Número do lote da vacina especificada
 * - Nome do utente (paciente)
 * - Data atual do sistema como data de vacinação
 * 
 * @param sys Ponteiro para a estrutura principal do sistema
 * @param vacina_index Índice da vacina no array sys->vacinas
 * @param utente Nome do paciente a ser vacinado
 * @return Ponteiro para a nova estrutura Inoculacao criada, ou NULL em caso de falha
 */
Inoculacao* cria_registro_vacinacao(Sys *sys, int vacina_index, const char *utente) {
    Inoculacao *inoc = verifica_memoria(sys, malloc(sizeof(Inoculacao)));
    strncpy(inoc->lote, sys->vacinas[vacina_index].lote, sizeof(inoc->lote) - 1);
    inoc->lote[sizeof(inoc->lote) - 1] = '\0'; 
    inoc->utente = verifica_memoria(sys, strdup(utente));
    inoc->validade = sys->data_sys;
    return inoc;
}

/**
 * @brief Aplica uma vacina a um utente no sistema
 * 
 * Realiza todo o processo de vacinação, incluindo:
 * Verifica disponibilidade da vacina
 * verifica duplicidades de aplicação
 * cria o registro de vacinação
 * atualiza os contadores de aplicação e doses
 * 
 * @param sys Ponteiro para a estrutura principal do sistema
 * @param utente Nome do paciente a ser vacinado
 * @param nome Nome da vacina a ser aplicada
 */
void aplica_vacina(Sys *sys, char *utente, char *nome) {
    int vacina_index = encontra_vacina_disponivel(sys, nome);
    if (vacina_index == -1) {
        printf("%s\n", sys->comentarios[9]);
        return;
    }
    if (verifica_inoculacao_duplicada(sys, utente, nome)) {
        printf("%s\n", sys->comentarios[10]);
        return;
    }

    Inoculacao *inoc = cria_registro_vacinacao(sys, vacina_index, utente);
    inserir_hashtable(sys, inoc);
    
    sys->vacinas[vacina_index].aplicacoes++;
    sys->vacinas[vacina_index].dose--;
    
    printf("%s\n", sys->vacinas[vacina_index].lote);
}

/**
 * @brief Remove inoculação da hashtable com base nos critérios fornecidos
 *
 * Esta função procura e remove registros de vacinação que correspondam ao
 * nome do utente, lote da vacina e data de aplicação.
 *
 * @param sys Ponteiro para a estrutura do sistema
 * @param utente Nome do utente cuja vacinação será removida
 * @param lote Número do lote da vacina a remover
 * @param dia Dia da aplicação da vacina
 * @param mes Mês da aplicação da vacina
 * @param ano Ano da aplicação da vacina
 */
void eliminar_inoculacao_hashtable(Sys *sys, char *utente, char *lote, int dia, int mes, int ano) {
    int cnt = 0;
    Entry *entry = encontrar_hashtable(sys, utente);
    if (entry == NULL) {
        printf("%s: %s", utente, sys->comentarios[0]);
        return;
    }
    Lista_inoculacoes *curr = entry->inocs; // Elemento atual
    Lista_inoculacoes *tmp = NULL;  // Elemento a ser removido
    Lista_inoculacoes *prev = NULL;  // Elemento anterior
    while (curr != NULL) {
        if (strcmp(curr->inoc->utente, utente) == 0 && strcmp(curr->inoc->lote, lote) == 0 &&
            curr->inoc->validade.dia == dia && curr->inoc->validade.mes == mes &&
            curr->inoc->validade.ano == ano) {
            tmp = curr;  // Armazena o elemento a ser removido
            curr = curr->next;  // Avança para o próximo elemento
            if (prev) prev->next = curr; // O anterior aponta para o próximo
            else entry->inocs = curr;  // Atualiza o head se for o primeiro elemento
            eliminar_inoculacao_lista(sys, tmp->inoc->utente, tmp->inoc->lote, tmp->inoc->validade.dia, tmp->inoc->validade.mes, tmp->inoc->validade.ano);
            cnt++;
            sys->numero_inoculacoes--;
        }
        else {
            prev = curr;
            curr = curr->next;
        }
    }
    printf("%d\n", cnt);
}

/**
 * @brief Remove inoculaçao da lista global do sistema
 * 
 * Percorre a lista ligada de inoculações e remove o registro que correspondam
 * exatamente aos nome do utente, lote da vacina e data de aplicação.
 * Atualiza os ponteiros da aplicaçao anterior á aplicaçao removida para 
 * ligar a aplicaçao seguinte á removida. Atualiza tambem a tail.
 * 
 * @param sys Ponteiro para a estrutura principal do sistema
 * @param utente Nome do utente (comparação exata case-sensitive)
 * @param lote Número do lote da vacina (comparação exata case-sensitive)
 * @param dia Dia da validade da vacina
 * @param mes Mês da validade da vacina
 * @param ano Ano da validade da vacina
 */
void eliminar_inoculacao_lista (Sys *sys, char *utente, char *lote, int dia, int mes, int ano) {
    Lista_inoculacoes *curr = sys->inoculacoes;
    Lista_inoculacoes *tmp = NULL;
    Lista_inoculacoes *prev = NULL;
    while (curr != NULL) {
        if (strcmp(curr->inoc->utente, utente) == 0 &&
            strcmp(curr->inoc->lote, lote) == 0 &&
            curr->inoc->validade.dia == dia &&
            curr->inoc->validade.mes == mes &&
            curr->inoc->validade.ano == ano) {
                tmp = curr;
                curr = curr->next;
                if (prev) prev->next = curr;
                else sys->inoculacoes = curr;
                // Atualiza o ponteiro de fim se estiver removendo o último nó
                if (tmp == sys->tail_inoculacoes) sys->tail_inoculacoes = prev;
                free(tmp->inoc->utente);
                free(tmp->inoc);
                free(tmp);
        }
        else {
            prev = curr;
            curr = curr->next;
        }
    }
}

/**
 * @brief Remove inoculação da hashtable com base nos critérios fornecidos
 *
 * Esta função procura e remove registros de vacinação que correspondam ao
 * nome do utente e data de aplicaçao.
 * Atualiza os ponteiros da aplicaçao anterior á aplicaçao removida para 
 * ligar a aplicaçao seguinte á removida.
 *
 * @param sys Ponteiro para a estrutura do sistema
 * @param utente Nome do utente cuja vacinação será removida
 * @param dia Dia da aplicação da vacina
 * @param mes Mês da aplicação da vacina
 * @param ano Ano da aplicação da vacina
 */
void eliminar_inoculacao_hashtable_sem_lote(Sys *sys, char *utente, int dia, int mes, int ano) {
    int cnt = 0;
    Entry *entry = encontrar_hashtable(sys, utente);
    if (entry == NULL) {
        printf("%s: %s", utente, sys->comentarios[0]);
        return;
    }
    Lista_inoculacoes *curr = entry->inocs;
    Lista_inoculacoes *tmp = NULL;
    Lista_inoculacoes *prev = NULL;


    while (curr != NULL) {
        if (strcmp(curr->inoc->utente, utente) == 0 &&
            curr->inoc->validade.dia == dia &&
            curr->inoc->validade.mes == mes &&
            curr->inoc->validade.ano == ano) {
                tmp = curr;
                curr = curr->next;
                if (prev) prev->next = curr;
                else entry->inocs = curr;
                eliminar_inoculacao_sem_lote(sys, tmp->inoc->utente, tmp->inoc->validade.dia, tmp->inoc->validade.mes, tmp->inoc->validade.ano);
                cnt++;
                sys->numero_inoculacoes--;
        }
        else {
            prev = curr;
            curr = curr->next;
        }
    }
    printf("%d\n", cnt);
}


/**
 * @brief Remove inoculaçao da lista global do sistema
 * 
 * Percorre a lista ligada de inoculações e remove o registro que correspondam
 * exatamente aos nome do utente e data de aplicação.
 * Atualiza os ponteiros da aplicaçao anterior á aplicaçao removida para 
 * ligar a aplicaçao seguinte á removida. Atualiza tambem a tail.
 * 
 * @param sys Ponteiro para a estrutura principal do sistema
 * @param utente Nome do utente
 * @param dia Dia da validade da vacina
 * @param mes Mês da validade da vacina
 * @param ano Ano da validade da vacina
 */
void eliminar_inoculacao_sem_lote (Sys *sys, char *utente, int dia, int mes, int ano) {
    Lista_inoculacoes *curr = sys->inoculacoes;
    Lista_inoculacoes *tmp = NULL;
    Lista_inoculacoes *prev = NULL;
    while (curr != NULL) {
        if (strcmp(curr->inoc->utente, utente) == 0 &&
            curr->inoc->validade.dia == dia &&
            curr->inoc->validade.mes == mes &&
            curr->inoc->validade.ano == ano) {
                tmp = curr;
                curr = curr->next;
                if (prev) prev->next = curr;
                else sys->inoculacoes = curr;
                if (tmp == sys->tail_inoculacoes) sys->tail_inoculacoes = prev;
                free(tmp->inoc->utente);
                free(tmp->inoc);
                free(tmp);
        }
        else {
            prev = curr;
            curr = curr->next;
        }
    }
}

/**
 * @brief Remove inoculação da hashtable com base nos critérios fornecidos
 *
 * Esta função procura e remove registros de vacinação que correspondam ao
 * nome do utente.
 * Atualiza os ponteiros da aplicaçao anterior á aplicaçao removida para 
 * ligar a aplicaçao seguinte á removida.
 *
 * @param sys Ponteiro para a estrutura do sistema
 * @param utente Nome do utente cuja vacinação será removida
 */
void eliminar_inoculacao_hashtable_apenas_utente(Sys *sys, char *utente) {
    int cnt = 0;
    Entry *entry = encontrar_hashtable(sys, utente);
    if (entry == NULL) {
        printf("%s: %s\n", utente, sys->comentarios[0]);
        return;
    }
    Lista_inoculacoes *curr = entry->inocs;
    Lista_inoculacoes *tmp = NULL;
    Lista_inoculacoes *prev = NULL;


    while (curr != NULL) {
        if (strcmp(curr->inoc->utente, utente) == 0) {
                tmp = curr;
                curr = curr->next;
                if (prev) 
                    prev->next = curr;
                else 
                    entry->inocs = curr;
                eliminar_inoculacao_apenas_utente(sys, tmp->inoc->utente);
                cnt++;
                sys->numero_inoculacoes--;
        }
        else {
            prev = curr;
            curr = curr->next;
        }
    }
    printf("%d\n", cnt);
}

/**
 * @brief Remove inoculação da hashtable com base nos critérios fornecidos
 *
 * Esta função procura e remove registros de vacinação que correspondam ao
 * nome do utente.
 * Atualiza os ponteiros da aplicaçao anterior á aplicaçao removida para 
 * ligar a aplicaçao seguinte á removida. Atualiza tambem a tail.
 *
 * @param sys Ponteiro para a estrutura do sistema
 * @param utente Nome do utente cuja vacinação será removida
 */
void eliminar_inoculacao_apenas_utente (Sys *sys, char *utente) {
    Lista_inoculacoes *curr = sys->inoculacoes;
    Lista_inoculacoes *tmp = NULL;
    Lista_inoculacoes *prev = NULL;
    while (curr != NULL) {
        if (strcmp(curr->inoc->utente, utente) == 0) {
                tmp = curr;
                curr = curr->next;
                if (prev) prev->next = curr;
                else sys->inoculacoes = curr;
                if (tmp == sys->tail_inoculacoes) sys->tail_inoculacoes = prev;
                free(tmp->inoc->utente);
                free(tmp->inoc);
                free(tmp);
        }
        else {
            prev = curr;
            curr = curr->next;
        }
    }
}