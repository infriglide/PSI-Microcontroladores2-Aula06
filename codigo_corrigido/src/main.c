/*
 * CÓDIGO CORRIGIDO
 *
 * Executa em sequência os dois cenários:
 *   1) Contador: corrigido com SEMÁFORO BINÁRIO (k_sem inicializado com 1).
 *   2) Saque:    corrigido com MUTEX (k_mutex).
 *
 * Para mudar os testes, altere os #define abaixo:
 *   ENTRADAS_POR_PORTA (cenário 1) e SALDO_INICIAL (cenário 2).
 */
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdint.h>

#ifndef ENTRADAS_POR_PORTA
#define ENTRADAS_POR_PORTA 100
#endif

#ifndef SALDO_INICIAL
#define SALDO_INICIAL 100
#endif

#define TAM_PILHA 1024
#define PRIO      5

/* ===================== CENÁRIO 1: CONTADOR (semáforo) ===================== */
#define NUM_PORTAS 3

static int pessoas_dentro;
static struct k_sem trava_contador;
static const char *nome_porta[NUM_PORTAS] = { "Norte", "Sul", "Leste" };

K_THREAD_STACK_ARRAY_DEFINE(pilhas_portas, NUM_PORTAS, TAM_PILHA);
static struct k_thread portas[NUM_PORTAS];

static void registrar_entradas(void *arg1, void *arg2, void *arg3)
{
	int id = (int)(intptr_t)arg1;
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);

	for (int i = 0; i < ENTRADAS_POR_PORTA; i++) {
		k_sem_take(&trava_contador, K_FOREVER);   /* pede a permissão */
		int lido = pessoas_dentro;
		k_msleep(1);
		pessoas_dentro = lido + 1;
		k_sem_give(&trava_contador);              /* devolve a permissão */
	}
	printk("Porta %s terminou\n", nome_porta[id]);
}

static void teste_contador(void)
{
	int esperado = NUM_PORTAS * ENTRADAS_POR_PORTA;

	printk("===== CENARIO 1: CONTADOR =====\n");
	k_sem_init(&trava_contador, 1, 1);

	for (int i = 0; i < NUM_PORTAS; i++) {
		k_thread_create(&portas[i], pilhas_portas[i],
				K_THREAD_STACK_SIZEOF(pilhas_portas[i]),
				registrar_entradas, (void *)(intptr_t)i, NULL, NULL,
				PRIO, 0, K_NO_WAIT);
	}
	for (int i = 0; i < NUM_PORTAS; i++) {
		k_thread_join(&portas[i], K_FOREVER);
	}

	printk("Esperado: %d | Contado: %d\n", esperado, pessoas_dentro);
	if (pessoas_dentro != esperado) {
		printk("RESULTADO: *** RACE CONDITION (%d entradas perdidas) ***\n\n",
		       esperado - pessoas_dentro);
	} else {
		printk("RESULTADO: OK\n\n");
	}
}

/* ===================== CENÁRIO 2: SAQUE (mutex) ===================== */
#define VALOR_SAQUE  70
#define NUM_CLIENTES 3

static int saldo = SALDO_INICIAL;
static struct k_mutex trava_conta;
static const char *canal[NUM_CLIENTES] = { "Caixa eletronico", "App", "Agencia" };

K_THREAD_STACK_ARRAY_DEFINE(pilhas_clientes, NUM_CLIENTES, TAM_PILHA);
static struct k_thread clientes[NUM_CLIENTES];

static void tentar_sacar(void *arg1, void *arg2, void *arg3)
{
	int id = (int)(intptr_t)arg1;
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);

	k_mutex_lock(&trava_conta, K_FOREVER);        /* entra na seção crítica */
	if (saldo >= VALOR_SAQUE) {
		k_msleep(5);
		saldo = saldo - VALOR_SAQUE;
		printk("[%s] saque de %d aprovado. Saldo: %d\n", canal[id], VALOR_SAQUE, saldo);
	} else {
		printk("[%s] saque de %d recusado. Saldo: %d\n", canal[id], VALOR_SAQUE, saldo);
	}
	k_mutex_unlock(&trava_conta);                 /* sai da seção crítica */
}

static void teste_saque(void)
{
	printk("===== CENARIO 2: SAQUE =====\n");
	k_mutex_init(&trava_conta);

	for (int i = 0; i < NUM_CLIENTES; i++) {
		k_thread_create(&clientes[i], pilhas_clientes[i],
				K_THREAD_STACK_SIZEOF(pilhas_clientes[i]),
				tentar_sacar, (void *)(intptr_t)i, NULL, NULL,
				PRIO, 0, K_NO_WAIT);
	}
	for (int i = 0; i < NUM_CLIENTES; i++) {
		k_thread_join(&clientes[i], K_FOREVER);
	}

	printk("Saldo inicial: %d | Saque por canal: %d | Saldo final: %d\n",
	       SALDO_INICIAL, VALOR_SAQUE, saldo);
	if (saldo < 0) {
		printk("RESULTADO: *** RACE CONDITION (saldo negativo) ***\n");
	} else {
		printk("RESULTADO: OK\n");
	}
}

int main(void)
{
	teste_contador();
	teste_saque();
	return 0;
}
