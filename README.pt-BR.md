# HAB OBDH — SAIRUS

[Read in English](README.md)

Software de voo Arduino do computador de bordo (OBC/OBDH) do balão de alta altitude desenvolvido no **IFSP, Campus Boituva**, pelo Prof. Mario Pin. Adquire dados de GPS, temperatura, barometria e alimentação, grava no cartão SD e transmite telemetria binária pelo rádio Ebyte E32.

## Versões que voaram

| Missão | Firmware |
| --- | --- |
| SAIRUS I | [v1.10](https://github.com/mariopin80/HAB_OBDH/releases/tag/v1.10) |
| SAIRUS II | [v2.7](https://github.com/mariopin80/HAB_OBDH/releases/tag/v2.7) |

As versões v3.0, v3.1 e v4.0 são evoluções experimentais posteriores aos voos. Os cabeçalhos da v3.1 e da v4.0 registram que essas versões não foram testadas em hardware.

## Histórico de versões

Os 23 sketches recuperados do acervo estão disponíveis em [Releases](https://github.com/mariopin80/HAB_OBDH/releases). Cada entrada do [histórico de alterações](CHANGELOG.pt-BR.md) identifica o arquivo original, aponta para o código e traz a comparação com a versão anterior. O histórico Git foi reconstruído a partir dos arquivos preservados. As datas do histórico vêm dos cabeçalhos dos sketches.

A identificação das versões segue os nomes dos arquivos do acervo. O primeiro sketch ganhou um cabeçalho com a versão e os rótulos de v1.5, v1.7b e v3.1 foram corrigidos. O restante de cada sketch foi conferido byte a byte com o acervo.

## Abrir um sketch

Baixe **Source code (zip)** no release desejado, extraia o arquivo e abra `HAB_OBDH/HAB_OBDH.ino` na IDE Arduino. Selecione a placa e o core Arduino correspondentes ao hardware do OBDH. O sketch em `main` é a versão de desenvolvimento v4.0.

Os sketches usam `SoftwareSerial`, `TinyGPS`, a biblioteca do E32 com `EBYTE.h`, `OneWire`, `DallasTemperature`, `Adafruit_BMP085` e `SdFat`. Instale `LowPower` para v1.7b, v1.8, v1.9 e da v2.6 em diante. Da v3.0 em diante, o código usa também `<avr/wdt.h>` do core AVR. O código da v2.0.0 à v2.1 indica SdFat 2.1.0. Para o rádio, use a biblioteca que oferece a classe `EBYTE` e o método `SendStruct()` usados no sketch.

## Conexões nas versões de voo

As direções dos pinos são relativas ao OBDH. As duas seriais são inicializadas a 9600 baud.

| Sinal | v1.10 | v2.7 |
| --- | --- | --- |
| RX / TX serial do E32 | D7 / D6 | D7 / D6 |
| M0 / M1 do E32 | D4 / D5 | D4 / D5 |
| RX / TX serial do GPS | D8 / D9 | D8 / D9 |
| Dados dos sensores OneWire | D3 | D3 |
| Chip select SPI do SD | D10 | D10 |
| Medição da bateria | A0 | A1 |
| Medição do painel solar | A1 | A0 |

A v2.7 seleciona `analogReference(EXTERNAL)` e indica referência externa de 3,3 V no hardware daquela versão. Os valores analógicos são armazenados em contagens do ADC. O registro em SD é gravado em `teste3.txt`. O rádio transmite a estrutura `DATA` em formato binário bruto. Use no receptor a disposição de campos da versão correspondente do firmware.
