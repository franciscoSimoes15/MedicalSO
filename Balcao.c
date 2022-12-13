// ISEC SISTEMAS OPERATIVOS 21/22
// Created by
// Francisco Simões 2019133920
// Pedro Praça 2020130980




#include "Balcao.h"
#include "Medico.h"
#include "Cliente.h"
bool encerra = false;
void envia_mensagem(pid_t pid, tipo_mensagem mensagem, char *fifo_path) {
    int fifo_fd = 0;
    char fifo[BUFFER];
    if (strcmp(fifo_path, BALCAO_FIFO) == 0) {
        strcpy(fifo, BALCAO_FIFO);
    } else {
        sprintf(fifo, fifo_path, pid);
    }

    fifo_fd = open(fifo, O_WRONLY);
    if (write(fifo_fd, &mensagem, sizeof(tipo_mensagem)) != sizeof(tipo_mensagem)) {
        printf("MENSAGEM MAL RECEBIDA");
        printf("%s\n", fifo_path);
        exit(EXIT_FAILURE);
    }
    close(fifo_fd);
}

void trata_ctrl_c(int s) {
    encerra= true;
    printf("Vou terminar ");
}

void le_variaveis_ambiente(int *MAXCLIENTES, int *MAXMEDICOS) {
    *MAXCLIENTES = 2;
    *MAXMEDICOS = 2;
    if (getenv("MAXCLIENTES") != NULL)
        *MAXCLIENTES = atoi(getenv("MAXCLIENTES"));
    printf("MAXCLIENTES = %d ", *MAXCLIENTES);

    if (getenv("MAXMEDICOS") != NULL)
        *MAXMEDICOS = atoi(getenv("MAXMEDICOS"));
    printf("MAXMEDICOS = %d ", *MAXMEDICOS);
}

int abre_pipes(int b_c[2], int c_b[2], int *pid) {

    int estado;
    if (pipe(b_c) == -1) { exit(1); } // Erro ao criar pipe balcao-> classificador se devolver -1
    if (pipe(c_b) == -1) exit(2); // Erro ao criar pipe classificador->balcao se devolver -1
    *pid = fork();
    if (*pid < 0) { exit(3); } // Erro ao criar processo filho se menor 0

    if (*pid == 0) {//PROCESSO FILHO
        // fechar o stdin e duplicar b_c[0] para conseguir armazenar no canal b_c, na extremidade de escrita, os sintomas
        close(STDIN_FILENO);
        dup(b_c[0]);
        close(b_c[0]);
        close(b_c[1]);
        // fechar o stdout e duplicar c_b[1] para conseguir ler no canal c_b, na extremidade de leitura, a especialidade e prioridade
        close(STDOUT_FILENO);
        dup(c_b[1]);
        close(c_b[1]);
        close(c_b[0]);
        // executa o programa classificador
        printf("EXECUTA CLASSIF");
        execl("./classificador", "classificador", NULL);
        perror("\nPrograma classificador não encontrado\n");
        return 4;
    }
    return 0;
}

int classifica_sintomas(int *b_c, int *c_b, char *especialidade, int *pid, char *sintomas) {
    if (*pid > 0) {
        //fecho as extremidades já usadas em filho que não vão ser necessárias no processo pai
        close(c_b[1]);
        close(b_c[0]);

        //armazena os sintomas na extremidade de escrita b_c[1]
        write(b_c[1], sintomas, strlen(sintomas));

        //envia a especialidade na extremidade de leitura c_b[0]
        read(c_b[0], especialidade, BUFFER);
        especialidade[strlen(especialidade)] = '\0';
    }

    return 0;
}

void printa_especialistas(tipo_medico *especialistas, int n_especialistas) {
    if (n_especialistas == 0) {
        printf("NAO HA MEDICOS\n");
        return;
    } else {
        printf("LISTAGEM DE ESPECIALISTAS\n");
        printf("\tpid\tnome\tespecialidade\npid_cliente\n");
        for (int i = 0; i < n_especialistas; i++) {
            printf("\t%d", especialistas[i].pid);
            printf("\t%s", especialistas[i].nome);
            printf("\t%s", especialistas[i].especialidade);
            printf("\t%d\n", especialistas[i].pid_cliente);
        }
    }
}

/*void printaclientesorto(tipo_cliente *ortopedia, int n_clientes_orto) {
    for (int i = 0; i < n_clientes_orto; i++) {
        printf("\t%d", ortopedia[i].pid);
        printf("\t%s", ortopedia[i].nome);
        printf("\t%s", ortopedia[i].sintomas);
        printf("\t%s", ortopedia[i].especialidade);
        printf("\t%d\n", ortopedia[i].prioridade);
    }
}

void printaclientesest(tipo_cliente *estomatologia, int n_clientes_est) {
    for (int i = 0; i < n_clientes_est; i++) {
        printf("\t%d", estomatologia[i].pid);
        printf("\t%s", estomatologia[i].nome);
        printf("\t%s", estomatologia[i].sintomas);
        printf("\t%s", estomatologia[i].especialidade);
        printf("\t%d\n", estomatologia[i].prioridade);
    }
}

void printaclientesgeral(tipo_cliente *geral, int n_clientes_geral) {
    for (int i = 0; i < n_clientes_geral; i++) {
        printf("\t%d", geral[i].pid);
        printf("\t%s", geral[i].nome);
        printf("\t%s", geral[i].sintomas);
        printf("\t%s", geral[i].especialidade);
        printf("\t\t%d\n", geral[i].prioridade);
    }
}

void printaclientesneuro(tipo_cliente *neurologia, int n_clientes_neuro) {
    for (int i = 0; i < n_clientes_neuro; i++) {
        printf("\t%d", neurologia[i].pid);
        printf("\t%s", neurologia[i].nome);
        printf("\t%s", neurologia[i].sintomas);
        printf("\t%s", neurologia[i].especialidade);
        printf("\t%d\n", neurologia[i].prioridade);
    }
}

void printaclientesofto(tipo_cliente *oftalmologia, int n_clientes_oft) {
    for (int i = 0; i < n_clientes_oft; i++) {
        printf("\t%d", oftalmologia[i].pid);
        printf("\t%s", oftalmologia[i].nome);
        printf("\t%s", oftalmologia[i].sintomas);
        printf("\t%s", oftalmologia[i].especialidade);
        printf("\t%d\n", oftalmologia[i].prioridade);
    }
}*/ //(efeitos debug)

void printa_clientes(tipo_cliente *clientes, int n_clientes) {

    if (n_clientes == 0) {
        printf("NAO HA CLIENTES\n");
        return;
    } else {
        printf("Obrigado pela sua resposta\n");
        printf("LISTAGEM DE CLIENTES\n");
        printf("\tpid\tnome\tsintomas\tespecialidade\tprioridade\n");
        for (int i = 0; i < n_clientes; i++) {
            printf("\t%d", clientes[i].pid);
            printf("\t%s", clientes[i].nome);
            if (strcmp(clientes[i].sintomas, "FICA_FILA_ESPERA") == 0) {
                printf("\tEm espera");
            } else if (strcmp(clientes[i].sintomas, "SAI_FILA_ESPERA") == 0) {
                printf("\tA ser atendido");
            }
            printf("\t%s", clientes[i].sintomas);
            printf("\t%s", clientes[i].especialidade);
            printf("\t%d\n", clientes[i].prioridade);
        }
    }


}

bool verifica_fila_espera(tipo_cliente *c, int n_clientes_oft, int n_clientes_neuro, int n_clientes_geral,
                          int n_clientes_est, int n_clientes_orto) {
    bool maxatingido = false;
    if (strcmp(c->especialidade, "oftalmologia") == 0) {
        if (n_clientes_oft > ESP_QUEUE - 1) {
            printf("Entrei ofto error\n");
            c->prioridade = SEM_VAGAS_FILA_ESPERA;
            maxatingido = true;
        }
    } else if (strcmp(c->especialidade, "neurologia") == 0) {
        if (n_clientes_neuro > ESP_QUEUE - 1) {
            c->prioridade = SEM_VAGAS_FILA_ESPERA;
            printf("SEM VAGAS %d\n", c->prioridade);
            maxatingido = true;
        }
    } else if (strcmp(c->especialidade, "geral") == 0) {
        if (n_clientes_geral > ESP_QUEUE - 1) {
            c->prioridade = SEM_VAGAS_FILA_ESPERA;
            printf("SEM VAGAS %d\n", c->prioridade);
            maxatingido = true;
        }
    } else if (strcmp(c->especialidade, "estomatologia") == 0) {
        if (n_clientes_est > ESP_QUEUE - 1) {
            c->prioridade = SEM_VAGAS_FILA_ESPERA;
            printf("SEM VAGAS %d\n", c->prioridade);
            maxatingido = true;
        }
    } else if (strcmp(c->especialidade, "ortopedia") == 0) {
        if (n_clientes_orto > ESP_QUEUE - 1) {
            c->prioridade = SEM_VAGAS_FILA_ESPERA;
            printf("SEM VAGAS %d\n", c->prioridade);
            maxatingido = true;
        }
    }
    return maxatingido;
}

void insere_na_especialidade(tipo_cliente *c, int *n_clientes, tipo_cliente *especialidade) {
    tipo_cliente temp; // copia de cliente
    tipo_mensagem mensagem;
    especialidade[(*n_clientes)++] = *c;

    for (int i = 0; i < *n_clientes; i++) { // for encadeado serve para fazer o sort da lista de espera
        for (int j = i + 1; j < *n_clientes; j++) {  // compara um elemento da fila de espera com os restantes
            if (especialidade[i].prioridade >
                especialidade[j].prioridade) { // se o utente seguinte tiver maior prioridade, isto é,
                temp = especialidade[i];
                especialidade[i] = especialidade[j];
                especialidade[j] = temp;
            }
        }
    }
    for (int i = 0; i < *n_clientes; i++) {
        if (especialidade[i].pid == c->pid) {    // a posicao do cliente inserido na fila
            // corresponde ao numero de pessoas que tem a sua frente
            especialidade[i].npessoas_frente = i;
            /*         printf("nome %s %d\n",especialidade[i].nome,especialidade[i].npessoas_frente);*/
            for (int j = i + 1; j < *n_clientes; j++) {
                especialidade[j].npessoas_frente += 1;
                /* printf("pessoas %s %d\n",especialidade[j].nome,especialidade[j].npessoas_frente);*/
                sprintf(mensagem.mensagem, "[INFO] Foi atualizado a sua posição de espera!!!\nPessoas a sua frente %d",
                        especialidade[j].npessoas_frente);
                envia_mensagem(especialidade[j].pid, mensagem, CLIENTE_FIFO);
            }

        }
    }
}

void retira_fila_especialidade(int n_clientes_esp, tipo_cliente *especialidade,
                               tipo_cliente c) { // em caso de o eliminar da especialidade
    tipo_cliente temp;

    if (strcmp(c.sintomas, "SAI_FILA_ESPERA") == 0) {
        for (int i = 0; i < n_clientes_esp; i++) {
            if (especialidade[i].pid == c.pid) {
                temp = especialidade[i];

                for (int j = i; j < n_clientes_esp; j++) {
                    especialidade[j] = especialidade[j + 1];
                }
                especialidade[n_clientes_esp - 1] = temp;
            }
        }
    }
}

void ordena_especialistas(int n_especialistas, tipo_medico *especialistas) { // no caso de alguem sair
    tipo_medico temp;
    if (n_especialistas > 1) {
        for (int i = 0; i < n_especialistas; i++) {
            for (int j = i + 1; j < i; j++) {
                if (especialistas[i].pid_cliente != 0 && especialistas[j].pid_cliente == 0) {
                    temp = especialistas[i];
                    especialistas[i] = especialistas[j];
                    especialistas[j] = temp;
                }
            }
        }
    } else {
        printf("/*So tem um especi*/alista no sistema");
    }
}

void procura_index(int n_especialistas, tipo_medico *especialistas, tipo_cliente *especialidade, int *index, int n_clientes,
              char qual[BUFFER]) {
    *index = -1;
    if (n_especialistas > 0) {
        for (int i = 0; i < n_especialistas; i++) {
            if (especialistas[i].pid_cliente == 0
                && strcmp(especialistas[i].especialidade, qual) == 0) {
                strcpy(especialidade[n_clientes - 1].sintomas, "SAI_FILA_ESPERA");
                *index = i;
                return;
            }
        }
    } else {
        printf("SEM MEDICO PARA ATENDER\n");
    }
}

void procura_index_cliente(int n_especialistas, tipo_medico *especialistas, tipo_cliente *especialidade, int *index,
                           int n_clientes) {
    *index = -1;
    if (n_clientes > 0) {
        for (int i = 0; i < n_clientes; i++) {
            printf("%d %s\n", i, especialidade[i].sintomas);
            printf("%d %d\n", i, especialistas[n_especialistas - 1].pid_cliente);
            if (strcmp(especialidade[i].sintomas, "FICA_FILA_ESPERA") == 0) {
                printf("ENTREI\n");
                for (int j = 0; j < n_especialistas; j++) {
                    if (especialistas[j].pid_cliente == 0) {
                        strcpy(especialidade[i].sintomas, "SAI_FILA_ESPERA");
                        printf("ENTREI procura_index %d %d\n ", especialistas[j].pid, especialidade[i].pid);
                        *index = i;
                        return;
                    }
                }
            }
        }
    } else {
        printf("SEM CLIENTES PARA ATENDER...\n");
    }
}

void printa_comandos() {
    printf("\tutentes - lista utentes"
           "\n\tespecialistas - lista especialistas"
           "\n\tdelut X (X corresponde ao pid do cliente) - remove cliente"
           "\n\tdelesp X (X corresponde ao pid do especialista) - remove especialista"
           "\n\tencerra - Termina balcão"
           "\n\tdebug - somente efeitos debug (printa clientes e especialistas)\n\n");
}

void remove_cliente(char *nomerecebido, tipo_cliente *clientes, int *n_clientes, tipo_cliente *ortopedia,
                    int *n_clientes_orto, tipo_cliente *estomatologia, int *n_clientes_est, tipo_cliente *neurologia,
                    int *n_clientes_neuro, tipo_cliente *oftalmologia, int *n_clientes_oft, tipo_cliente *geral,
                    int *n_clientes_geral) {
    for (int i = 0; i < *n_clientes; i++) {
        if (strcmp(nomerecebido, clientes[i].nome) == 0) {
            if (strcmp(clientes[i].sintomas, "SAI_FILA_ESPERA") != 0) {
                if (*n_clientes > 0) {
                    if (strcmp("ortopedia", clientes[i].especialidade) == 0) {
                        if (*n_clientes_orto > 0) {
                            for (int j = i; j < *n_clientes; j++)
                                clientes[i] = clientes[j];
                            printf("Utente ortopedia removido com sucesso\n");

                            for (int k = 0; k < *n_clientes_orto; k++) {
                                if (clientes[i].pid == ortopedia[k].pid) {
                                    ortopedia[i] = ortopedia[k];
                                }
                            }
                            (*n_clientes_orto)--;
                            (*n_clientes)--;
                        } else {
                            printf("Nao tem clientes ortopedia\n");
                        }
                    } else if (strcmp("estomatologia", clientes[i].especialidade) == 0) {
                        if (*n_clientes_est > 0) {
                            for (int j = i; j < *n_clientes; j++)
                                clientes[i] = clientes[j];
                            printf("Utente estomatologia removido com sucesso\n\n");

                            for (int k = 0; k < *n_clientes_est; k++) {
                                if (clientes[i].pid == estomatologia[k].pid) {
                                    estomatologia[i] = estomatologia[k];
                                }
                            }
                            (*n_clientes_est)--;
                            (*n_clientes)--;
                        } else {
                            printf("Nao tem clientes estomatologia\n");
                        }
                    } else if (strcmp(clientes[i].especialidade, "geral") == 0) {
                        if (*n_clientes_geral > 0) {
                            for (int j = i; j < *n_clientes; j++)
                                clientes[i] = clientes[j];
                            printf("Utente estomatologia removido com sucesso\n");

                            for (int k = 0; k < *n_clientes_geral; k++) {
                                if (clientes[i].pid == geral[k].pid) {
                                    geral[i] = geral[k];
                                }
                            }
                            (*n_clientes_geral)--;
                            (*n_clientes)--;
                        } else {
                            printf("Nao tem clientes geral\n");
                        }
                    } else if (strcmp(clientes[i].especialidade, "neurologia") == 0) {
                        if (*n_clientes_neuro > 0) {
                            for (int j = i; j < *n_clientes; j++)
                                clientes[i] = clientes[j];
                            printf("Utente neurologia removido com sucesso\n");

                            for (int k = 0; k < *n_clientes_neuro; k++) {
                                if (clientes[i].pid == neurologia[k].pid) {
                                    neurologia[i] = neurologia[k];
                                }
                            }
                            (*n_clientes_neuro)--;
                            (*n_clientes)--;
                        } else {
                            printf("Nao tem clientes neurologia\n");
                        }
                    } else if (strcmp(clientes[i].especialidade, "oftalmologia") == 0) {
                        if (*n_clientes_oft > 0) {
                            for (int j = i; j < *n_clientes; j++)
                                clientes[i] = clientes[j];
                            printf("Utente oftalmologia removido com sucesso\n");

                            for (int k = 0; k < *n_clientes_oft; k++) {
                                if (clientes[i].pid == oftalmologia[k].pid) {
                                    oftalmologia[i] = oftalmologia[k];
                                }
                            }
                            (*n_clientes_oft)--;
                            (*n_clientes)--;
                        } else {
                            printf("Nao tem clientes oftalmologia\n");
                        }
                    }
                } else {
                    printf("Nao tem clientes neurologia\n");
                }
            } else {
                printf("Esta a ser atendido logo nao pode ser removido\n");
            }
        } else {
            printf("Nao existe nenhum utente com o nome %s!\n", nomerecebido);
        }
    }
}

void remove_especialistas(char *nomerecebido, tipo_medico *especialistas, int *n_especialistas) {
    for (int i = 0; i < *n_especialistas; i++) {
        printf("%s\n", especialistas[i].nome);
        if (strcmp(especialistas[i].nome, nomerecebido) == 0 && especialistas[i].pid_cliente == 0) {
            for (int j = i; j < *n_especialistas; j++) {
                especialistas[i] = especialistas[j];
            }
            (*n_especialistas)--;
            printf("especialistas=%d\n", *n_especialistas);
            printf("Especialista removido com sucesso!\n");
        } else {
            printf("Nao existe nenhum especialista com o nome %s!\n", nomerecebido);
        }
    }
}


void verifica_iniciaconsulta(tipo_medico *m, tipo_cliente *c, tipo_mensagem *mensagem, tipo_cliente *ortopedia,
                             int *n_clientes_orto, tipo_cliente *estomatologia, int *n_clientes_est,
                             tipo_cliente *neurologia, int *n_clientes_neuro, tipo_cliente *oftalmologia,
                             int *n_clientes_oft, tipo_cliente *geral, int *n_clientes_geral,
                             tipo_medico *especialistas, int *n_especialistas, tipo_cliente *clientes,
                             int *n_clientes) {
    int index;
    if (mensagem->tipo == TIPO_CHEGADA_CLIENTE) {

        if (strcmp(c->especialidade, "ortopedia") == 0) {
            insere_na_especialidade(c, n_clientes_orto, ortopedia);
            procura_index(*n_especialistas, especialistas, ortopedia, &index, *n_clientes_orto,
                          "ortopedia");//procura index de medico para atender
            for (int i = 0; i < *n_clientes; i++) {
                if (clientes[i].pid == ortopedia[*n_clientes_orto - 1].pid) {
                    strcpy(clientes[i].sintomas, ortopedia[*n_clientes_orto - 1].sintomas);
                }
            }
            if (index >= 0) {
                retira_fila_especialidade(*n_clientes_orto, ortopedia, *c); // retira da fila de espera
                especialistas[index].pid_cliente = ortopedia[*n_clientes_orto - 1].pid;
                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = especialistas[index].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai ser atendido pelo Dr.(a) %s",
                         especialistas[index].nome);
                envia_mensagem(ortopedia[*n_clientes_orto - 1].pid, *mensagem, CLIENTE_FIFO);
                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = ortopedia[*n_clientes_orto - 1].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai atender o cliente %s",
                         ortopedia[*n_clientes_orto - 1].nome);
                envia_mensagem(especialistas[index].pid, *mensagem, MEDICO_FIFO);
                (*n_clientes_orto)--;
                ordena_especialistas(*n_especialistas, especialistas);
                printa_especialistas(especialistas, *n_especialistas);
            } else {

                mensagem->tipo = ESPERA_MEDICO;
                mensagem->pid = 0;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem),
                         "Aguarde para o balcão lhe indicar um medico...\n");
                envia_mensagem(ortopedia[*n_clientes_orto - 1].pid, *mensagem, CLIENTE_FIFO);
            }
        }
        if (strcmp(c->especialidade, "geral") == 0) {
            insere_na_especialidade(c, n_clientes_geral, geral);
            procura_index(*n_especialistas, especialistas, geral, &index, *n_clientes_geral,
                          "geral");//procura index de medico para atender
            for (int i = 0; i < *n_clientes; i++) {
                if (clientes[i].pid == geral[*n_clientes_geral - 1].pid) {
                    strcpy(clientes[i].sintomas, geral[*n_clientes_geral - 1].sintomas);
                }
            }
            if (index >= 0) {
                retira_fila_especialidade(*n_clientes_geral, geral, *c); // retira da fila de espera
                especialistas[index].pid_cliente = geral[*n_clientes_geral - 1].pid;
                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = especialistas[index].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai ser atendido pelo Dr.(a) %s",
                         especialistas[index].nome);
                envia_mensagem(geral[*n_clientes_geral - 1].pid, *mensagem, CLIENTE_FIFO);

                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = geral[*n_clientes_geral - 1].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai atender o cliente %s",
                         geral[*n_clientes_geral - 1].nome);
                envia_mensagem(especialistas[index].pid, *mensagem, MEDICO_FIFO);
                (*n_clientes_geral)--;
                ordena_especialistas(*n_especialistas, especialistas);
            } else {
                mensagem->tipo = ESPERA_MEDICO;
                mensagem->pid = 0;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem),
                         "Aguarde para o balcão lhe indicar um medico...\n");
                envia_mensagem(geral[*n_clientes_geral - 1].pid, *mensagem, CLIENTE_FIFO);
            }
        }
        if (strcmp(c->especialidade, "neurologia") == 0) {
            insere_na_especialidade(c, n_clientes_neuro, neurologia);
            procura_index(*n_especialistas, especialistas, neurologia, &index, *n_clientes_neuro,
                          "neurologia");//procura index de medico para atender
            for (int i = 0; i < *n_clientes; i++) {
                if (clientes[i].pid == neurologia[*n_clientes_neuro - 1].pid) {
                    strcpy(clientes[i].sintomas, neurologia[*n_clientes_neuro - 1].sintomas);
                }
            }
            if (index >= 0) {
                retira_fila_especialidade(*n_clientes_neuro, neurologia, *c); // retira da fila de espera
                especialistas[index].pid_cliente = neurologia[*n_clientes_neuro - 1].pid;
                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = especialistas[index].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai ser atendido pelo Dr.(a) %s",
                         especialistas[index].nome);
                envia_mensagem(neurologia[*n_clientes_neuro - 1].pid, *mensagem, CLIENTE_FIFO);

                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = neurologia[*n_clientes_neuro - 1].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai atender o cliente %s",
                         neurologia[*n_clientes_neuro - 1].nome);
                envia_mensagem(especialistas[index].pid, *mensagem, MEDICO_FIFO);
                (*n_clientes_neuro)--;
                ordena_especialistas(*n_especialistas, especialistas);
            } else {
                mensagem->tipo = ESPERA_MEDICO;
                mensagem->pid = 0;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem),
                         "Aguarde para o balcão lhe indicar um medico...\n");
                envia_mensagem(neurologia[*n_clientes_neuro - 1].pid, *mensagem, CLIENTE_FIFO);
            }
        }
        if (strcmp(c->especialidade, "estomatologia") == 0) {
            insere_na_especialidade(c, n_clientes_est, estomatologia);
            procura_index(*n_especialistas, especialistas, estomatologia, &index, *n_clientes_est,
                          "estomatologia");//procura index de medico para atender
            for (int i = 0; i < *n_clientes; i++) {
                if (clientes[i].pid == estomatologia[*n_clientes_est - 1].pid) {
                    strcpy(clientes[i].sintomas, estomatologia[*n_clientes_est - 1].sintomas);
                }
            }
            if (index >= 0) {
                retira_fila_especialidade(*n_clientes_est, estomatologia, *c); // retira da fila de espera
                especialistas[index].pid_cliente = estomatologia[*n_clientes_est - 1].pid;
                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = especialistas[index].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai ser atendido pelo Dr.(a) %s",
                         especialistas[index].nome);
                envia_mensagem(estomatologia[*n_clientes_est - 1].pid, *mensagem, CLIENTE_FIFO);

                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = estomatologia[*n_clientes_est - 1].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai atender o cliente %s",
                         estomatologia[*n_clientes_est - 1].nome);
                envia_mensagem(especialistas[index].pid, *mensagem, MEDICO_FIFO);
                (*n_clientes_est)--;
                ordena_especialistas(*n_especialistas, especialistas);
            } else {
                mensagem->tipo = ESPERA_MEDICO;
                mensagem->pid = 0;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem),
                         "Aguarde para o balcão lhe indicar um medico...\n");
                envia_mensagem(estomatologia[*n_clientes_est - 1].pid, *mensagem, CLIENTE_FIFO);
            }
        }
        if (strcmp(c->especialidade, "oftalmologia") == 0) {
            insere_na_especialidade(c, n_clientes_oft, oftalmologia);
            procura_index(*n_especialistas, especialistas, oftalmologia, &index, *n_clientes_oft,
                          "estomatologia");//procura index de medico para atender
            for (int i = 0; i < *n_clientes; i++) {
                if (clientes[i].pid == oftalmologia[*n_clientes_oft - 1].pid) {
                    strcpy(clientes[i].sintomas, oftalmologia[*n_clientes_oft - 1].sintomas);
                }
            }
            if (index >= 0) {
                retira_fila_especialidade(*n_clientes_oft, oftalmologia, *c); // retira da fila de espera
                especialistas[index].pid_cliente = oftalmologia[*n_clientes_oft - 1].pid;
                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = especialistas[index].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai ser atendido pelo Dr.(a) %s",
                         especialistas[index].nome);
                envia_mensagem(oftalmologia[*n_clientes_oft - 1].pid, *mensagem, CLIENTE_FIFO);

                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = oftalmologia[*n_clientes_oft - 1].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai atender o cliente %s",
                         oftalmologia[*n_clientes_oft - 1].nome);
                envia_mensagem(especialistas[index].pid, *mensagem, MEDICO_FIFO);
                (*n_clientes_oft)--;
                ordena_especialistas(*n_especialistas, especialistas);
            } else {
                mensagem->tipo = ESPERA_MEDICO;
                mensagem->pid = 0;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem),
                         "Aguarde para o balcão lhe indicar um medico...\n");
                envia_mensagem(oftalmologia[*n_clientes_oft - 1].pid, *mensagem, CLIENTE_FIFO);
            }
        }
    }
    if (mensagem->tipo == TIPO_CHEGADA_MEDICO) {

        if (strcmp(m->especialidade, "ortopedia") == 0) {

            procura_index_cliente(*n_especialistas, especialistas, ortopedia, &index,
                                  *n_clientes_orto);//procura index de cliente para atender
            if (index >= 0) {
                retira_fila_especialidade(*n_clientes_orto, ortopedia, *c); // retira da fila de espera
                especialistas[*n_especialistas - 1].pid_cliente = ortopedia[index].pid;
                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = ortopedia[index].pid;


                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai atender o cliente %s\n",
                         ortopedia[index].nome);
                envia_mensagem(especialistas[*n_especialistas - 1].pid, *mensagem, MEDICO_FIFO);


                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = especialistas[*n_especialistas - 1].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai ser atendido pelo Dr.(a) %s",
                         especialistas[*n_especialistas - 1].nome);
                envia_mensagem(ortopedia[index].pid, *mensagem, CLIENTE_FIFO);
            } else {
                mensagem->tipo = ESPERA_CLIENTE;
                mensagem->pid = especialistas[*n_especialistas - 1].pid_cliente;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem),
                         "Aguarde para o balcão lhe indicar um cliente...");
                envia_mensagem(especialistas[*n_especialistas - 1].pid, *mensagem, MEDICO_FIFO);
            }
        }
        if (strcmp(m->especialidade, "geral") == 0) {


            procura_index_cliente(*n_especialistas, especialistas, geral, &index,
                                  *n_clientes_geral);//procura index de medico para atender
            if (index >= 0) {
                retira_fila_especialidade(*n_clientes_geral, geral, *c); // retira da fila de espera

                especialistas[*n_especialistas - 1].pid_cliente = geral[index].pid;
                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = geral[index].pid;

                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai atender o cliente %s\n",
                         geral[index].nome);
                envia_mensagem(especialistas[*n_especialistas - 1].pid, *mensagem, MEDICO_FIFO);


                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = especialistas[*n_especialistas - 1].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai ser atendido pelo Dr.(a) %s",
                         especialistas[*n_especialistas - 1].nome);
                envia_mensagem(geral[index].pid, *mensagem, CLIENTE_FIFO);

            } else {
                mensagem->tipo = ESPERA_CLIENTE;
                mensagem->pid = especialistas[*n_especialistas - 1].pid_cliente;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem),
                         "Aguarde para o balcão lhe indicar um cliente...");
                envia_mensagem(especialistas[*n_especialistas - 1].pid, *mensagem, MEDICO_FIFO);

            }
        }
        if (strcmp(m->especialidade, "neurologia") == 0) {


            procura_index_cliente(*n_especialistas, especialistas, neurologia, &index,
                                  *n_clientes_neuro);//procura index de medico para atender
            if (index >= 0) {
                retira_fila_especialidade(*n_clientes_neuro, neurologia, *c); // retira da fila de espera
                especialistas[*n_especialistas - 1].pid_cliente = neurologia[index].pid;
                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = neurologia[index].pid;

                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai atender o cliente %s\n",
                         neurologia[index].nome);
                envia_mensagem(especialistas[*n_especialistas - 1].pid, *mensagem, MEDICO_FIFO);

                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = especialistas[*n_especialistas - 1].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai ser atendido pelo Dr.(a) %s",
                         especialistas[*n_especialistas - 1].nome);
                envia_mensagem(neurologia[index].pid, *mensagem, CLIENTE_FIFO);

            } else {
                mensagem->tipo = ESPERA_CLIENTE;
                mensagem->pid = especialistas[*n_especialistas - 1].pid_cliente;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem),
                         "Aguarde para o balcão lhe indicar um cliente...");
                envia_mensagem(especialistas[*n_especialistas - 1].pid, *mensagem, MEDICO_FIFO);

            }
        }
        if (strcmp(m->especialidade, "estomatologia") == 0) {


            procura_index_cliente(*n_especialistas, especialistas, estomatologia, &index,
                                  *n_clientes_est);//procura index de medico para atender
            if (index >= 0) {
                retira_fila_especialidade(*n_clientes_est, estomatologia, *c); // retira da fila de espera
                especialistas[*n_especialistas - 1].pid_cliente = estomatologia[index].pid;
                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = estomatologia[index].pid;


                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai atender o cliente %s\n",
                         estomatologia[index].nome);
                envia_mensagem(especialistas[*n_especialistas - 1].pid, *mensagem, MEDICO_FIFO);

                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = especialistas[*n_especialistas - 1].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai ser atendido pelo Dr.(a) %s",
                         especialistas[*n_especialistas - 1].nome);
                envia_mensagem(estomatologia[index].pid, *mensagem, CLIENTE_FIFO);

            } else {
                mensagem->tipo = ESPERA_CLIENTE;
                mensagem->pid = especialistas[*n_especialistas - 1].pid_cliente;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem),
                         "Aguarde para o balcão lhe indicar um cliente...");
                envia_mensagem(especialistas[*n_especialistas - 1].pid, *mensagem, MEDICO_FIFO);
            }
        }
        if (strcmp(m->especialidade, "oftalmologia") == 0) {


            procura_index_cliente(*n_especialistas, especialistas, oftalmologia, &index,
                                  *n_clientes_oft);//procura index de medico para atender
            if (index >= 0) {
                retira_fila_especialidade(*n_clientes_oft, oftalmologia, *c); // retira da fila de espera
                especialistas[*n_especialistas - 1].pid_cliente = oftalmologia[index].pid;
                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = oftalmologia[index].pid;

                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai atender o cliente %s\n",
                         oftalmologia[index].nome);
                envia_mensagem(especialistas[*n_especialistas - 1].pid, *mensagem, MEDICO_FIFO);

                mensagem->tipo = INICIO_CONSULTA;
                mensagem->pid = especialistas[*n_especialistas - 1].pid;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem), "Vai ser atendido pelo Dr.(a) %s",
                         especialistas[*n_especialistas - 1].nome);
                envia_mensagem(oftalmologia[index].pid, *mensagem, CLIENTE_FIFO);

            } else {
                strcpy(oftalmologia[*n_clientes_oft - 1].sintomas, "FICA_FILA_ESPERA");
                mensagem->tipo = ESPERA_CLIENTE;
                mensagem->pid = especialistas[*n_especialistas - 1].pid_cliente;
                snprintf(mensagem->mensagem, sizeof(mensagem->mensagem),
                         "Aguarde para o balcão lhe indicar um cliente...");
                envia_mensagem(especialistas[*n_especialistas - 1].pid, *mensagem, MEDICO_FIFO);
            }
        }
    }
}

int main(void) {
    // N e M corresponde às variáveis de ambiente MAXCLIENTES, MAXMEDICOS respetivamente
    int N = 0, M = 0;
    //le variaveis de ambiente
    le_variaveis_ambiente(&N, &M);
    char c_fifo_fname[50], m_fifo_fname[50];
    char msg[BUFFER];
    signal(SIGINT, trata_ctrl_c);
    fprintf(stdout, "\n-------------BEM-VINDO A BALCÃO-------------\n");

    if (mkfifo(BALCAO_FIFO, 0777) == -1) {
        perror("Ja existe um balcao a correr");
        exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "\nNovo fifo balcao criado");
    }
    int m_fifo_fd, c_fifo_fd;
    int b_fifo_fd = open(BALCAO_FIFO, O_RDWR);
    if (b_fifo_fd == -1) {
        perror("\nErro ao correr o FIFO do balcao ");
        exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "FIFO balcao a correr para escrita e leitura\n");
    }
    pid_t pid_balcao = getpid();
    printf("balcao=%d\n", pid_balcao);

    // corresponde a um cliente
    tipo_cliente c;
    //corresponde a um medico
    tipo_medico m;
    //Instâncias correspondentes a ponteiros para a estrutura cliente e medico respetivamente (client = ponteiro para cliente, doctor = ponteiro para medico)
    tipo_cliente *clientes; // armazena os clientes em espera de serem atendidos, sendo que só podem estar NMAXCLIENTES (variável ambiente) em espera
    tipo_medico *especialistas; // armazena os medicos em espera de começarem consulta, sendo que só podem estar NMAXMEDICOS (variável ambiente) em espera

    //filas de espera de cada especialidade
    tipo_cliente *oftalmologia;
    tipo_cliente *neurologia;
    tipo_cliente *estomatologia;
    tipo_cliente *ortopedia;
    tipo_cliente *geral;

    int b_c[2], c_b[2];
    int index = -1;

    int n_clientes = 0, n_especialistas = 0, n_clientes_est = 0, n_clientes_orto = 0, n_clientes_geral = 0, n_clientes_neuro = 0, n_clientes_oft = 0, n_frente = 0;

    int menu_fd;
    char comando[BUFFER];
    fd_set rfds;
    struct timeval tv;
    int retval;
    int nfd;
    tipo_mensagem mensagem;

    clientes = malloc(sizeof(tipo_cliente) * N);//ALOCA NMAX de clientes no sistema
    especialistas = malloc(sizeof(tipo_medico) * M);//ALOCA NMAX de medicos no sistema
    oftalmologia = malloc(sizeof(tipo_cliente) * ESP_QUEUE);
    neurologia = malloc(sizeof(tipo_cliente) * ESP_QUEUE);
    estomatologia = malloc(sizeof(tipo_cliente) * ESP_QUEUE);
    ortopedia = malloc(sizeof(tipo_cliente) * ESP_QUEUE);
    geral = malloc(sizeof(tipo_cliente) * ESP_QUEUE);

    bool maxatingido = false;
    char *tokens;
    int maxfd;
    int valor, pid, freq;

    abre_pipes(b_c, c_b, &pid);//abre pipes e executa classificador

    printf("Digite help se tem dúvida nos comandos a introduzir\n");
    while (!encerra) {
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(b_fifo_fd, &rfds);
        maxfd = STDIN_FILENO > b_fifo_fd ? STDIN_FILENO : b_fifo_fd;
        retval = select(maxfd + 1, &rfds, NULL, NULL, NULL);
        if (retval == -1) { // termina os medicos e os clientes
            for (int i = 0; i < n_especialistas; i++) {
                mensagem.tipo = TIPO_SAIDA_MEDICO;
                sprintf(mensagem.mensagem, "O balcão mandou encerrar o medico");
                envia_mensagem(especialistas[i].pid, mensagem, MEDICO_FIFO);
            }
            for (int i = 0; i < n_clientes; i++) {
                mensagem.tipo = TIPO_SAIDA_CLIENTE;
                sprintf(mensagem.mensagem, "O balcão mandou encerrar o cliente");
                envia_mensagem(clientes[i].pid, mensagem, CLIENTE_FIFO);
            }
           /* unlink(BALCAO_FIFO);
            perror("select()");
            exit(EXIT_FAILURE);*/
        } else {
            if (FD_ISSET(STDIN_FILENO, &rfds)) { // é para enviar mensagem
                fgets(comando, BUFFER, stdin);
                comando[strlen(comando) - 1] = '\0';
                tokens = strtok(comando, " ");
                if (tokens != NULL) {
                    if (strcmp(tokens, "delut") == 0) {
                        tokens = strtok(NULL, " ");
                        if (tokens != NULL) {
                            remove_cliente(tokens, clientes, &n_clientes, ortopedia, &n_clientes_orto,
                                           estomatologia,
                                           &n_clientes_est, neurologia, &n_clientes_neuro, oftalmologia,
                                           &n_clientes_oft, geral, &n_clientes_geral);
                        }
                    } else if (strcmp(tokens, "delesp") == 0) {
                        tokens = strtok(NULL, " ");
                        if (tokens != NULL) {
                            remove_especialistas(tokens, especialistas, &n_especialistas);
                        }
                    } else if (strcmp(tokens, "freq") == 0) {
                        tokens = strtok(NULL, " ");
                        if (tokens != NULL) {
                            freq = atoi(tokens);
                            if (freq < 1) {
                                printf("Freq invalido\n");
                            } else {

                                printf("Atualizei freq para %d\n", freq);
                            }
                        }
                    } else if (strcmp(tokens, "encerra") == 0) {
                        printf("Balcao encerrou\n");
                        for (int i = 0; i < n_especialistas; i++) {
                            sprintf(mensagem.mensagem, "O balcão mandou encerrar o médico");

                            mensagem.tipo = TIPO_SAIDA_MEDICO;

                            envia_mensagem(especialistas[i].pid, mensagem, MEDICO_FIFO);
                        }
                        for (int i = 0; i < n_clientes; i++) {
                            sprintf(mensagem.mensagem, "O balcão mandou encerrar o cliente");

                            mensagem.tipo = TIPO_SAIDA_CLIENTE;

                            envia_mensagem(clientes[i].pid, mensagem, CLIENTE_FIFO);
                        }
                        for (int i = 0; i < n_clientes; i++) {
                            sprintf(mensagem.mensagem, "O balcão mandou encerrar o cliente");
                            if (strcmp(clientes[i].sintomas, "FICA_FILA_ESPERA") == 0) {
                                mensagem.tipo = TIPO_SAIDA_CLIENTE;
                            } else if (strcmp(clientes[i].sintomas, "SAI_FILA_ESPERA") == 0) {
                                mensagem.tipo = FIM_CONSULTA_CLIENTE;
                            }
                            envia_mensagem(clientes[i].pid, mensagem, CLIENTE_FIFO);
                        }
                        break;

                    } else if (strcmp(tokens, "utentes") == 0) {
                        printf("Lista de utentes\n");
                        printa_clientes(clientes, n_clientes);
                    } else if (strcmp(tokens, "especialistas") == 0) {
                        printf("Lista de especialista\n");
                        printa_especialistas(especialistas, n_especialistas);
                    } else if (strcmp(tokens, "d") == 0) {
                        printf("debug\n");
                        printa_clientes(clientes, n_clientes);
                        printa_especialistas(especialistas, n_especialistas);
                    } else if (strcmp(comando, "help") == 0) {
                        printa_comandos();
                    }
                }
            } else if (FD_ISSET(b_fifo_fd, &rfds)) { // balcao recebe mensagem
                int res = read(b_fifo_fd, &mensagem, sizeof(tipo_mensagem));
                if (res != sizeof(tipo_mensagem)) {
                    close(c_b[0]);
                    close(b_c[1]);
                    close(b_fifo_fd);
                    unlink(BALCAO_FIFO);
                    exit(EXIT_FAILURE);
                }
                if (mensagem.tipo == TIPO_SAIDA_CLIENTE) {
                    printf("%s\n", mensagem.mensagem);
                    if (n_clientes > 0) {
                        for (int i = 0;
                             i < n_clientes; i++) { // for encadeado serve para fazer o sort da lista de espera
                            if (clientes[i].pid == mensagem.pid) {
                                strcpy(clientes[i].sintomas, "SAIU");
                                remove_cliente(clientes[i].nome, clientes, &n_clientes, ortopedia, &n_clientes_orto,
                                               estomatologia,
                                               &n_clientes_est, neurologia, &n_clientes_neuro, oftalmologia,
                                               &n_clientes_oft, geral, &n_clientes_geral);
                            }
                            break;
                        }

                    } else {
                        if (n_clientes == 0) {
                            printf("SEM CLIENTES NO SISTEMA\n");
                        }
                    }
                    n_clientes--;
                }

                if (mensagem.tipo == TIPO_SAIDA_MEDICO) {
                    printf("%s\n", mensagem.mensagem);
                    if (n_especialistas > 0) {
                        for (int i = 0;
                             i < n_especialistas; i++) { // for encadeado serve para fazer o sort da lista de espera
                            if (especialistas[i].pid == mensagem.pid) {
                                remove_especialistas(especialistas[i].nome, especialistas, &n_especialistas);
                                break;
                            }
                        }
                    } else {
                        if (n_especialistas == 1) {
                            printf("SEM ESPECIALISTAS NO SISTEMA\n");
                        }
                    }
                    n_especialistas--;
                }
                if (mensagem.tipo == FIM_CONSULTA_MEDICO) {
                    printf("%s\n", mensagem.mensagem);
                    tipo_medico temp;
                    ordena_especialistas(n_especialistas, especialistas);
                    especialistas[n_especialistas - 1].pid_cliente = 0;

                    for (int i = 0; i < n_clientes; i++) {
                        if (strcmp(clientes[i].sintomas, "FICA_FILA_ESPERA") == 0) {
                            for (int j = 0; j < n_especialistas; j++) {
                                if (strcmp(clientes[i].especialidade, especialistas[j].especialidade) == 0 &&
                                    mensagem.pid == especialistas[j].pid) {
                                    strcpy(clientes[i].sintomas, "SAI_FILA_ESPERA");
                                    if (strcmp(clientes[i].especialidade, "ortopedia") == 0) {
                                        retira_fila_especialidade(n_clientes_orto, ortopedia, c);
                                        especialistas[index].pid_cliente = ortopedia[n_clientes_orto - 1].pid;
                                        mensagem.tipo = INICIO_CONSULTA;
                                        mensagem.pid = especialistas[index].pid;
                                        snprintf(mensagem.mensagem, sizeof(mensagem.mensagem),
                                                 "Vai ser atendido pelo Dr.(a) %s", especialistas[index].nome);
                                        envia_mensagem(ortopedia[n_clientes_orto - 1].pid, mensagem, CLIENTE_FIFO);
                                        mensagem.tipo = INICIO_CONSULTA;
                                        mensagem.pid = ortopedia[n_clientes_orto - 1].pid;
                                        snprintf(mensagem.mensagem, sizeof(mensagem.mensagem),
                                                 "Vai atender o cliente %s",
                                                 ortopedia[n_clientes_orto - 1].nome);
                                        envia_mensagem(especialistas[index].pid, mensagem, MEDICO_FIFO);
                                        (n_clientes_orto)--;
                                        ordena_especialistas(n_especialistas, especialistas);
                                        printa_especialistas(especialistas, n_especialistas);
                                    }
                                    if (strcmp(clientes[i].especialidade, "estomatologia") == 0) {
                                        retira_fila_especialidade(n_clientes_est, estomatologia, c);
                                        especialistas[index].pid_cliente = estomatologia[n_clientes_est - 1].pid;
                                        mensagem.tipo = INICIO_CONSULTA;
                                        mensagem.pid = especialistas[index].pid;
                                        snprintf(mensagem.mensagem, sizeof(mensagem.mensagem),
                                                 "Vai ser atendido pelo Dr.(a) %s", especialistas[index].nome);
                                        envia_mensagem(estomatologia[n_clientes_est - 1].pid, mensagem, CLIENTE_FIFO);
                                        mensagem.tipo = INICIO_CONSULTA;
                                        mensagem.pid = estomatologia[n_clientes_est - 1].pid;
                                        snprintf(mensagem.mensagem, sizeof(mensagem.mensagem),
                                                 "Vai atender o cliente %s",
                                                 estomatologia[n_clientes_est - 1].nome);
                                        envia_mensagem(especialistas[index].pid, mensagem, MEDICO_FIFO);
                                        (n_clientes_est)--;
                                        ordena_especialistas(n_especialistas, especialistas);
                                        printa_especialistas(especialistas, n_especialistas);
                                    }
                                    if (strcmp(clientes[i].especialidade, "geral") == 0) {
                                        retira_fila_especialidade(n_clientes_geral, geral, c);
                                        especialistas[index].pid_cliente = geral[n_clientes_geral - 1].pid;
                                        mensagem.tipo = INICIO_CONSULTA;
                                        mensagem.pid = especialistas[index].pid;
                                        snprintf(mensagem.mensagem, sizeof(mensagem.mensagem),
                                                 "Vai ser atendido pelo Dr.(a) %s", especialistas[index].nome);
                                        envia_mensagem(geral[n_clientes_geral - 1].pid, mensagem, CLIENTE_FIFO);
                                        mensagem.tipo = INICIO_CONSULTA;
                                        mensagem.pid = geral[n_clientes_geral - 1].pid;
                                        snprintf(mensagem.mensagem, sizeof(mensagem.mensagem),
                                                 "Vai atender o cliente %s",
                                                 geral[n_clientes_geral - 1].nome);
                                        envia_mensagem(especialistas[index].pid, mensagem, MEDICO_FIFO);
                                        (n_clientes_geral)--;
                                        ordena_especialistas(n_especialistas, especialistas);
                                        printa_especialistas(especialistas, n_especialistas);
                                    }
                                    if (strcmp(clientes[i].especialidade, "neurologia") == 0) {
                                        retira_fila_especialidade(n_clientes_neuro, neurologia, c);
                                        especialistas[index].pid_cliente = neurologia[n_clientes_neuro - 1].pid;
                                        mensagem.tipo = INICIO_CONSULTA;
                                        mensagem.pid = especialistas[index].pid;
                                        snprintf(mensagem.mensagem, sizeof(mensagem.mensagem),
                                                 "Vai ser atendido pelo Dr.(a) %s", especialistas[index].nome);
                                        envia_mensagem(neurologia[n_clientes_neuro - 1].pid, mensagem, CLIENTE_FIFO);
                                        printf("sintomas %s\n", neurologia[n_clientes_neuro - 1].sintomas);
                                        mensagem.tipo = INICIO_CONSULTA;
                                        mensagem.pid = neurologia[n_clientes_neuro - 1].pid;
                                        snprintf(mensagem.mensagem, sizeof(mensagem.mensagem),
                                                 "Vai atender o cliente %s",
                                                 neurologia[n_clientes_neuro - 1].nome);
                                        envia_mensagem(especialistas[index].pid, mensagem, MEDICO_FIFO);
                                        (n_clientes_neuro)--;
                                        ordena_especialistas(n_especialistas, especialistas);
                                        printa_especialistas(especialistas, n_especialistas);
                                    }
                                    if (strcmp(clientes[i].especialidade, "oftalmologia") == 0) {
                                        retira_fila_especialidade(n_clientes_oft, oftalmologia, c);
                                        especialistas[index].pid_cliente = oftalmologia[n_clientes_oft - 1].pid;
                                        mensagem.tipo = INICIO_CONSULTA;
                                        mensagem.pid = especialistas[index].pid;
                                        snprintf(mensagem.mensagem, sizeof(mensagem.mensagem),
                                                 "Vai ser atendido pelo Dr.(a) %s", especialistas[index].nome);
                                        envia_mensagem(oftalmologia[n_clientes_oft - 1].pid, mensagem, CLIENTE_FIFO);
                                        printf("sintomas %s\n", oftalmologia[n_clientes_oft - 1].sintomas);
                                        mensagem.tipo = INICIO_CONSULTA;
                                        mensagem.pid = neurologia[n_clientes_oft - 1].pid;
                                        snprintf(mensagem.mensagem, sizeof(mensagem.mensagem),
                                                 "Vai atender o cliente %s",
                                                 oftalmologia[n_clientes_oft - 1].nome);
                                        envia_mensagem(especialistas[index].pid, mensagem, MEDICO_FIFO);
                                        (n_clientes_oft)--;
                                        ordena_especialistas(n_especialistas, especialistas);
                                        printa_especialistas(especialistas, n_especialistas);
                                    }
                                } else{
                                    mensagem.tipo = ESPERA_CLIENTE;
                                    mensagem.pid = especialistas[n_especialistas - 1].pid_cliente;
                                    snprintf(mensagem.mensagem, sizeof(mensagem.mensagem),
                                             "Aguarde para o balcão lhe indicar um cliente...");
                                    envia_mensagem(especialistas[n_especialistas - 1].pid, mensagem, MEDICO_FIFO);
                                }
                            }
                        }
                    }

                }
                if (mensagem.tipo == TIPO_CHEGADA_CLIENTE) {
                    if (res < (int) sizeof(tipo_mensagem)) {
                        fprintf(stderr, "Recebido sintoma incompleta\n "
                                        "bytes lidos: %d", res);
                        continue;
                    }

                    c.pid = mensagem.pid;
                    strcpy(c.nome, mensagem.nome);
                    strcpy(c.sintomas, mensagem.mensagem);
                    classifica_sintomas(b_c, c_b, c.especialidade, &pid, c.sintomas);

                    sprintf(c_fifo_fname, CLIENTE_FIFO, c.pid);

                    c_fifo_fd = open(c_fifo_fname, O_RDWR);
                    if (c_fifo_fd == -1)
                        perror("\nErro a abrir o fifo do cliente para escrita");

                    if (res > 0) {
                        char *Divide_especialidade_recebida = strtok(c.especialidade," ");// Retorna a string até o espaço (especialidade)
                        strcpy(c.especialidade, Divide_especialidade_recebida);
                        Divide_especialidade_recebida = strtok(NULL, " ");
                        c.prioridade = atoi(Divide_especialidade_recebida);
                    } else
                        printf("\nSem resposta ou resposta incompreensível"
                               "[bytes lidos: %d]", res);

                    maxatingido = verifica_fila_espera(&c, n_clientes_oft, n_clientes_neuro, n_clientes_geral,
                                                       n_clientes_est, n_clientes_orto);
                    if (maxatingido) {
                        mensagem.tipo = SEM_VAGAS;
                        sprintf(mensagem.mensagem, "SEM VAGAS NA FILA %s PARA ENTRAR NO SISTEMA", c.especialidade);

                    } else {
                        tipo_cliente temp;
                        if (n_clientes >= N) {
                            mensagem.tipo = SEM_VAGAS;
                            strcpy(mensagem.mensagem, "SEM VAGAS PARA ENTRAR NO SISTEMA");
                        } else {
                            strcpy(mensagem.nome, c.nome);
                            mensagem.pid = c.pid;
                            sprintf(mensagem.mensagem, "BEM VINDO UTENTE %s\nAguarde por um medico...\n", c.nome);
                            strcpy(c.sintomas, "FICA_FILA_ESPERA");
                            envia_mensagem(c.pid, mensagem, CLIENTE_FIFO);
                            snprintf(mensagem.mensagem, sizeof(mensagem.mensagem), "", c.nome);
                            envia_mensagem(c.pid, mensagem, CLIENTE_FIFO);
                            clientes[n_clientes++] = c;
                            printf("\nAdicionado novo utente ao balcao com PID[%d]\n", c.pid);
                            verifica_iniciaconsulta(&m, &c, &mensagem, ortopedia, &n_clientes_orto, estomatologia,
                                                    &n_clientes_est, neurologia, &n_clientes_neuro, oftalmologia,
                                                    &n_clientes_oft, geral, &n_clientes_geral, especialistas,
                                                    &n_especialistas, clientes, &n_clientes);
                            maxatingido = false;
                            snprintf(mensagem.mensagem, sizeof(mensagem.mensagem),
                                     "Os seus sintomas foram classificados com prioridade %d na especialidade %s\nTem %d pessoas a sua frente",
                                     c.prioridade, c.especialidade, c.npessoas_frente);
                            mensagem.tipo = MSG_BALCAO;
                            mensagem.pid = getpid();
                        }
                    }
                    res = write(c_fifo_fd, &mensagem, sizeof(tipo_mensagem));
                    close(c_fifo_fd);
                }
                if (mensagem.tipo == TIPO_CHEGADA_MEDICO) {
                    if (res < (int) sizeof(tipo_mensagem)) {
                        fprintf(stderr, "Recebido sintoma incompleta\n "
                                        "bytes lidos: %d", res);
                        continue;
                    } else {
                        printf("\nAdicionado o especialista %s ao balcao com PID[%d]\n", mensagem.nome,
                               mensagem.pid);

                    }
                    if (n_especialistas > M) {
                        mensagem.pid = SEM_VAGAS;
                        printf("SEM VAGAS INSTANCIAS %d\n", mensagem.pid);
                        if (write(m_fifo_fd, &mensagem, sizeof(tipo_mensagem)) != sizeof(tipo_mensagem)) {
                            exit(EXIT_FAILURE);
                        }
                    } else {
                        tipo_cliente temp;
                        m.pid = mensagem.pid;
                        strcpy(m.nome, mensagem.nome);
                        strcpy(m.especialidade, mensagem.mensagem);
                        m.pid_cliente = 0;
                        especialistas[(n_especialistas)++] = m;

                        mensagem.tipo = MSG_BALCAO;
                        mensagem.pid = getpid();
                        snprintf(mensagem.mensagem, sizeof(mensagem.mensagem), "BEM VINDO DR.(a) %s\nAguarde...",
                                 especialistas[n_especialistas - 1].nome);
                        envia_mensagem(especialistas[n_especialistas - 1].pid, mensagem, MEDICO_FIFO);
                        mensagem.tipo = TIPO_CHEGADA_MEDICO;
                        verifica_iniciaconsulta(&m, &c, &mensagem, ortopedia, &n_clientes_orto, estomatologia,
                                                &n_clientes_est, neurologia, &n_clientes_neuro, oftalmologia,
                                                &n_clientes_oft, geral, &n_clientes_geral, especialistas,
                                                &n_especialistas, clientes, &n_clientes);

                    }
                }
            }
        }
    }

    close(c_b[1]);
    close(b_c[0]);
    close(c_fifo_fd);
    close(b_fifo_fd);
    unlink(BALCAO_FIFO);
    printf("Terminei Balcao....");
    return (EXIT_SUCCESS);
}