# Parser
    Programa alterado por Lucas Cardoso de Medeiros
    Para o exercício 8 da Atividade Somativa da disciplina de Linguagens Formais e Compiladores
    Engenharia de Computação - PUCPR 2020

    Instruções fornecidas:
    8. A função do analisador léxico é encontrar os lexemas de uma linguagem definida em relação a um determinado alfabeto Σ e classificar este lexema em tokens para uso do analisador léxico. Durante a disciplina nós fizemos um parser Recursive Descent para uma linguagem desenhada especificamente para as operações da lógica proposicional. Este parser apenas validava as expressões digitadas. Sua tarefa será modificar este parser para que, além de validar ele gere os tokens utilizados em cada expressão. Para isso você deverá utilizar as seguintes classes: operador, operando, tipo e valor. Os lexemas ou serão da classe operador ou da classe operando. Todos terão tipo de acordo com a gramática utilizada e todos terão valor, quando o valor não for relevante para um determinado token use a expressão “null” sem as aspas.

    A gramática utilizada para este exercício é:

        · 𝐹𝑜𝑟𝑚𝑢𝑙𝑎 = 𝐶𝑜𝑛𝑠𝑡𝑎𝑛𝑡𝑒 | 𝑃𝑟𝑜𝑝𝑜𝑠𝑖𝑐𝑎𝑜 | 𝐹𝑜𝑟𝑚𝑢𝑙𝑎𝑈𝑛𝑎𝑟𝑖𝑎 | 𝐹𝑜𝑟𝑚𝑢𝑙𝑎𝐵𝑖𝑛𝑎𝑟𝑖𝑎.

        · 𝐶𝑜𝑛𝑠𝑡𝑎𝑛𝑡𝑒 = "𝑇" | "𝐹“.

        · 𝑃𝑟𝑜𝑝𝑜𝑠𝑖𝑐𝑎𝑜 = [𝑎 − 𝑧0 − 9]+.

        · 𝐹𝑜𝑟𝑚𝑢𝑙𝑎𝑈𝑛𝑎𝑟𝑖𝑎 = 𝐴𝑏𝑟𝑒𝑃𝑎𝑟𝑒𝑛 𝑂𝑝𝑒𝑟𝑎𝑑𝑜𝑟𝑈𝑛𝑎𝑟𝑖𝑜 𝐹𝑜𝑟𝑚𝑢𝑙𝑎 𝐹𝑒𝑐h𝑎𝑃𝑎𝑟𝑒𝑛.

        · 𝐹𝑜𝑟𝑚𝑢𝑙𝑎𝐵𝑖𝑛𝑎𝑟𝑖𝑎 = 𝐴𝑏𝑟𝑒𝑃𝑎𝑟𝑒𝑛 𝑂𝑝𝑒𝑟𝑎𝑡𝑜𝑟𝐵𝑖𝑛𝑎𝑟𝑖𝑜 𝐹𝑜𝑟𝑚𝑢𝑙𝑎 𝐹𝑜𝑟𝑚𝑢𝑙𝑎 𝐹𝑒𝑐h𝑎𝑃𝑎𝑟𝑒𝑛.

        · 𝐴𝑏𝑟𝑒𝑃𝑎𝑟𝑒𝑛 = "(“.

        · 𝐹𝑒𝑐h𝑎𝑃𝑎𝑟𝑒𝑛 = ")“.

        · 𝑂𝑝𝑒𝑟𝑎𝑡𝑜𝑟𝑈𝑛𝑎𝑟𝑖𝑜 = "\𝑛𝑒𝑔“.

        · 𝑂𝑝𝑒𝑟𝑎𝑡𝑜𝑟𝐵𝑖𝑛𝑎𝑟𝑖𝑜 = "\𝑣𝑒𝑒“ | "\𝑤𝑒𝑑𝑔𝑒" | "\𝑟𝑖𝑔h𝑡𝑎𝑟𝑟𝑜𝑤" | "\𝑙𝑒𝑓𝑡𝑟𝑖𝑔h𝑡𝑎𝑟𝑟𝑜𝑤“. 