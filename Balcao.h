// ISEC SISTEMAS OPERATIVOS 21/22// Created by// Francisco Simões 2019133920// Pedro Praça 2020130980#ifndef TPSO_BALCAO_H#define TPSO_BALCAO_H#include "Medico.h"#include "Cliente.h"#include <unistd.h>#include <stdlib.h>#include <stdio.h>#include <string.h>#include <fcntl.h>#include <limits.h>#include <sys/types.h>#include <sys/stat.h>#include <ctype.h>#include <signal.h>#include <sys/time.h>#include <sys/select.h>#include <errno.h>#include <pthread.h>#include <stdbool.h>#define BUFFER 256#define BALCAO_FIFO "/tmp/balcao_fifo"#define SEM_VAGAS (-1)#define SEM_VAGAS_FILA_ESPERA (-2)#define ESP_QUEUE 5#define TIPO_CHEGADA_MEDICO 0#define TIPO_CHEGADA_CLIENTE 1#define TIPO_SAIDA_MEDICO 2#define TIPO_SAIDA_CLIENTE 3#define MSG_BALCAO 12#define MSG_CLIENTE 11#define INICIO_CONSULTA 15#define FIM_CONSULTA_MEDICO 15#define FIM_CONSULTA_CLIENTE 16#define MSG_MEDICO 13#define ESPERA_CLIENTE (-1)#define ESPERA_MEDICO (-2)// estrutura de mensagem correspondente a um pedido cliente->balcaotypedef struct {    pid_t pid;    int tipo;    char mensagem[BUFFER]; // mensagem / sintomas do cliente / especialidade do medico    char nome[BUFFER]; //nome especialista / nome cliente} tipo_mensagem;typedef struct {    pid_t pid;    char nome[BUFFER]; //nome cliente    char sintomas[BUFFER]; // sintomas cliente    char especialidade[BUFFER];//especialidade obtida de cada cliente pós classificador    int prioridade; // prioridade cliente    int npessoas_frente; // quantidade de pessoas a frente} tipo_cliente;typedef struct {    pid_t pid;    char nome[BUFFER]; //nome cliente    char especialidade[BUFFER];//especialidade obtida de cada cliente pós classificador    pid_t pid_cliente;} tipo_medico;void envia_mensagem(pid_t pid, tipo_mensagem mensagem, char *fifo_path);void le_variaveis_ambiente(int *MAXCLIENTES, int *MAXMEDICOS);int abre_pipes(int b_c[2], int c_b[2], int *pid);int classifica_sintomas(int *b_c, int *c_b, char *especialidade, int *pid, char *sintomas);void printa_especialistas(tipo_medico *especialistas, int n_especialistas);void printa_clientes(tipo_cliente *clientes, int n_clientes);bool verifica_fila_espera(tipo_cliente *c, int n_clientes_oft, int n_clientes_neuro, int n_clientes_geral,                          int n_clientes_est, int n_clientes_orto);void insere_na_especialidade(tipo_cliente *c, int *n_clientes, tipo_cliente *especialidade);void retira_fila_especialidade(int n_clientes_esp, tipo_cliente *especialidade,                               tipo_cliente c);void ordena_especialistas(int n_especialistas, tipo_medico *especialistas);voidprocura_index(int n_especialistas, tipo_medico *especialistas, tipo_cliente *especialidade, int *index, int n_clientes,              char qual[BUFFER]);void procura_index_cliente(int n_especialistas, tipo_medico *especialistas, tipo_cliente *especialidade, int *index,                           int n_clientes);void printa_comandos();void remove_cliente(char *nomerecebido, tipo_cliente *clientes, int *n_clientes, tipo_cliente *ortopedia,                    int *n_clientes_orto, tipo_cliente *estomatologia, int *n_clientes_est, tipo_cliente *neurologia,                    int *n_clientes_neuro, tipo_cliente *oftalmologia, int *n_clientes_oft, tipo_cliente *geral,                    int *n_clientes_geral);void remove_especialistas(char *nomerecebido, tipo_medico *especialistas, int *n_especialistas);void verifica_iniciaconsulta(tipo_medico *m, tipo_cliente *c, tipo_mensagem *mensagem, tipo_cliente *ortopedia,                             int *n_clientes_orto, tipo_cliente *estomatologia, int *n_clientes_est,                             tipo_cliente *neurologia, int *n_clientes_neuro, tipo_cliente *oftalmologia,                             int *n_clientes_oft, tipo_cliente *geral, int *n_clientes_geral,                             tipo_medico *especialistas, int *n_especialistas, tipo_cliente *clientes,                             int *n_clientes);#endif //TPSO_BALCAO_H