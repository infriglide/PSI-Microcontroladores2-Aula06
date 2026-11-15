#include <kernel.h>
#include <sys/printk.h>



#define N_INCREMENTOS 1000
#define STACK_SIZE 512

volatile int contador = 0;

void incrementar(void *a, void *b, void *c)
{
    for (int i = 0; i < N_INCREMENTOS; i++) {
        int temp = contador;   // lê
        k_yield();             // força troca de thread
        contador = temp + 1;   // escreve valor desatualizado
    }
}

K_THREAD_DEFINE(t1, STACK_SIZE, incrementar, NULL, NULL, NULL, 5, 0, 0);
K_THREAD_DEFINE(t2, STACK_SIZE, incrementar, NULL, NULL, NULL, 5, 0, 0);

void main(void)
{
    printk("Iniciando...\n");
    while (1) {
        k_msleep(2000);
        printk("Esperado: %d | Obtido: %d\n", 2 * N_INCREMENTOS, contador);
    }
}