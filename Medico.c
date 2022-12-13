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
    char m_fifo_fname[BUFFER];
    mensagem1.pid = getpid();
    mensagem1.tipo = TIPO_SAIDA_MEDICO;
    envia_mensagem(0, mensagem1, BALCAO_FIFO);
    sprintf(m_fifo_fname, MEDICO_FIFO, getpid());
    unlink(m_fifo_fname);
    printf("Medico terminado abruptamente");
    exit(EXIT_SUCCESS);
}

void inicializa(tipo_mensagem *d1, char **argv) {

    strcpy(d1->nome, argv[1]);
    strcpy(d1->mensagem, argv[2]);
    d1->pid = getpid();
    d1->tipo = TIPO_CHEGADA_MEDICO;
}


int main(int argc, char **argv) {
    if (argc != 3) {
        printf("ERRO SINTAXE <NOME MEDICO>");
        return -1;
    }

    printf("PID %d\n", getpid());
    fprintf(stdout, "-------------BEM-VINDO MEDICO-------------\n");
    fd_set rfds;
    bool atender = false;
    bool sai = false;
    int retval;
    char str_escrita[BUFFER], str_leitura[BUFFER], buffer[BUFFER];
    tipo_mensagem mensagem;
    int b_fifo_fd = 0;
    int c_fifo_fd = 0;
    int m_fifo_fd = 0;
    int maxfd = 0;
    pid_t pid_cliente = 0; //pid cliente que e atendido
    char c_fifo_fname[25], m_fifo_fname[25];
    int read_res;

    signal(SIGINT, trata_ctrl_c);

    b_fifo_fd = open(BALCAO_FIFO, O_WRONLY);
    if (b_fifo_fd == -1) {
        fprintf(stderr, "\nO Balcão não está a correr\n");
        exit(EXIT_FAILURE);
    }


    inicializa(&mensagem, argv);


    sprintf(m_fifo_fname, MEDICO_FIFO, mensagem.pid);

    if (mkfifo(m_fifo_fname, 0777) == -1) {

        exit(EXIT_FAILURE);
    } else
        fprintf(stderr, "\nFIFO do Medico criado");


    m_fifo_fd = open(m_fifo_fname, O_RDWR);
    if (m_fifo_fd == -1) {
        perror("\nErro ao abrir o FIFO do Medico");
        close(b_fifo_fd);
        unlink(m_fifo_fname);
        exit(EXIT_FAILURE);
    }

    if (write(b_fifo_fd, &mensagem, sizeof(tipo_mensagem)) == -1) {
        close(b_fifo_fd);
        close(m_fifo_fd);
        unlink(BALCAO_FIFO);
        exit(EXIT_FAILURE);
    }

    if (read(m_fifo_fd, &mensagem, sizeof(tipo_mensagem)) == -1) {
        close(b_fifo_fd);
        close(m_fifo_fd);
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
        FD_SET(m_fifo_fd, &rfds);
        maxfd = STDIN_FILENO > m_fifo_fd ? STDIN_FILENO : m_fifo_fd;
        retval = select(maxfd + 1, &rfds, NULL, NULL, NULL);

        if (retval == -1) {
            perror("select()");
            exit(EXIT_FAILURE);
        } else if (retval == 0) { // timeout
            printf("Terminou o tempo de espera.\n");
        } else {
            if (FD_ISSET(STDIN_FILENO, &rfds)) { // é para enviar mensagem
                fgets(str_escrita, BUFFER, stdin);
                str_escrita[strlen(str_escrita) - 1] = '\0';

                if (!atender) {
                    if (strcmp(str_escrita, MSG_SAIDA) == 0) {
                        printf("Sai do sistema\n");
                        mensagem.tipo = TIPO_SAIDA_MEDICO;
                        mensagem.pid = getpid();
                        strcpy(mensagem.mensagem, str_escrita);
                        snprintf(mensagem.mensagem, BUFFER, "Medico %s [%d] terminou sessão", argv[1], getpid());
                        envia_mensagem(mensagem.pid, mensagem, BALCAO_FIFO);
                        unlink(m_fifo_fname);
                        sai = true;
                    } else {
                        printf("Dr(a). %s aguarde por um cliente\n", argv[1]);
                    }
                } else {
                    mensagem.tipo = MSG_MEDICO;
                    mensagem.pid = getpid();
                    strcpy(mensagem.mensagem, str_escrita);
                    snprintf(mensagem.mensagem, BUFFER, "Dr(a). %s diz: %s", argv[1], str_escrita);
                    envia_mensagem(pid_cliente, mensagem, CLIENTE_FIFO);
                    if (strcmp(str_escrita, MSG_SAIDA) == 0) {
                        mensagem.pid = getpid();
                        strcpy(mensagem.nome, argv[1]);
                        sprintf(mensagem.mensagem, "%s", MSG_SAIDA);
                        envia_mensagem(pid_cliente, mensagem, CLIENTE_FIFO);
                        snprintf(mensagem.mensagem, BUFFER, "Medico %s [%d] terminou a consulta", argv[1], getpid());
                        envia_mensagem(pid_cliente, mensagem, BALCAO_FIFO);
                        mensagem.tipo = FIM_CONSULTA_MEDICO;
                        snprintf(mensagem.mensagem, BUFFER, "Medico %s [%d] terminou a consulta", argv[1], getpid());
                        envia_mensagem(mensagem.pid, mensagem, BALCAO_FIFO);
                        atender = false;
                    }
                }
            }

            if (FD_ISSET(m_fifo_fd, &rfds)) { // é para receber mensagem

                if (read(m_fifo_fd, &mensagem, sizeof(tipo_mensagem)) == -1) {
                    exit(EXIT_FAILURE);
                }

                if (!atender) {
                    if (strcmp(mensagem.mensagem, MSG_SAIDA) == 0) {
                        mensagem.tipo = TIPO_SAIDA_MEDICO;
                        mensagem.pid = getpid();
                        snprintf(mensagem.mensagem, BUFFER, "Medico %s [%d]  terminou sessão", argv[1], getpid());
                        envia_mensagem(mensagem.pid, mensagem, BALCAO_FIFO);
                        unlink(m_fifo_fname);
                        sai = true;
                    }
                    if (mensagem.tipo == TIPO_SAIDA_MEDICO) { // mandado terminar pelo balcao
                        printf("%s\n", mensagem.mensagem);
                        mensagem.pid = getpid();
                        unlink(m_fifo_fname);
                        sai = true;
                    }
                    if (mensagem.tipo == INICIO_CONSULTA) {
                        pid_cliente = mensagem.pid;
                        printf("%s", mensagem.mensagem);
                        atender = true;
                    }
                    if (mensagem.tipo == ESPERA_CLIENTE) {
                        printf("%s\n", mensagem.mensagem);
                    }

                } else {
                    if (strcmp(mensagem.mensagem, MSG_SAIDA) == 0) { // cliente diz adeus
                        printf("%s\n", mensagem.mensagem);
                        mensagem.tipo = FIM_CONSULTA_MEDICO;
                        mensagem.pid = getpid();
                        snprintf(mensagem.mensagem, BUFFER, "Medico %s [%d] saiu da consulta terminada pelo cliente",
                                 argv[1], getpid());
                        envia_mensagem(mensagem.pid, mensagem, BALCAO_FIFO);
                        atender = false;
                    }
                    if (mensagem.tipo == TIPO_SAIDA_MEDICO) { // mandado terminar pelo balcao
                        mensagem.pid = getpid();
                        unlink(m_fifo_fname);
                        sai = true;
                    } else {

                        printf("%s\n", mensagem.mensagem);
                    }
                }
            }
        }
    }
    close(m_fifo_fd);
    close(b_fifo_fd);
    unlink(m_fifo_fname);
    printf("Terminei Medico....");
    return (0);
}
