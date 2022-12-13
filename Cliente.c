// ISEC SISTEMAS OPERATIVOS 21/22
// Created by
// Francisco Simões 2019133920
// Pedro Praça 2020130980



#include "Cliente.h"
#include "Balcao.h"
#include <stdbool.h>

#define MSG_SAIDA "adeus"

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
        exit(EXIT_FAILURE);
    }
    close(fifo_fd);
}

void trata_ctrl_c(int s) {
    tipo_mensagem mensagem1;
    char c_fifo_fname[BUFFER];
    mensagem1.pid = getpid();
    mensagem1.tipo = TIPO_SAIDA_CLIENTE;
    sprintf(mensagem1.mensagem, "Cliente [%d] terminou abruptamente", getpid());
    envia_mensagem(0, mensagem1, BALCAO_FIFO);
    sprintf(c_fifo_fname, CLIENTE_FIFO, getpid());
    unlink(c_fifo_fname);
    printf("Cliente terminado abruptamente");
    exit(EXIT_SUCCESS);
}

void inicializa(tipo_mensagem *d1, char **argv) {

    printf("INICIALIZA\n");
    strcpy(d1->nome, argv[1]);
    d1->pid = getpid();
    d1->tipo = TIPO_CHEGADA_CLIENTE;
}


int main(int argc, char **argv) {
    if (argc > 2) {
        printf("ERRO SINTAXE <NOME UTENTE>");
        return -1;
    }
    printf("PID %d\n", getpid());
    fprintf(stdout, "-------------BEM-VINDO A CLIENTE-------------\n");
    fd_set rfds;
    bool atendido = false;
    bool sai = false;
    int retval;
    char str_escrita[BUFFER], str_leitura[BUFFER], buffer[BUFFER];
    tipo_mensagem mensagem;
    int b_fifo_fd = 0;
    int c_fifo_fd = 0;
    int m_fifo_fd = 0;
    int maxfd = 0;
    pid_t pid_medico = 0; //pid medico que vai atender
    char c_fifo_fname[25], m_fifo_fname[25];    /* nome do FIFO deste cliente */
    int read_res;
    signal(SIGINT, trata_ctrl_c);

    b_fifo_fd = open(BALCAO_FIFO, O_WRONLY);
    if (b_fifo_fd == -1) {
        fprintf(stderr, "\nO Balcão não está a correr\n");
        exit(EXIT_FAILURE);
    }
    inicializa(&mensagem, argv);

    sprintf(c_fifo_fname, CLIENTE_FIFO, mensagem.pid);

    if (mkfifo(c_fifo_fname, 0777) == -1) {
        exit(EXIT_FAILURE);
    } else
        fprintf(stderr, "\nFIFO do cliente criado");

    c_fifo_fd = open(c_fifo_fname, O_RDWR);
    if (c_fifo_fd == -1) {
        perror("\nErro ao abrir o FIFO do cliente");
        close(b_fifo_fd);
        unlink(c_fifo_fname);
        exit(EXIT_FAILURE);
    }


    printf("\nSintomas -> ");
    fflush(stdin);
    fgets(mensagem.mensagem, BUFFER, stdin);


    if (write(b_fifo_fd, &mensagem, sizeof(tipo_mensagem)) != sizeof(tipo_mensagem)) {
        close(b_fifo_fd);
        close(c_fifo_fd);
        unlink(BALCAO_FIFO);
        exit(EXIT_FAILURE);
    }

    if (read(c_fifo_fd, &mensagem, sizeof(tipo_mensagem)) != sizeof(tipo_mensagem)) {
        close(b_fifo_fd);
        close(c_fifo_fd);
        unlink(BALCAO_FIFO);
        exit(EXIT_FAILURE);
    }

    printf("%s\n", mensagem.mensagem); // escreve a mensagem recebida
    if (mensagem.tipo == SEM_VAGAS) {
        exit(EXIT_SUCCESS);
    }
    if (mensagem.tipo == SEM_VAGAS_FILA_ESPERA) {
        exit(EXIT_SUCCESS);
    }
    while (!sai) {
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(c_fifo_fd, &rfds);
        maxfd = STDIN_FILENO > c_fifo_fd ? STDIN_FILENO : c_fifo_fd;
        retval = select(maxfd + 1, &rfds, NULL, NULL, NULL);

        if (retval == -1) {
            perror("select()");
            exit(EXIT_FAILURE);
        } else {
            if (FD_ISSET(STDIN_FILENO, &rfds)) { // é para enviar mensagem
                fgets(str_escrita, BUFFER, stdin);
                str_escrita[strlen(str_escrita) - 1] = '\0';
                // se estiver a ser atendido
                // enviar mensagem pro balcao a dizer que vai sair
                if (!atendido) {
                    if (strcmp(str_escrita, MSG_SAIDA) == 0) {
                        mensagem.tipo = TIPO_SAIDA_CLIENTE;
                        snprintf(mensagem.mensagem, BUFFER, "Cliente %s [%d] terminou a sua sessao", argv[1], getpid());
                        envia_mensagem(getpid(), mensagem, BALCAO_FIFO);
                        unlink(c_fifo_fname);
                        sai = true;
                    } else {
                        printf("Aguarde um medico\n");
                    }
                } else {
                    mensagem.tipo = MSG_CLIENTE;
                    mensagem.pid = getpid();
                    strcpy(mensagem.mensagem, str_escrita);
                    snprintf(mensagem.mensagem, BUFFER, "[Medico] %s diz: %s", argv[1], str_escrita);
                    envia_mensagem(pid_medico, mensagem, MEDICO_FIFO);
                    if (strcmp(str_escrita, MSG_SAIDA) == 0) {
                        mensagem.tipo = FIM_CONSULTA_CLIENTE;
                        mensagem.pid = getpid();
                        strcpy(mensagem.mensagem, str_escrita);
                        snprintf(mensagem.mensagem, BUFFER, "Cliente %s [%d] terminou a consulta", argv[1], getpid());
                        envia_mensagem(mensagem.pid, mensagem, BALCAO_FIFO);
                        snprintf(mensagem.mensagem, BUFFER, "adeus", argv[1], getpid());
                        envia_mensagem(pid_medico, mensagem, MEDICO_FIFO);
                        unlink(c_fifo_fname);
                        atendido = false;
                        sai = true;
                    }
                }
            }

            if (FD_ISSET(c_fifo_fd, &rfds)) { // é para receber mensagem
                if (read(c_fifo_fd, &mensagem, sizeof(tipo_mensagem)) == -1) {
                    exit(EXIT_FAILURE);
                }
                if (!atendido) {
                    if (mensagem.tipo == TIPO_SAIDA_CLIENTE) { // mandado terminar pelo balcao
                        mensagem.pid = getpid();
                        unlink(c_fifo_fname);
                        sai = true;
                    }
                    if (mensagem.tipo == INICIO_CONSULTA) {
                        pid_medico = mensagem.pid;
                        printf("%s\n", mensagem.mensagem);
                        atendido = true;
                    } else {
                        printf("%s\n", mensagem.mensagem);
                    }
                } else {
                    if (strcmp(mensagem.mensagem, MSG_SAIDA) == 0) {
                        printf("%s\n", mensagem.mensagem);
                        mensagem.tipo = TIPO_SAIDA_CLIENTE;
                        mensagem.pid = getpid();
                        snprintf(mensagem.mensagem, BUFFER, "Cliente %s [%d] saiu da consulta e terminou sessão",
                                 argv[1], getpid());
                        envia_mensagem(mensagem.pid, mensagem, BALCAO_FIFO);
                        unlink(c_fifo_fname);
                        atendido = false;
                        sai = true;
                    } else if (mensagem.tipo == TIPO_SAIDA_CLIENTE) { // mandado terminar pelo balcao
                        mensagem.pid = getpid();
                        unlink(c_fifo_fname);
                        sai = true;
                    } else {
                        printf("%s\n", mensagem.mensagem);
                    }
                }
            }

        }
    }
    close(c_fifo_fd);
    close(b_fifo_fd);
    unlink(c_fifo_fname);
    printf("Terminei Cliente....");

    return (0);
}