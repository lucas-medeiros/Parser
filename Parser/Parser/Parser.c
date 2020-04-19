#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h> 

/*
* Este programa, baseado no trabalho de Ryan Flannery (2009)
* disponível em: http://www.ryanflannery.net/teaching/common/recursive-descent-parsing/
* implementa um parser recursive descent. Para expressões da lógica propsicional
* escritas em latex (Negação, E, Ou, Condicional e Bicondicional).
*
* Utiliza a seguinte gramática escrita em BNF:
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
* Protótipos: um protótipo para cada das funções definidas no BNF
*
* Observe que todas as funções são do tipo bool, ou retornam verdadeiro ou falso.
*
* Todas as funções usarão como indicador de posição o próprio ponteiro da string
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

//funções de apoio
void RemoverEspacosEmBranco(char* s[]);
bool FormulaWrapper(char* s[]);

//definindo alguns strings como constantes para não precisar digitar repetidamente
//estes são os comandos utilizados no padrão latex que usaremos para digitar nossas proposições
const char* STRING_NOT = "\\neg";
const char* STRING_OR = "\\vee";
const char* STRING_AND = "\\wedge";
const char* STRING_IMPLIES = "\\rightarrow";
const char* STRING_IFF = "\\leftrightarrow";
const char* STRING_NULL = "null";
const char* CLASS_OPERADOR = "operador";
const char* CLASS_OPERANDO = "operando";
const char* TYPE_OU = "operador unario";
const char* TYPE_OB = "operador binario";
const char* TYPE_PROP = "proposicao";
const char* TYPE_CONST = "constante";
const char* CONST_T = "T";
const char* CONST_F = "F";

//comprimento máximo das proposições e das linhas
#define COMPRIMENTO_PROPOSICAO 50
#define COMPRIMENTO_MAXIMO_DA_LINHA 500
#define NUM_MAX_TOKENS 50

typedef struct {
    char* classe;
    char* tipo;
    char* valor;
} token;

int pos;
token Tokens[NUM_MAX_TOKENS];
bool imprimiu;

void LimpaTokens() {
    for (int i = 0; i < NUM_MAX_TOKENS; i++) {
        Tokens[i].classe = "";
        Tokens[i].tipo = "";
        Tokens[i].valor = "";
    }
    pos = 0;
    imprimiu = false;
}

void ImprimeTokens() {
    printf("\nTabela de tokens: \nclasse\t\ttipo\t\t\tvalor\n\n");
    for (int i = 0; i < NUM_MAX_TOKENS; i++) {
        if (Tokens[i].classe != ""){
            if(strcmp(Tokens[i].tipo, TYPE_OB) == 0)
                printf("%s\t%s\t%s\n", Tokens[i].classe, Tokens[i].tipo, Tokens[i].valor);
            else
                printf("%s\t%s\t\t%s\n", Tokens[i].classe, Tokens[i].tipo, Tokens[i].valor);
        }
    }
    printf("\n\n");
}

//Programa de Testes
int main(int argc, char* argv[]) {

    LimpaTokens();

    //array para guardar a fórmula digitada
    char input[COMPRIMENTO_MAXIMO_DA_LINHA];
    //ponteiro para guardar uma cópia da entrada
    char* copy;

    //mensagens para o usuário
    printf("Digite as formulas, uma por linha com no maximo %d caracteres: \n", COMPRIMENTO_MAXIMO_DA_LINHA);
    printf("Digite CTRL+D to sair!!!\n");

    /*
    * vamos rodar o laço principal utilizando o retorno do fgets como controle
    * o laço irá rodar enquanto o fgets devolver algo diferente de NULL
    *
    * O fgets: fgets(input, COMPRIMENTO_MAXIMO_DA_LINHA, stdin)
    * irá ler COMPRIMENTO_MAXIMO_DA_LINHA carcteres do dispositivo padrão de entrada (stdin)
    * no nosso caso, do teclado e irá escrever este string no array input que definimos anteriormente
    * o fgets retorna o ponteiro do próprio array input quanto tudo está correto ou um ponteiro NULL
    */
    while (fgets(input, COMPRIMENTO_MAXIMO_DA_LINHA, stdin) != NULL) {
        //vamos manter uma cópia porque todas as funções operam com os ponteiros
        //da string digitada.
        copy = input;

        //aqui faremos a avaliação da proposição digitada.
        if (FormulaWrapper(&copy)) {
            printf("===> Formula corretamente formatada. :)\n");
            if(!imprimiu)
                ImprimeTokens();
        }
        else
            printf("===> Formula mal formatada. :(\n");
        //tem q limpar o vetor Tokens dps
        LimpaTokens();
    }
    return 0;
}//fim do main

// Única função chamada para verficar nossas fórmulas
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



//funções de apoio
//devolve a string de entrada sem qualquer espaço em branco
//e faz isso, simplesmente incrementando o ponteiro da string de entrada
void RemoverEspacosEmBranco(char* s[]) {
    //testa todos as formas possíveis de se ter um espaço em branco na tela
    //e considera a quebra de linha e a tabulação como espaços em branco
    while (*s[0] == ' ' || *s[0] == '\n' || *s[0] == '\t')
        *s = *s + 1;
}

// Compara a string de entrada contra uma string conhecida, semente
// Se ouver coincidência adiantamos a string de entrada no comprimento da semente e devolvemos TRUE
//
bool match(char* s[], const char* semente) {
    // a comparação será feita com o retorno de strncmp(*s, semente, strlen(semente))
    // strncmp  compara duas strings, no caso s e semente até o comprimento de semente (strlen(semente)
    // e devolve um inteiro <0 ou >0 caso inexista igualdade ou zero se existir coincidência
    int comp = strlen(semente);
    if (strncmp(*s, semente, comp) == 0) {
        //sempre que que semente existir fazemos a aritmética de ponteiros com o comprimento de strlen
        *s = *s + comp;
        //encerramos a função retornando TRUE
        return true;
    }
    return false;
}

/*****************************************************************************
* Todas as funções principais se comportam da mesma forma.
* A função "bool Expressions(char *s[])" irá
*
*   1. comparar o string de entrada "s" contra expressão determinada na BNF.
*
*   2. Se (if) o string de entrada coincide com a espressão, então (then)
*      vamos adiantar o ponteiro da expressão de entrada, e
*      retornar ture.
*
*   3. Caso contrário, não alteramos o ponteiro do string de entrada e retornamos falso.
*
****************************************************************************/

// Para identificar a regra: Formula ::= Constante | Proposicao | FormulaUnaria | FormulaBinaria
// Neste caso uma regra de escolha Formula pode representar uma Constante OU Proposicao OU...
bool Formula(char* s[]) {
    //uma cópia loca do string de entrada
    char* original = *s;
    //removendo os espaços em branco
    RemoverEspacosEmBranco(s);
    //agora vamos chamar cada uma das funções das regras inferiores para ver se uma delas é verdadeira
    //e encerra função retornando true se encontrar uma delas
    if (Constante(s) || Proposicao(s) || FormulaUnaria(s) || FormulaBinaria(s)) return true;
    *s = original;
    return false;
}

// Identifica uma constante segundo a regra: Constante ::= "T" | "F"
// Esta é uma operação de escolha (Ou) entre dois símbolos "T" ou "F"
// cabe a nós interpretar como True, verdadeiro ou False, falso.
bool Constante(char* s[]) {
    char* original = *s;
    //só por segurança removemos os espaços em todas as funções
    RemoverEspacosEmBranco(s);
    //fazemos a verificação e interrompemos a função
    if (match(s, "T")) {
        pos++;
        Tokens[pos].classe = CLASS_OPERANDO;
        Tokens[pos].tipo = TYPE_CONST;
        Tokens[pos].valor = CONST_T;
        return true;
    } 
    else if (match(s, "F")) {
        pos++;
        Tokens[pos].classe = CLASS_OPERANDO;
        Tokens[pos].tipo = TYPE_CONST;
        Tokens[pos].valor = CONST_F;
        return true;
    }

    //não ouve coincidência então devolvemos o string original
    *s = original;
    return false;
}

// Identifica uma proposição segundo a regra Proposicao ::= [a-z0-9]+
// Neste caso, teremos que usar uma expressão regular [a-z0-9]+
// Mas é um padrão tão simples que pode ser colocado diretamente no sscanf
// A função sscanf_s lê um string, remove caracteres desta string de acordo
// com a regra e devolve a string removida na string inicial
bool Proposicao(char* s[]) {
    //primeiro copiamos para, se necessário devolver string sem modificações
    char* original = *s;
    //removemos espaços em branco, só por segurança novamente.
    RemoverEspacosEmBranco(s);

    // usaremos este array de char e este int para controlar o processo do sscanf
    char prop[COMPRIMENTO_PROPOSICAO];
    int  numeroCaracteresLidos;

    //vamos usar o retor no sscanf_s neste caso, com dois argumentos
    // %[a-z0-9] significando qualquer letra minúscula ou número que será devolvido em prop
    // %n que conterá a quantidade de caracteres que será devolvido em numeroCaracteresLidos
    // referência do sscanf em http://pubs.opengroup.org/onlinepubs/009695399/functions/scanf.html
    // se for ler strings lembre-se que é necessário dimensionar o buffer para cada string
    if (sscanf_s(*s, "%[a-z0-9]%n", &prop, sizeof(prop), &numeroCaracteresLidos) == 1) {

        prop[numeroCaracteresLidos + 1] = '\0';

        pos++;
        Tokens[pos].classe = CLASS_OPERANDO;
        Tokens[pos].tipo = TYPE_PROP;
        Tokens[pos].valor = prop;
        *s = *s + numeroCaracteresLidos;
        ImprimeTokens();
        if (*s != '\0' && !imprimiu)
            imprimiu = true;
        return true;
    }
    //não achou nenhuma proposição devolve o ponteiro original
    *s = original;
    return false;
}

//identificando o operador unário segundo a regra://OperatorUnario :: = "\neg"
//lembre-se que o \neg" está definido em STRING_NOT
bool OperadorUnario(char* s[]) {
    char* original = *s;
    RemoverEspacosEmBranco(s);

    if (match(s, STRING_NOT)) {
        pos++;
        Tokens[pos].classe = CLASS_OPERADOR;
        Tokens[pos].tipo = TYPE_OU;
        Tokens[pos].valor = STRING_NOT;
        return true;
    }
      
    *s = original;
    return false;
}

//identificamos o abrir e o fechar parênteses e os operadores
//binários da mesma forma
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

    if (match(s, STRING_OR)) {
        pos++;
        Tokens[pos].classe = CLASS_OPERADOR;
        Tokens[pos].tipo = TYPE_OB;
        Tokens[pos].valor = STRING_OR;
        return true;
    }
    else if (match(s, STRING_AND)) {
        pos++;
        Tokens[pos].classe = CLASS_OPERADOR;
        Tokens[pos].tipo = TYPE_OB;
        Tokens[pos].valor = STRING_AND;
        return true;
    }
    else if (match(s, STRING_IMPLIES)) {
        pos++;
        Tokens[pos].classe = CLASS_OPERADOR;
        Tokens[pos].tipo = TYPE_OB;
        Tokens[pos].valor = STRING_IMPLIES;
        return true;
    }
    else if (match(s, STRING_IFF)) {
        pos++;
        Tokens[pos].classe = CLASS_OPERADOR;
        Tokens[pos].tipo = TYPE_OB;
        Tokens[pos].valor = STRING_IFF;
        return true;
    }
    
    *s = original;
    return false;
}

// identificando a FormulaUnaria segundo a regra: FormulaUnaria ::= AbreParen OperadorUnario Formula FechaParen
// observe que cada função chamada adianta a posição do ponteiro do string de entrada
// observe também que basta que um elemento seja falso para que a formula não seja reconhecida
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