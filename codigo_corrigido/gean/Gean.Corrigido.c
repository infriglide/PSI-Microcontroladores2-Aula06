#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

// Variável global compartilhada, agora protegida por mutex
int saldo_conta = 1000;
K_MUTEX_DEFINE(saldo_mutex);

/* Thread 1: Marido fazendo um depósito na conta conjunta */
void thread_deposito(void)
{
    while (1) {
        k_mutex_lock(&saldo_mutex, K_FOREVER);

        int saldo_temporario = saldo_conta;
        k_sleep(K_MSEC(10));
        saldo_temporario += 500;
        saldo_conta = saldo_temporario;

        printk("Deposito de R$500. Saldo final gravado: R$%d\n", saldo_conta);

        k_mutex_unlock(&saldo_mutex);

        k_sleep(K_MSEC(2000));
    }
}

/* Thread 2: Esposa fazendo um saque na mesma conta conjunta */
void thread_saque(void)
{
    k_sleep(K_MSEC(2));

    while (1) {
        k_mutex_lock(&saldo_mutex, K_FOREVER);

        int saldo_temporario = saldo_conta;
        k_sleep(K_MSEC(10));
        saldo_temporario -= 200;
        saldo_conta = saldo_temporario;

        printk("Saque de R$200. Saldo final gravado: R$%d\n", saldo_conta);

        k_mutex_unlock(&saldo_mutex);

        k_sleep(K_MSEC(2000));
    }
}

#define TAMANHO_PILHA 1024
#define PRIORIDADE 5

K_THREAD_DEFINE(id_deposito, TAMANHO_PILHA, thread_deposito, NULL, NULL, NULL, PRIORIDADE, 0, 0);
K_THREAD_DEFINE(id_saque, TAMANHO_PILHA, thread_saque, NULL, NULL, NULL, PRIORIDADE, 0, 0);
