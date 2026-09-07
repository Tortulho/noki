# Noki — Basic things

# ENG VERSION

# Noki — Basic Usage Manual

## 1. Variables

Variables are created through assignment:

```noki
a = 10
b = 4.5
name = "Noki"
active = true
```

Basic types include:

```text
int
float
bool
string
vector
null
```

The variable type is determined by the assigned value.

---

## 2. Integers and Floats

Mathematical operations can be performed directly:

```noki
a = 10
b = 5

c = a + b
d = a - b
e = a * b
f = a / b
h = a ** b (pow)
```

Expressions can also be used directly:

```noki
result = 1 + 4
```

---

## 3. Booleans

Boolean values are:

```noki
a = true
b = false
```

They can be used in conditions:

```noki
if (a)
{
    println("active")
}
```

Comparison operators include:

```noki
a == b
a != b
a < b
a <= b
a > b
a >= b
```

Comparisons produce `true` or `false`.

---

## 4. Strings

Strings are enclosed in quotation marks:

```noki
name = "name"
message = "Hello World"
```

String methods are accessed directly:

```noki
name.cat("!")
println(name)
```

For example:

```noki
s = "abcdef"

println(s.size())
println(s.at(0))

s.reverse()
println(s)
```

See the String manual for the complete list of available methods.

---

## 5. Vectors

Vectors are written between `[` and `]`:

```noki
v = [1, 2, 3]
```

They can contain different types of values:

```noki
v = [1, 2.5, true, "hello"]
```

Vectors can also contain other vectors:

```noki
v = [1, [2, 3], [4, [5, 6]]]
```

Vector methods are accessed directly:

```noki
println(v.size())
v.pushback(4)
println(v)
```

See the Vector manual for the complete list of available methods.

---

## 6. Functions

Functions are defined with parameters and a body:

```noki
function soma(a, b)
{
    return a + b
}
```

They can then be called normally:

```noki
result = soma(10, 5)
println(result)
```

---

## 7. Return

A function can return a value using `return`:

```noki
function dobro(x)
{
    return x * 2
}
```

Usage:

```noki
a = dobro(5)
println(a)
```

Output:

```text
10
```

---

## 8. Conditions

The basic conditional structure is:

```noki
if (condition)
{
    ...
}
```

An `else` block can also be used:

```noki
if (a > 10)
{
    println("greater")
}
else
{
    println("less than or equal")
}
```

---

## 9. While Loops

A `while` loop keeps running while its condition is true:

```noki
i = 0

while (i < 5)
{
    println(i)
    i = i + 1
}
```

---

## 10. Break and Continue

`break` immediately terminates the current loop:

```noki
while (true)
{
    if (condition)
    {
        break
    }
}
```

`continue` skips to the next iteration:

```noki
while (i < 10)
{
    i = i + 1

    if (i == 5)
    {
        continue
    }

    println(i)
}
```

---

## 11. Builtin Functions

Noki provides several globally available builtin functions.

### `println(...)`

Prints values and adds a newline:

```noki
println("Hello")
println(123)
println(true)
```

### `print(...)`

Prints values without adding a newline:

```noki
print("Hello ")
print("World")
```

### `int(...)`

Converts a value to an integer:

```noki
a = int("123")
```

### `float(...)`

Converts a value to a float:

```noki
a = float("12.5")
```

### `string(...)`

Converts a value to a string:

```noki
a = string(123)
b = string(4.5)
c = string(true)
```

### `getInput()`

Reads a line from the user:

```noki
name = getInput()
println(name)
```

---

## 12. Libraries

Libraries are imported with the `import` statement:

```noki
import string
import vector
```

After importing a library, its functionality becomes available to the corresponding value types.

For example:

```noki
import string

s = "hello"
println(s.find("ell"))
```

And:

```noki
import vector

v = [1, 2, 3]
v.pushback(4)
println(v)
```

Methods belong to the type of the value. Therefore, the same method name can exist for different types:

```noki
s.find("a")
v.find(1)
```

The correct implementation is selected automatically according to the receiver's type.

---

## 13. Move

The `<-` operator performs a move:

```noki
a = [1, 2, 3]
b <- a
```

The value is transferred from `a` to `b`.

The destination receives ownership of the moved value.

---

## 14. Null

`null` represents the absence of a value:

```noki
a = null
```

It can also be returned by functions that do not produce a value:

```noki
function test()
{
    return null
}
```

---

## 15. Example Program

```noki
import string
import vector

name = "Noki"
age = 1

println(name)

name.cat(" Language")
println(name)

v = [1, 2, 3]

v.pushback(4)
v.pushback(5)

println(v)
println(v.size())

if (v.size() > 3)
{
    println("Vector has more than 3 elements")
}

function dobro(x)
{
    return x * 2
}

result = dobro(10)

println(result)
```

Expected output:

```text
Noki
Noki Language
[1, 2, 3, 4, 5]
5
Vector has more than 3 elements
20
```

---

## 16. General Structure

A Noki program can combine these features:

```text
imports
    ↓
variables
    ↓
expressions
    ↓
functions
    ↓
conditions
    ↓
loops
    ↓
function calls / methods
```

For example:

```noki
import string

s = "hello"

if (s.size() > 0)
{
    println(s)
}
```


# PT VERSION

## 1. Variáveis

As variáveis são criadas através de uma atribuição:

```noki
a = 10
b = 4.5
nome = "Noki"
ativo = true
```

Os tipos básicos incluem:

```text
int
float
bool
string
vector
null
```

O tipo da variável é determinado pelo valor atribuído.

---

## 2. Inteiros e floats

Operações matemáticas podem ser feitas diretamente:

```noki
a = 10
b = 5

c = a + b
d = a - b
e = a * b
f = a / b
h = a ** b (pow / potência)
```

Também podem ser utilizadas expressões:

```noki
result = 1 + 4
```

O resultado é armazenado em `result`.

---

## 3. Booleanos

Booleanos podem ser:

```noki
a = true
b = false
```

Podem ser utilizados em condições:

```noki
if (a)
{
    println("ativo")
}
```

Também existem operadores de comparação:

```noki
a == b
a != b
a < b
a <= b
a > b
a >= b
```

As comparações produzem `true` ou `false`.

---

## 4. Strings

Strings são delimitadas por aspas:

```noki
nome = "nome"
mensagem = "Hello World"
```

Podem ser modificadas através dos seus métodos:

```noki
nome.cat("!")
println(nome)
```

Por exemplo:

```noki
s = "abcdef"

println(s.size())
println(s.at(0))

s.reverse()
println(s)
```

Os métodos disponíveis estão documentados no manual de Strings.

---

## 5. Vectors

Vectors são definidos entre `[` e `]`:

```noki
v = [1, 2, 3]
```

Podem conter diferentes tipos de valores:

```noki
v = [1, 2.5, true, "hello"]
```

Também podem conter outros vectors:

```noki
v = [1, [2, 3], [4, [5, 6]]]
```

Os métodos são utilizados diretamente no vector:

```noki
println(v.size())
v.pushback(4)
println(v)
```

---

## 6. Funções

Uma função Noki é definida com parâmetros e um corpo.

Exemplo:

```noki
function soma(a, b)
{
    return a + b
}
```

Pode depois ser chamada:

```noki
resultado = soma(10, 5)
println(resultado)
```

---

## 7. Return

Uma função pode devolver um valor usando `return`:

```noki
function dobro(x)
{
    return x * 2
}
```

Uso:

```noki
a = dobro(5)
println(a)
```

Resultado:

```text
10
```

---

## 8. Condições

A estrutura básica é:

```noki
if (condicao)
{
    ...
}
```

Também pode existir `else`:

```noki
if (a > 10)
{
    println("maior")
}
else
{
    println("menor ou igual")
}
```

---

## 9. While

Um ciclo `while` executa enquanto a condição for verdadeira:

```noki
i = 0

while (i < 5)
{
    println(i)
    i = i + 1
}
```

---

## 10. Break e Continue

`break` termina imediatamente o ciclo:

```noki
while (true)
{
    if (condicao)
    {
        break
    }
}
```

`continue` passa diretamente para a próxima iteração:

```noki
while (i < 10)
{
    i = i + 1

    if (i == 5)
    {
        continue
    }

    println(i)
}
```

---

## 11. Funções Builtin

O Noki possui algumas funções disponíveis globalmente.

### `println(...)`

Imprime os valores e termina com uma nova linha:

```noki
println("Hello")
println(123)
println(true)
```

### `print(...)`

Imprime sem adicionar uma nova linha:

```noki
print("Hello ")
print("World")
```

### `int(...)`

Converte um valor para inteiro:

```noki
a = int("123")
```

### `float(...)`

Converte um valor para float:

```noki
a = float("12.5")
```

### `string(...)`

Converte valores para string:

```noki
a = string(123)
b = string(4.5)
c = string(true)
```

### `getInput()`

Lê uma linha introduzida pelo utilizador:

```noki
nome = getInput()
println(nome)
```

---

## 12. Libraries

Libraries podem ser importadas com:

```noki
import string
import vector
```

Depois podem ser utilizados os métodos associados aos valores.

Por exemplo:

```noki
import string

s = "hello"
println(s.find("ell"))
```

Ou:

```noki
import vector

v = [1, 2, 3]
v.pushback(4)
println(v)
```

Os métodos pertencem ao tipo do valor, portanto a mesma função pode existir em vários tipos:

```noki
s.find("a")
v.find(1)
```

Cada chamada utiliza automaticamente a implementação correspondente ao tipo do receiver.

---

## 13. Move

O operador `<-` realiza um move:

```noki
a = [1, 2, 3]
b <- a
```

Depois do move, o valor é transferido de `a` para `b`.

O destino recebe a propriedade do valor movido.

---

## 14. Null

`null` representa a ausência de um valor:

```noki
a = null
```

Pode ser devolvido por funções que não produzem um valor:

```noki
function teste()
{
    return null
}
```

---

## 15. Exemplo completo

```noki
import string
import vector

nome = "Noki"
idade = 1

println(nome)

nome.cat(" Language")
println(nome)

v = [1, 2, 3]

v.pushback(4)
v.pushback(5)

println(v)
println(v.size())

if (v.size() > 3)
{
    println("Vector tem mais de 3 elementos")
}

function dobro(x)
{
    return x * 2
}

resultado = dobro(10)

println(resultado)
```

Resultado esperado:

```text
Noki
Noki Language
[1, 2, 3, 4, 5]
5
Vector tem mais de 3 elementos
20
```

---

## 16. Estrutura geral

Um programa Noki pode combinar estes elementos livremente:

```text
imports
    ↓
variáveis
    ↓
expressões
    ↓
funções
    ↓
condições
    ↓
ciclos
    ↓
calls / methods
```

Exemplo:

```noki
import string

s = "hello"

if (s.size() > 0)
{
    println(s)
}
```
