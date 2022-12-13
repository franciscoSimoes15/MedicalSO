// ISEC SISTEMAS OPERATIVOS 21/22
// Created by
// Francisco Simões 2019133920
// Pedro Praça 2020130980


#ifndef TPSO_CLIENTE_H
#define TPSO_CLIENTE_H

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "Balcao.h"

#define CLIENTE_FIFO "/tmp/cliente_%d_fifo"
#define MEDICO_FIFO "/tmp/medico_%d_fifo"
#define BUFFER_CLIENTE 20



void trata_ctrl_c(int s);




#endif //TPSO_CLIENTE_H
