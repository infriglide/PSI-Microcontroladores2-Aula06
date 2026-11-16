Código Exemplo com Race Condition
========================================

O cenário escolhido para ilustrar a condição de corrida (race condition) é 
o de uma conta bancária conjunta. Neste exemplo, duas pessoas (representadas 
por duas threads rodando no FRDM KL25Z) tentam acessar e modificar o saldo 
da mesma conta bancária ao mesmo tempo.

Como a abordagem mais simples para comunicação paralela é o uso de variáveis 
globais para compartilhar os dados entre as threads, a ausência de mecanismos 
de proteção faz com que uma operação atropele a outra.

A falha ocorre porque a operação matemática foi dividida em três etapas: 
1. ler a global; 
2. modificar a local; 
3. gravar na global.

O que causa a Race Condition
========================================
Quando a thread_deposito lê que o saldo é R$ 1000 e entra em pausa (k_sleep), 
o escalonador do Zephyr cede a CPU para a thread_saque. A segunda thread também 
lê o saldo original de R$ 1000, subtrai 200 e salva R$ 800 na global. Logo em 
seguida, a primeira thread acorda, termina sua conta (1000 + 500) e salva R$ 1500 
na global, apagando completamente o saque que havia acabado de acontecer.