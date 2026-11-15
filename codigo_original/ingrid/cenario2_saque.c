#include <kernel.h>
#include <sys/printk.h>
#include <stdint.h>

#define SALDO_INICIAL 100
#define VALOR_SAQUE 80
#define STACK_SIZE 1024

volatile int saldo = SALDO_INICIAL;

void sacar(void *a, void *b, void *c)
{
    int id = (int)(intptr_t)a;

    if (saldo >= VALOR_SAQUE) {        // verifica
        k_yield();                     // força troca de thread
        saldo = saldo - VALOR_SAQUE;   // age com base na verificação antiga
        printk("Thread %d: saque de %d efetuado. Saldo: %d\n", id, VALOR_SAQUE, saldo);
    } else {
        printk("Thread %d: saque negado. Saldo: %d\n", id, saldo);
    }
}

K_THREAD_DEFINE(t1, STACK_SIZE, sacar, (void *)1, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(t2, STACK_SIZE, sacar, (void *)2, NULL, NULL, 5, 0, 0);

void main(void)
{
    printk("Iniciando... saldo inicial: %d\n", SALDO_INICIAL);
    while (1) {
        k_msleep(2000);
        printk("Esperado: saldo >= 0 | Obtido: %d\n", saldo);
    }
}