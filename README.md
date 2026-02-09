# Algoritmos (OSPF & LACP)

## Contexto
Desenvolvido como parte de **Algotítmos e Estrtura de Dados II (CES-11)** no **Instituto Tecnológico de Aeronáutica (ITA)**.

## Objetivo
O objetivo deste projeto foi simular uma topologia de rede robusta de um provedor de serviços de Internet (ISP). O sistema calcula os caminhos de roteamento mais eficientes com base nos custos de largura de banda (imitando o protocolo **OSPF**) e gerencia a agregação de links físicos (**LACP**) para aumentar a taxa de transferência.

## Destaques Técnicos
* **Linguagem:** C (Focada em gerenciamento de memória de baixo nível e otimização de estruturas).
* **Algoritmos de Grafos:** Implementada a lógica do **Algoritmo de Dijkstra** para encontrar o "Caminho Mais Curto" (menor custo) entre roteadores com base na métrica $Custo = LarguraDeBandaDeReferência / LarguraDeBandaDoLink$[cite: 272].
* **Agregação de Links:** Simulação do **LACP** (Protocolo de Controle de Agregação de Links) para somar dinamicamente as larguras de banda de links físicos paralelos em uma única conexão lógica[cite: 279].
* **Balanceamento de Carga:** O mecanismo lida com cenários de "Múltiplos Caminhos de Custo Igual", distribuindo o tráfego por várias rotas quando os custos são idênticos[cite: 277].

## Principais Recursos
* [cite_start]**Topologia Dinâmica:** Suporta a adição/remoção de roteadores e links em tempo de execução[cite: 293, 297].
* [cite_start]**Rastreamento de Rotas:** O comando `TRACE` exibe o(s) caminho(s) ideal(is) entre quaisquer dois nós na rede[cite: 291].
* [cite_start]**Tratamento de Erros:** Valida os padrões Ethernet (10/100/1000 Mbps) e aplica as regras de consistência LACP[cite: 287, 289].

