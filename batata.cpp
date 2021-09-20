#include <iostream>
#include <stdio.h>
#include "tinyexpr.h"

using namespace std;

// Função matemática a ser desenhada
string expression_string;
// Variável para "x"
double x_variable = 0;
// Controle de erros
int err;
// Ponteiro para expressão compilada
te_expr *expr;

// Solicita ao usuário que informe uma função
void obtemFuncao()
{
  cout << "Informe a equação a ser desenhada: ";
  getline(cin, expression_string);
  cout << "A equação será : " << expression_string << endl;
}

// Compila a expressão e registra as variáveis
void preparaFuncao()
{
  // Vincula o valor da variável global "x_variable" à variável "x" da expressão
  te_variable vars[] = {"x", &x_variable};

  /* Compile the expression with variables. */
  expr = te_compile(expression_string.c_str(), vars, 1, &err);
}

int main()
{
  // o intervalo entre os pontos (Quanto maior o denominador, mais pontos, melhor o gráfico. Use 10, 100, 1000...)
  double delta = 1.0 / 10.0;
  // O intervalo de valores para x, que vai desde xumin até xmax (Sistema de Coordenadas do Universo: SRU)
  double xumin = -10.0;
  double xumax = 10.0;

  obtemFuncao();
  preparaFuncao();

  for (double x = xumin; x < xumax; x += delta)
  {

    double calculed_value;

    // Coloca o valor da variável x na global "x_variable", que está vinculada à expressão
    x_variable = x;
    // avalia a expressão para x
    calculed_value = te_eval(expr);

    // Aqui mostra o valor ou manda desenhar o pixel
    printf("%19.15f\t%19.15f\n", x, calculed_value);
  }

  return 0;
}
