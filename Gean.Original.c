#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

// Variável global compartilhada (sem proteção)
int saldo_conta = 1000;

/* Thread 1: Marido fazendo um depósito na conta conjunta */
void thread_deposito(void)
{
    while (1) {
        // 1. Lê o saldo atual
        int saldo_temporario = saldo_conta; 
        
        // 2. O Zephyr pausa esta thread aqui (simulando a lentidão do sistema bancário).
        // Isso permite que a thread de saque inicie sua leitura ANTES do depósito terminar.
        k_sleep(K_MSEC(10)); 
        
        // 3. Calcula e salva o novo saldo
        saldo_temporario += 500; 
        saldo_conta = saldo_temporario;
        
        printk("Deposito de R$500. Saldo final gravado: R$%d\n", saldo_conta);
        
        k_sleep(K_MSEC(2000)); // Aguarda antes do próximo depósito
    }
}

/* Thread 2: Esposa fazendo um saque na mesma conta conjunta */
void thread_saque(void)
{
    // Atraso inicial mínimo para alinhar as duas threads
    k_sleep(K_MSEC(2)); 

    while (1) {
        // 1. Lê o saldo atual (que ainda não recebeu o depósito porque a outra thread dormiu)
        int saldo_temporario = saldo_conta;
        
        // 2. Simula lentidão no caixa eletrônico
        k_sleep(K_MSEC(10)); 
        
        // 3. Calcula e salva o novo saldo, SOBRESCREVENDO o depósito da outra thread
        saldo_temporario -= 200; 
        saldo_conta = saldo_temporario;
        
        printk("Saque de R$200. Saldo final gravado: R$%d\n", saldo_conta);
        
        k_sleep(K_MSEC(2000)); // Aguarda antes do próximo saque
    }
}

// Configuração das Threads com a mesma prioridade para forçar a disputa de CPU
#define TAMANHO_PILHA 1024
#define PRIORIDADE 5

K_THREAD_DEFINE(id_deposito, TAMANHO_PILHA, thread_deposito, NULL, NULL, NULL, PRIORIDADE, 0, 0);
K_THREAD_DEFINE(id_saque, TAMANHO_PILHA, thread_saque, NULL, NULL, NULL, PRIORIDADE, 0, 0);