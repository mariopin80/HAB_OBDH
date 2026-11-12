# Histórico de versões

[Read in English](CHANGELOG.md) · [README do projeto](README.pt-BR.md)

Cada entrada compara o sketch do acervo com a versão anterior. As datas são as registradas nos cabeçalhos.

## v1

Arquivo do acervo `HAB_OBDH_v1.ino`

- Primeiro sketch do acervo. Lê GPS, temperaturas via OneWire e barômetro, grava as medições no cartão SD e transmite a estrutura `DATA` pelo E32 em operação diurna.
- Define os estados de inicialização, dia, noite e segurança. Bateria e painel solar aparecem no pacote como campos sem rotina de aquisição. O estado de segurança ainda é um esboço.
- O cabeçalho com a versão foi acrescentado na reconstrução do histórico.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/60e64f053900ab45eb1fdfecb252611bf1f2b27b)

## v1.1

Arquivo do acervo `HAB_OBDH_v1.1.ino` · 05/11/2020

- Adiciona a leitura da bateria em A0 e do painel solar em A1. O modo de segurança reavalia a bateria e retorna à inicialização quando a leitura supera o limite.
- Substitui vários campos de ponto flutuante por inteiros, multiplica temperaturas por dez e registra a pressão bruta do barômetro. Inclui HDOP, satélites e leituras do ADC no log do SD.
- Ativa diagnóstico serial a 9600 baud. A inicialização aguarda enquanto houver menos de seis satélites, HDOP acima de 10 e leitura de bateria acima de 9.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/8fdac7df3829db582f90146b6ce9593aaf6231e0) · [Comparar com v1](https://github.com/mariopin80/HAB_OBDH/compare/60e64f053900ab45eb1fdfecb252611bf1f2b27b...8fdac7df3829db582f90146b6ce9593aaf6231e0)

## v1.2

Arquivo do acervo `HAB_OBDH_v1.2.ino` · 11/11/2020

- Adiciona o contador de aquisição `CNT` ao pacote de rádio e à primeira coluna do log. Move a quebra de linha do log para depois da leitura do painel solar.
- Altera a pressão de referência de 1013,25 para 1016,00 hPa e os índices dos sensores de temperatura de 0–2 para 1–3.
- Reduz o limite de inicialização de seis para dois satélites e retira a condição de horário do laço diurno, que passa a depender da leitura de bateria acima de 9.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/790257b042e94e9e7f8840117133015b7536707f) · [Comparar com v1.1](https://github.com/mariopin80/HAB_OBDH/compare/8fdac7df3829db582f90146b6ce9593aaf6231e0...790257b042e94e9e7f8840117133015b7536707f)

## v1.3

Arquivo do acervo `HAB_OBDH_v1.3.ino` · 25/11/2020

- Restabelece a janela diurna das 11h às 22h UTC, fixa o limiar de bateria em 587 contagens do ADC e retira as mensagens de diagnóstico. A inicialização da serial permanece no código.
- Muda as esperas explícitas para cerca de 10 s no estado diurno e 30 s à noite e no estado de segurança. Volta aos índices de temperatura 0–2.
- A condição de inicialização exige simultaneamente menos de quatro e mais de 40 satélites, o que impede a execução do laço de espera pelo GPS. A v1.10 corrige a condição.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/e11a1a3628f3ffc0f866635a11f152da5137cc6c) · [Comparar com v1.2](https://github.com/mariopin80/HAB_OBDH/compare/790257b042e94e9e7f8840117133015b7536707f...e11a1a3628f3ffc0f866635a11f152da5137cc6c)

## v1.4

Arquivo do acervo `HAB_OBDH_v1.4.ino` · 02/12/2020

- Passa a transmitir durante o dia somente quando muda o minuto do GPS. A aquisição e a gravação no SD continuam a cada ciclo.
- Substitui o atraso de 9 s e a leitura de GPS de 1 s por um `smartdelay()` de 2 s no estado diurno.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/edecc396e65573dbd55f1abdc3e9693aa88e3d84) · [Comparar com v1.3](https://github.com/mariopin80/HAB_OBDH/compare/e11a1a3628f3ffc0f866635a11f152da5137cc6c...edecc396e65573dbd55f1abdc3e9693aa88e3d84)

## v1.5

Arquivo do acervo `HAB_OBDH_v1.5.ino`

- Desativa a inicialização da serial de diagnóstico, inicializa `min_ant` com o minuto atual ao entrar em operação diurna e o atualiza depois de transmitir. O primeiro pacote diurno fica para a próxima mudança de minuto.
- O nome do arquivo identifica v1.5. O cabeçalho herdado exibia v1.4 e 02/12/2020, portanto a data desta revisão é incerta.
- O rótulo do cabeçalho foi atualizado para v1.5 na reconstrução do histórico.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/0e5fe37a6fa64e156694b2c2d402c7199ab3ba32) · [Comparar com v1.4](https://github.com/mariopin80/HAB_OBDH/compare/edecc396e65573dbd55f1abdc3e9693aa88e3d84...0e5fe37a6fa64e156694b2c2d402c7199ab3ba32)

## v1.6

Arquivo do acervo `HAB_OBDH_v1.6.ino` · 02/12/2020

- Adiciona o envio em alta potência pelo E32 com `OPT_TP30`, restaura `OPT_TP21` depois do envio e usa potência menor nas demais mudanças de minuto.
- O cabeçalho descreve um intervalo de dez minutos para alta potência. O código compara o minuto atual com `min_ant10` e atribui `10 * Minute` a essa variável. A v1.7b passa a verificar o resto da divisão por dez.
- As esperas explícitas diurnas passam para 8 s de atraso e 2 s de leitura do GPS, com esperas adicionais na transmissão de alta potência.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/f82abd44189e83af0be5a049c3c92234a77a71f7) · [Comparar com v1.5](https://github.com/mariopin80/HAB_OBDH/compare/0e5fe37a6fa64e156694b2c2d402c7199ab3ba32...f82abd44189e83af0be5a049c3c92234a77a71f7)

## v1.7b

Arquivo do acervo `HAB_OBDH_v1.7b.ino` · 05/12/2020

- Coloca o E32 em power-down entre transmissões diurnas e o reativa para enviar. Seleciona alta potência nos minutos do GPS divisíveis por dez.
- Adiciona `LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF)` ao estado de bateria baixa e revê as esperas de aquisição durante dia e noite.
- Substitui a leitura da bateria pelo valor fixo 600, que fica acima do limiar de bateria baixa.
- O rótulo do cabeçalho foi atualizado de v1.7 para v1.7b, conforme o nome do arquivo.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/a19cf190022667bac8dae5c41e848ce7d4fab737) · [Comparar com v1.6](https://github.com/mariopin80/HAB_OBDH/compare/f82abd44189e83af0be5a049c3c92234a77a71f7...a19cf190022667bac8dae5c41e848ce7d4fab737)

## v1.8

Arquivo do acervo `HAB_OBDH_v1.8.ino` · 29/12/2020

- Troca o sono de oito segundos do microcontrolador no estado de segurança por um atraso de dez segundos.
- Divide a espera noturna em 9 s de atraso e 1 s de leitura do GPS.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/b208ff5aa7cb71266a5b0b9038a173e6a211b104) · [Comparar com v1.7b](https://github.com/mariopin80/HAB_OBDH/compare/a19cf190022667bac8dae5c41e848ce7d4fab737...b208ff5aa7cb71266a5b0b9038a173e6a211b104)

## v1.9

Arquivo do acervo `HAB_OBDH_v1.9.ino` · 13/01/2021

- Adiciona transmissão de contingência após mais de 90 s de `millis()` sem envio diurno. O cabeçalho chama essa função de watchdog de transmissão.
- Centraliza o limiar da bateria em `Vbat_min = 410`. Altera a leitura simulada de bateria de 600 para 500 contagens do ADC.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/9fd80293c604f9cbd707adde69aedffa64c5ecd2) · [Comparar com v1.8](https://github.com/mariopin80/HAB_OBDH/compare/b208ff5aa7cb71266a5b0b9038a173e6a211b104...9fd80293c604f9cbd707adde69aedffa64c5ecd2)

## v1.10

Arquivo do acervo `HAB_OBDH_v1.10.ino` · 14/01/2021

- Voou no **SAIRUS I**, conforme indicado também no cabeçalho.
- Restabelece a leitura real da bateria e corrige a espera inicial enquanto o número de satélites estiver fora da faixa de 4 a 40 e a bateria estiver acima do mínimo.
- Envia os vetores `Lo_P` e `Hi_P` pela UART do rádio e comanda os modos do E32 diretamente por M0/M1. Transmite na mudança do minuto do GPS ou pelo temporizador de 90 s, com alta potência nos minutos divisíveis por dez.
- Adiciona uma espera após inicializar os sensores de temperatura e mede temperatura enquanto aguarda o GPS.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/7a742c6ba36d9c11965c09aaa3ca6d9d38d40898) · [Comparar com v1.9](https://github.com/mariopin80/HAB_OBDH/compare/9fd80293c604f9cbd707adde69aedffa64c5ecd2...7a742c6ba36d9c11965c09aaa3ca6d9d38d40898)

## v2.0.0

Arquivo do acervo `HAB_OBDH_v2.0.0.ino` · 20/10/2021

- Inicia a reorganização do firmware com constantes nomeadas para os pinos, novas denominações de UART e estados `INICIALIZACAO`, `NIVEL_1`, `NIVEL_2` e `SEGURANCA`.
- Deixa os corpos dos estados vazios. O setup inicializa os periféricos e abre o log, com rotinas de aquisição e transmissão fora do laço principal.
- Substitui `flush()` por `sync()` na rotina de gravação do SD. O cabeçalho de bibliotecas registra SdFat 2.1.0.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/8e7a8a79ae8d5e4d0ccdcec490f83d95966189da) · [Comparar com v1.10](https://github.com/mariopin80/HAB_OBDH/compare/7a742c6ba36d9c11965c09aaa3ca6d9d38d40898...8e7a8a79ae8d5e4d0ccdcec490f83d95966189da)

## v2.0.1

Arquivo do acervo `HAB_OBDH_v2.0.1.ino` · 09/06/2022

- Restabelece os estados de inicialização, dia (`OPER_1`), noite (`NOTURNO`) e bateria baixa. Na inicialização, aguarda entre 5 e 40 satélites enquanto a bateria estiver acima do mínimo.
- Retoma a gravação no SD de dia e de noite, a transmissão diurna na mudança do minuto ou pelo temporizador de 90 s e o retorno à inicialização após recuperação da bateria.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/7a35ba0b62557fcd61e15b8ddf7f6e61abcfe8cd) · [Comparar com v2.0.0](https://github.com/mariopin80/HAB_OBDH/compare/8e7a8a79ae8d5e4d0ccdcec490f83d95966189da...7a35ba0b62557fcd61e15b8ddf7f6e61abcfe8cd)

## v2.1

Arquivo do acervo `HAB_OBDH_v2.1.ino` · 28/09/2022

- Atualiza a versão e a data no cabeçalho. O corpo do sketch é idêntico byte a byte ao da v2.0.1.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/8e9e0db21aba89e8ef49968ad22aedd8fe8d4e76) · [Comparar com v2.0.1](https://github.com/mariopin80/HAB_OBDH/compare/7a35ba0b62557fcd61e15b8ddf7f6e61abcfe8cd...8e9e0db21aba89e8ef49968ad22aedd8fe8d4e76)

## v2.2

Arquivo do acervo `HAB_OBDH_v2.2.ino` · 07/02/2023

- Eleva de cinco para seis o mínimo de satélites na inicialização, ajusta a pressão de referência para 1013,00 hPa e passa a usar `smartdelay()` de 3 s no estado diurno.
- Move as sequências de transmissão para `LoRa_HiPWR()` e `LoRa_LoPWR()`. Os ramos diurnos contêm declarações locais de função (`void LoRa_HiPWR();` e `void LoRa_LoPWR();`), por isso esses ramos não transmitem nesta versão.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/1b36b0463ce9392052423eb3288d989efed832a9) · [Comparar com v2.1](https://github.com/mariopin80/HAB_OBDH/compare/8e9e0db21aba89e8ef49968ad22aedd8fe8d4e76...1b36b0463ce9392052423eb3288d989efed832a9)

## v2.3

Arquivo do acervo `HAB_OBDH_v2.3_debug.ino` · 07/02/2023

- Sketch de depuração. Usa um laço único de aquisição, com leitura do GPS por 5 s, gravação em SD e envio em baixa potência a cada iteração. Imprime mensagens de inicialização e telemetria a 115200 baud.
- Seleciona referência externa do ADC e altera `Speed` de `unsigned short` para `float`, mudando o formato binário da telemetria.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/bf2ed847e023828646e47c928154ab5944d493b7) · [Comparar com v2.2](https://github.com/mariopin80/HAB_OBDH/compare/1b36b0463ce9392052423eb3288d989efed832a9...bf2ed847e023828646e47c928154ab5944d493b7)

## v2.4

Arquivo do acervo `HAB_OBDH_v2.4_teste.ino` · 24/02/2023

- Sketch de teste. Retira o diagnóstico serial e reduz a janela de leitura do GPS para 3 s. Mantém a gravação no SD a cada iteração.
- Chama as rotinas de rádio na mudança do minuto do GPS ou depois de mais de 90 s. Seleciona alta potência nos minutos divisíveis por dez.
- Reduz `CNT` de `unsigned long` para `unsigned short` e passa a incrementá-lo nos envios, alterando o campo do pacote e o significado do contador.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/850597096a8d3f3490ce7a7002c6911a342c54de) · [Comparar com v2.3](https://github.com/mariopin80/HAB_OBDH/compare/bf2ed847e023828646e47c928154ab5944d493b7...850597096a8d3f3490ce7a7002c6911a342c54de)

## v2.5

Arquivo do acervo `HAB_OBDH_v2.5_voo.ino` · 28/02/2023

- Retoma os estados de inicialização, dia, noite e segurança, com mínimo de quatro satélites na partida. `CNT` volta a contar ciclos de aquisição.
- Inverte as entradas analógicas para bateria em A1 e painel solar em A0. Reduz `Vbat_min` de 410 para 390 contagens do ADC, valor que o sketch associa a 5,3 V no hardware do projeto.
- Usa atrasos de 90 s à noite e no modo de segurança. O laço diurno termina às 21h UTC e o noturno começa às 22h UTC, deixando a faixa das 21h fora dos dois laços.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/d6807bc32e9cef05e09e653e3cceb8d355e547c7) · [Comparar com v2.4](https://github.com/mariopin80/HAB_OBDH/compare/850597096a8d3f3490ce7a7002c6911a342c54de...d6807bc32e9cef05e09e653e3cceb8d355e547c7)

## v2.6

Arquivo do acervo `HAB_OBDH_v2.6_voo_LP.ino` · 07/03/2023

- Adiciona power-down do microcontrolador com a biblioteca `LowPower`. Solicita sono de 4 s na inicialização e no estado diurno, e de 8 s à noite e em segurança. O ADC e o BOD ficam desligados durante o sono.
- Usa `smartdelay()` de 2 s depois de acordar na inicialização, de dia e à noite. Identifica no código a referência externa de 3,3 V do ADC.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/b7c9b0057f889c49624b2e580259c1320c9e6953) · [Comparar com v2.5](https://github.com/mariopin80/HAB_OBDH/compare/d6807bc32e9cef05e09e653e3cceb8d355e547c7...b7c9b0057f889c49624b2e580259c1320c9e6953)

## v2.7

Arquivo do acervo `HAB_OBDH_v2.7_voo_LP.ino` · 22/03/2023

- Voou no **SAIRUS II**.
- Reduz de 8 para 4 s o sono à noite e em segurança. `smartdelay()` passa a conferir o tempo limite em cada iteração e a ler um byte do GPS por vez.
- Atualiza o comentário do estado diurno para 21h UTC. A condição noturna começa às 22h UTC, mantendo a lacuna da faixa das 21h introduzida na v2.5.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/1b00c6f6afe8e7a94091015957b8f7ea6290e0d5) · [Comparar com v2.6](https://github.com/mariopin80/HAB_OBDH/compare/b7c9b0057f889c49624b2e580259c1320c9e6953...1b00c6f6afe8e7a94091015957b8f7ea6290e0d5)

## v3.0

Arquivo do acervo `HAB_OBDH_v3.0.ino` · 09/04/2026

**Evolução experimental sem voo.**

- Adiciona watchdog de hardware AVR com tempo limite de 8 s e reativa o watchdog após o power-down. O programa o realimenta após as rotinas de temperatura, barômetro, GPS, alimentação e SD sinalizarem conclusão.
- Verifica o watchdog durante as tentativas de obter GPS e no modo de bateria baixa. Altera o início da noite para 21h UTC e retorna diretamente ao modo diurno após a recuperação da bateria.
- Altera os bytes de configuração do E32 nos dois perfis de potência. O cabeçalho lista descida, pouso, recuperação na partida sem GPS e beacon em segurança como trabalho previsto.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/b89dd4d77374e5eff7de8fc7b022d6f1360bafe5) · [Comparar com v2.7](https://github.com/mariopin80/HAB_OBDH/compare/1b00c6f6afe8e7a94091015957b8f7ea6290e0d5...b89dd4d77374e5eff7de8fc7b022d6f1360bafe5)

## v3.1

Arquivo do acervo `HAB_OBDH_v3.1.ino` · 15/04/2026

**Evolução experimental sem voo.**

- Introduz os estados `STARTUP`, `CLIMB`, `DESCENT`, `LAND`, `NIGHT` e `SAFE`. Descida e pouso ficam definidos como esboços nesta versão.
- Centraliza a aquisição e as flags do watchdog em `ALL_Data_aquisition()`, usa campos de telemetria de largura fixa e adiciona tratamento de valores inválidos do GPS, filtros de faixa e salto de altitude e registro do pico. `Speed` passa a 16 bits e recebe `gps.speed() / 100`.
- Transmite em baixa potência na inicialização enquanto aguarda pelo menos quatro satélites. Adiciona limiar de recuperação da bateria de 401 contagens do ADC e beacon de baixa potência a cada 150 ciclos de sono em `SAFE`.
- O cabeçalho registra que esta versão não foi testada em hardware.
- O rótulo do cabeçalho foi atualizado de v3.1b para v3.1, conforme o nome do arquivo.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/0cfe1428061af5b855eebd36e22013be7cafd06c) · [Comparar com v3.0](https://github.com/mariopin80/HAB_OBDH/compare/b89dd4d77374e5eff7de8fc7b022d6f1360bafe5...0cfe1428061af5b855eebd36e22013be7cafd06c)

## v4.0

Arquivo do acervo `HAB_OBDH_v4.0.ino` · 15/04/2026

**Evolução experimental sem voo.**

- Implementa `CLIMB` → `DESCENT` quando a altitude máxima registrada supera 1.000 m e a altitude atual cai mais de 500 m em relação ao pico. A descida grava no SD e transmite em alta potência na mudança de minuto ou após 30 s de `millis()`.
- Implementa `DESCENT` → `LAND` após dez amostras que atendam ao critério de altitude abaixo de 3.000 m e variação inferior a 5 m em relação à referência. Em `LAND`, transmite em alta potência a cada 30 ciclos de aquisição e solicita 8 s de sono por ciclo.
- Altera a saída de `NIGHT` para `LAND`, independentemente do estado anterior à noite. O cabeçalho indica que a v4.0 não foi testada em hardware.

[Código da versão](https://github.com/mariopin80/HAB_OBDH/tree/fdf6d0913026c8439b11dce2a32f053d733edda6) · [Comparar com v3.1](https://github.com/mariopin80/HAB_OBDH/compare/0cfe1428061af5b855eebd36e22013be7cafd06c...fdf6d0913026c8439b11dce2a32f053d733edda6)
