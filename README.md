Race Conditions com Mutex
Integrantes: Ingrid Oliveira e Giovanna Santana
Link Repositório Ingrid: https://github.com/infriglide/PSI-Microcontroladores2-Aula06



**Cenário 1: Contador**

1.1 Race condition: as duas threads leem o mesmo valor de contado, cedem a CPU e escrevem temp + 1, sobrescrevendo uma à outra. Cada par de incrementos soma só 1. Com N = 1000, saída.

Esperado: 2000 | Obtido: 1000. O erro ocorreria sempre que as duas leem antes de qualquer uma escrever.

 Solução do Cenário 1:
 Foi adicionado: mutex (K_MUTEX_DEFINE) cobrindo leitura e escrita juntos.

Evidências: as evidências podem ser encontrados na pasta evidências do repositório: 
https://github.com/infriglide/PSI-Microcontroladores2-Aula06/tree/main/evidencias


**Cenário 2: Saque**

2.1 Race condition: cada thread verifica saldo >= 80 e depois subtrai 80. Com saldo 100, as duas passam na verificação antes de qualquer subtração e o saldo termina em -60. Com saldos que cobrem os dois saques, o erro não aparece.

Solução do Cenário 2:
Foi adicionado: mutex cobrindo a verificação e a subtração, então a segunda thread só verifica depois que a primeira terminou.

**Mutex e Não Semáforo:**
O problema é exclusão mútua: só uma thread por vez na seção crítica, e quem trava é quem libera. O mutex tem dono e herança de prioridade no Zephyr. Um semáforo binário também funcionaria, mas é mais indicado para sinalização entre threads.



Avaliação Cruzada:






