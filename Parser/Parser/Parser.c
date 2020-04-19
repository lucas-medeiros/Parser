#include <stdio.h>
#include <string.h>
#include <stdbool.h>
/*
* Este programa, baseado no trabalho de Ryan Flannery (2009)
* dispon�vel em: http://www.ryanflannery.net/teaching/common/recursive-descent-parsing/
* implementa um parser recursive descent. Para express�es da l�gica propsicional
* escritas em latex (Nega��o, E, Ou, Condicional e Bicondicional).
*
* Utiliza a seguinte gram�tica escrita em BNF:
*   Formula = Constante | Proposicao | FormulaUnaria | FormulaBinaria.
*   Constante = "T" | "F".
*   Proposicao = [a-z0-9]+.
*   FormulaUnaria = AbreParen OperadorUnario Formula FechaParen.
*   FormulaBinaria= AbreParen OperatorBinario Formula Formula FechaParen.
*   AbreParen = "(".
*   FechaParen = ")".
*   OperatorUnario = "\neg".
*   OperatorBinario = "\vee" | "\wedge" | "\rightarrow" | "\leftrightarrow".
*/

/*
* Prot�tipos: um prot�tipo para cada das fun��es definidas no BNF
*
* Observe que todas as fun��es s�o do tipo bool, ou retornam verdadeiro ou falso.
*
* Todas as fun��es usar�o como indicador de posi��o o pr�prio ponteiro da string
* do seu argumento. Incrementando este ponteiro para percorrer a string de entrada.
*
*/
bool Formula(char* s[]);            //Formula = Constante | Proposicao | FormulaUnaria | FormulaBinaria.
bool Constante(char* s[]);          //Constante ::= "T" | "F".
bool Proposicao(char* s[]);         //Proposicao ::= [a-z0-9]+.
bool FormulaUnaria(char* s[]);      //FormulaUnaria ::= AbreParen OperadorUnario Formula FechaParen.
bool FormulaBinaria(char* s[]);     //FormulaBinaria :: = AbreParen OperatorBinario Formula Formula FechaParen.
bool AbreParen(char* s[]);          //AbreParen ::= "(".
bool FechaParen(char* s[]);         //FechaParen :: = ")".
bool OperadorUnario(char* s[]);     //OperatorUnario :: = "\neg".
bool OperadorBinario(char* s[]);    //OperatorBinario ::= "\vee" | "\wedge" | "\rightarrow" | "\leftrightarrow".
//void ImprimeTokens(token Tokens[], int size);

//fun��es de apoio
void RemoverEspacosEmBranco(char* s[]);
bool FormulaWrapper(char* s[]);

//definindo alguns strings como constantes para n�o precisar digitar repetidamente
//estes s�o os comandos utilizados no padr�o latex que usaremos para digitar nossas proposi��es
const char* STRING_NOT = "\\neg";
const char* STRING_OR = "\\vee";
const char* STRING_AND = "\\wedge";
const char* STRING_IMPLIES = "\\rightarrow";
const char* STRING_IFF = "\\leftrightarrow";
const char* CLASS_OPERADOR = "operador";
const char* CLASS_OPERANDO = "operando";
const char* TYPE_OU = "operador un�rio";
const char* TYPE_OB = "operador bin�rio";
const char* TYPE_PROP = "proposi��o";
const char* TYPE_CONST = "constante";
const char* CONST_T = "true";
const char* CONST_F = "false";

//comprimento m�ximo das proposi��es e das linhas
#define COMPRIMENTO_PROPOSICAO 100
#define COMPRIMENTO_MAXIMO_DA_LINHA 1000
#define NUM_MAX_TOKENS 50

typedef struct {
    char* classe;
    char* tipo;
    char* valor;
} token;

//Programa de Testes
int main(int argc, char* argv[]) {

    token Tokens[6];
    int i = 0;
    Tokens[i].classe = CLASS_OPERADOR;
    Tokens[i].tipo = TYPE_OB;
    Tokens[i].valor = STRING_NOT;

    i++;
    Tokens[i].classe = CLASS_OPERADOR;
    Tokens[i].tipo = TYPE_OU;
    Tokens[i].valor = STRING_AND;
    
    i++;
    Tokens[i].classe = CLASS_OPERADOR;
    Tokens[i].tipo = TYPE_OB;
    Tokens[i].valor = STRING_OR;
    
    i++;
    Tokens[i].classe = CLASS_OPERANDO;
    Tokens[i].tipo = TYPE_PROP;
    Tokens[i].valor = "lucas";

    i++;
    Tokens[i].classe = CLASS_OPERANDO;
    Tokens[i].tipo = TYPE_CONST;
    Tokens[i].valor = CONST_T;

    i++;
    Tokens[i].classe = CLASS_OPERANDO;
    Tokens[i].tipo = TYPE_PROP;
    Tokens[i].valor = "2020";


    //ImprimeTokens(Tokens, i);
    printf("Tabela de tokens: \nclasse\t\ttipo\t\tvalor\n\n");
    for (int j = 0; j > i; j++) {
        printf("%s\t\t%s\t\t%s\n", Tokens[j].classe, Tokens[j].tipo, Tokens[j].valor);
    }
    printf("\n\n");

    //array para guardar a f�rmula digitada
    char input[COMPRIMENTO_MAXIMO_DA_LINHA];
    //ponteiro para guardar uma c�pia da entrada
    char* copy;

    //mensagens para o usu�rio
    printf("Digite as formulas, uma por linha com no maximo %d caracteres: \n", COMPRIMENTO_MAXIMO_DA_LINHA);
    printf("Digite CTRL+D to sair!!!\n");

    /*
    * vamos rodar o la�o principal utilizando o retorno do fgets como controle
    * o la�o ir� rodar enquanto o fgets devolver algo diferente de NULL
    *
    * O fgets: fgets(input, COMPRIMENTO_MAXIMO_DA_LINHA, stdin)
    * ir� ler COMPRIMENTO_MAXIMO_DA_LINHA carcteres do dispositivo padr�o de entrada (stdin)
    * no nosso caso, do teclado e ir� escrever este string no array input que definimos anteriormente
    * o fgets retorna o ponteiro do pr�prio array input quanto tudo est� correto ou um ponteiro NULL
    */
    while (fgets(input, COMPRIMENTO_MAXIMO_DA_LINHA, stdin) != NULL) {
        //vamos manter uma c�pia porque todas as fun��es operam com os ponteiros
        //da string digitada.
        copy = input;

        //aqui faremos a avalia��o da proposi��o digitada.
        if (FormulaWrapper(&copy))
            printf("===> Formula corretamente formatada. :)\n");
        else
            printf("===> Formula mal formatada. :(\n");
    }
    return 0;
}//fim do main

// �nica fun��o chamada para verficar nossas f�rmulas
bool FormulaWrapper(char* s[]) {
    char* original = *s; //o valor apontado por

    if (Formula(s)) {
        RemoverEspacosEmBranco(s);
        if (*s[0] == '\0') //strings em C terminam com o caractere '\0'
            return true;
        else {
            *s = original;
            return false;
        }
    }
    return false;
}



//fun��es de apoio
//devolve a string de entrada sem qualquer espa�o em branco
//e faz isso, simplesmente incrementando o ponteiro da string de entrada
void RemoverEspacosEmBranco(char* s[]) {
    //testa todos as formas poss�veis de se ter um espa�o em branco na tela
    //e considera a quebra de linha e a tabula��o como espa�os em branco
    while (*s[0] == ' ' || *s[0] == '\n' || *s[0] == '\t')
        *s = *s + 1;
}

// Compara a string de entrada contra uma string conhecida, semente
// Se ouver coincid�ncia adiantamos a string de entrada no comprimento da semente e devolvemos TRUE
//
bool match(char* s[], const char* semente) {
    // a compara��o ser� feita com o retorno de strncmp(*s, semente, strlen(semente))
    // strncmp  compara duas strings, no caso s e semente at� o comprimento de semente (strlen(semente)
    // e devolve um inteiro <0 ou >0 caso inexista igualdade ou zero se existir coincid�ncia
    int comp = strlen(semente);
    if (strncmp(*s, semente, comp) == 0) {
        //sempre que que semente existir fazemos a aritm�tica de ponteiros com o comprimento de strlen
        *s = *s + comp;
        //encerramos a fun��o retornando TRUE
        return true;
    }
    return false;
}

/*
void ImprimeTokens(token Tokens[], int size) {
    printf("Tabela de tokens: \nclasse\t\ttipo\t\tvalor\n\n");
    for (int i = 0; i < size; i++) {
        printf("%s\t\t%s\t\t%s\n", Tokens[i].classe, Tokens[i].tipo, Tokens[i].valor);
    }
} */

/*****************************************************************************
* Todas as fun��es principais se comportam da mesma forma.
* A fun��o "bool Expressions(char *s[])" ir�
*
*   1. comparar o string de entrada "s" contra express�o determinada na BNF.
*
*   2. Se (if) o string de entrada coincide com a espress�o, ent�o (then)
*      vamos adiantar o ponteiro da express�o de entrada, e
*      retornar ture.
*
*   3. Caso contr�rio, n�o alteramos o ponteiro do string de entrada e retornamos falso.
*
****************************************************************************/

// Para identificar a regra: Formula ::= Constante | Proposicao | FormulaUnaria | FormulaBinaria
// Neste caso uma regra de escolha Formula pode representar uma Constante OU Proposicao OU...
bool Formula(char* s[]) {
    //uma c�pia loca do string de entrada
    char* original = *s;
    //removendo os espa�os em branco
    RemoverEspacosEmBranco(s);
    //agora vamos chamar cada uma das fun��es das regras inferiores para ver se uma delas � verdadeira
    //e encerra fun��o retornando true se encontrar uma delas
    if (Constante(s) || Proposicao(s) || FormulaUnaria(s) || FormulaBinaria(s)) return true;
    *s = original;
    return false;
}

// Identifica uma constante segundo a regra: Constante ::= "T" | "F"
// Esta � uma opera��o de escolha (Ou) entre dois s�mbolos "T" ou "F"
// cabe a n�s interpretar como True, verdadeiro ou False, falso.
bool Constante(char* s[]) {
    char* original = *s;
    //s� por seguran�a removemos os espa�os em todas as fun��es
    RemoverEspacosEmBranco(s);
    //fazemos a verifica��o e interrompemos a fun��o
    if (match(s, "T") || match(s, "F")) return true;
    //n�o ouve coincid�ncia ent�o devolvemos o string original
    *s = original;
    return false;
}

// Identifica uma proposi��o segundo a regra Proposicao ::= [a-z0-9]+
// Neste caso, teremos que usar uma express�o regular [a-z0-9]+
// Mas � um padr�o t�o simples que pode ser colocado diretamente no sscanf
// A fun��o sscanf_s l� um string, remove caracteres desta string de acordo
// com a regra e devolve a string removida na string inicial
bool Proposicao(char* s[]) {
    //primeiro copiamos para, se necess�rio devolver string sem modifica��es
    char* original = *s;
    //removemos espa�os em branco, s� por seguran�a novamente.
    RemoverEspacosEmBranco(s);

    // usaremos este array de char e este int para controlar o processo do sscanf
    char prop[COMPRIMENTO_PROPOSICAO];
    int  numeroCaracteresLidos;

    //vamos usar o retor no sscanf_s neste caso, com dois argumentos
    // %[a-z0-9] significando qualquer letra min�scula ou n�mero que ser� devolvido em prop
    // %n que conter� a quantidade de caracteres que ser� devolvido em numeroCaracteresLidos
    // refer�ncia do sscanf em http://pubs.opengroup.org/onlinepubs/009695399/functions/scanf.html
    // se for ler strings lembre-se que � necess�rio dimensionar o buffer para cada string
    if (sscanf_s(*s, "%[a-z0-9]%n", &prop, sizeof(prop), &numeroCaracteresLidos) == 1) {
        *s = *s + numeroCaracteresLidos;
        return true;
    }
    //n�o achou nenhuma proposi��o devolve o ponteiro original
    *s = original;
    return false;
}

//identificando o operador un�rio segundo a regra://OperatorUnario :: = "\neg"
//lembre-se que o \neg" est� definido em STRING_NOT
bool OperadorUnario(char* s[]) {
    char* original = *s;
    RemoverEspacosEmBranco(s);

    if (match(s, STRING_NOT))
        return true;

    *s = original;
    return false;
}

//identificamos o abrir e o fechar par�nteses e os operadores
//bin�rios da mesma forma
bool AbreParen(char* s[]) {
    char* original = *s;
    RemoverEspacosEmBranco(s);
    if (match(s, "("))
        return true;

    *s = original;

    return false;
}

bool FechaParen(char* s[]) {
    char* original = *s;
    RemoverEspacosEmBranco(s);

    if (match(s, ")"))
        return true;

    *s = original;
    return false;
}

bool OperadorBinario(char* s[]) {
    char* original = *s;
    RemoverEspacosEmBranco(s);

    if (match(s, STRING_OR) || match(s, STRING_AND)
        || match(s, STRING_IMPLIES) || match(s, STRING_IFF))
        return true;

    *s = original;
    return false;
}

// identificando a FormulaUnaria segundo a regra: FormulaUnaria ::= AbreParen OperadorUnario Formula FechaParen
// observe que cada fun��o chamada adianta a posi��o do ponteiro do string de entrada
// observe tamb�m que basta que um elemento seja falso para que a formula n�o seja reconhecida
bool FormulaUnaria(char* s[]) {
    char* original = *s;
    RemoverEspacosEmBranco(s);

    if (!AbreParen(s)) {
        *s = original;
        return false;
    }

    if (!OperadorUnario(s)) {
        *s = original;
        return false;
    }

    if (!Formula(s)) {
        *s = original;
        return false;
    }

    if (!FechaParen(s)) {
        *s = original;
        return false;
    }

    return true;
}

//identificando a FormulaBinaria segundo a regra: FormulaBinaria :: = AbreParen OperatorBinario Formula Formula FechaParen
bool FormulaBinaria(char* s[]) {
    char* original = *s;
    RemoverEspacosEmBranco(s);

    if (!AbreParen(s)) {
        *s = original;
        return false;
    }

    if (!OperadorBinario(s)) {
        *s = original;
        return false;
    }

    if (!Formula(s)) {
        *s = original;
        return false;
    }

    if (!Formula(s)) {
        *s = original;
        return false;
    }

    if (!FechaParen(s)) {
        *s = original;
        return false;
    }
    return true;
}