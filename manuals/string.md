# Noki — String

# ENG version

Strings are intrinsic Noki values, and their methods are accessed through the `STRING` pseudo-object.

```noki
s = "hello"
s.size()
```

## `.format(...)`

Replaces format specifiers in the string itself.

| Specifier | Type   |
| --------- | ------ |
| `%d`      | int    |
| `%f`      | float  |
| `%s`      | string |
| `%v`      | vector |
| `%%`      | `%`    |

Example:

```noki
s = "int=%d float=%f string=%s"
s.format(123, 4.5, "hello")
println(s)
```

Output:

```text
int=123 float=4.500000 string=hello
```

Vectors are formatted recursively:

```noki
s = "v=%v"
s.format([1, 2, [3, 4]])
println(s)
```

Output:

```text
v=[1, 2, [3, 4]]
```

Returns `true`.

## `.cat(value)`

Appends a value to the end of the string.

Accepts `string`, `int`, `float`, and `bool`.

```noki
s = "Hello"
s.cat(" World")
s.cat(123)
s.cat(4.5)
s.cat(true)
```

Booleans are converted to `true` or `false`.

Returns `null`.

## `.find(string [, index])`

Finds the first occurrence of a string.

```noki
s = "abc abc abc"

s.find("abc")       # 0
s.find("abc", 1)    # 4
```

Negative indices are relative to the end of the string.

Returns the matching index or `-1`.

## `.rfind(string [, index])`

Searches for an occurrence starting from the end.

```noki
s = "abc abc abc"

s.rfind("abc")
s.rfind("abc", 6)
```

Returns the matching index or `-1`.

## `.replace(string, string [, count])`

Replaces occurrences and returns the number of replacements.

```noki
s = "one two two two three"

s.replace("two", "X")
```

With a limit:

```noki
s.replace("two", "X", 2)
```

A negative `count` means that all occurrences are replaced.

## `.rreplace(string, string [, count])`

Works like `replace`, but replacements are performed starting from the end of the string.

```noki
s = "a b b b c"

s.rreplace("b", "X", 2)
```

Returns the number of replacements.

## `.remove(string)`

Removes all occurrences of the specified string.

```noki
s = "hello hello world"

s.remove("hello ")
```

Returns `true` if at least one occurrence was removed, otherwise `false`.

## `.at(index)`

Returns a single character as a string of length 1.

```noki
s = "Noki"

println(s.at(0))
println(s.at(1))
```

Output:

```text
N
o
```

## `.copy(size [, index])`

Copies part of the string and returns a new string.

```noki
s = "abcdef"

s.copy(3)       # "abc"
s.copy(3, 2)    # "cde"
s.copy(2, -2)   # "ef"
```

## `.strip()`

Removes whitespace from the beginning and end of the string.

```noki
s = "   hello noki   "
s.strip()
```

The resulting string is:

```text
hello noki
```

Returns `null`.

## `.reserve(size)`

Reserves internal capacity for the string.

```noki
s = "hello"
s.reserve(100)
```

This does not change the current string length.

Returns `null`.

## `.reverse()`

Reverses the string in place.

```noki
s = "abcdef"
s.reverse()
```

Result:

```text
fedcba
```

Returns `null`.

## `.popback(size)`

Removes `size` characters from the end of the string.

```noki
s = "abcdef"

s.popback(2)
```

The string becomes:

```text
abcd
```

Returns `true`.

If the requested number of characters cannot be removed, it returns `false` and the string remains unchanged.

## `.size()`

Returns the number of characters in the string.

```noki
s = "Noki"
println(s.size())
```

Output:

```text
4
```

## `.isempty()`

Checks whether the string is empty.

```noki
s = ""
println(s.isempty())
```

Returns `true` or `false`.

## `.empty()`

Clears the string.

```noki
s = "hello"
s.empty()

println(s.isempty())
```

Output:

```text
true
```

Returns `null`.

## `.pop(index)`

Removes a character at the specified index.

```noki
s = "abcdef"

s.pop(2)
```

The string becomes:

```text
abdef
```

Returns `true` or `false`.

Negative indices are supported.

## `.pop(start, end)`

Removes a range of characters.

The `end` index is **inclusive**.

```noki
s = "abcdef"

s.pop(1, 3)
```

Removes `bcd`, leaving:

```text
aef
```

Returns `true` or `false`.

## Summary

```text
format
cat
find
rfind
replace
rreplace
remove
at
copy
strip
reserve
reverse
popback
size
isempty
empty
pop
```


# PT version

Strings são valores intrínsecos do Noki e os seus métodos são acedidos através do pseudo-objecto `STRING`.

```noki
s = "hello"
s.size()
```

## `.format(...)`

Substitui especificadores na própria string.

| Especificador | Tipo   |
| ------------- | ------ |
| `%d`          | int    |
| `%f`          | float  |
| `%s`          | string |
| `%v`          | vector |
| `%%`          | `%`    |

Exemplo:

```noki
s = "int=%d float=%f string=%s"
s.format(123, 4.5, "hello")
println(s)
```

Resultado:

```text
int=123 float=4.500000 string=hello
```

Vectors são convertidos recursivamente:

```noki
s = "v=%v"
s.format([1, 2, [3, 4]])
println(s)
```

Resultado:

```text
v=[1, 2, [3, 4]]
```

Retorna `true`.

## `.cat(value)`

Concatena um valor no final da string.

Aceita `string`, `int`, `float` e `bool`.

```noki
s = "Hello"
s.cat(" World")
s.cat(123)
s.cat(4.5)
s.cat(true)
```

`bool` é convertido para `true` ou `false`.

Retorna `null`.

## `.find(string [, index])`

Procura a primeira ocorrência.

```noki
s = "abc abc abc"

s.find("abc")       # 0
s.find("abc", 1)    # 4
```

Índices negativos são relativos ao fim.

Retorna o índice ou `-1`.

## `.rfind(string [, index])`

Procura a partir do fim.

```noki
s = "abc abc abc"

s.rfind("abc")
s.rfind("abc", 6)
```

Retorna o índice ou `-1`.

## `.replace(string, string [, count])`

Substitui ocorrências e retorna o número de substituições.

```noki
s = "one two two two three"

s.replace("two", "X")
```

Com limite:

```noki
s.replace("two", "X", 2)
```

Um `count` negativo significa substituir todas as ocorrências.

## `.rreplace(string, string [, count])`

Faz o mesmo que `replace`, mas começa pelas ocorrências do fim.

```noki
s = "a b b b c"

s.rreplace("b", "X", 2)
```

Retorna o número de substituições.

## `.remove(string)`

Remove todas as ocorrências.

```noki
s = "hello hello world"

s.remove("hello ")
```

Retorna `true` se algo foi removido e `false` caso contrário.

## `.at(index)`

Obtém um único carácter como uma string de tamanho 1.

```noki
s = "Noki"

println(s.at(0))
println(s.at(1))
```

Resultado:

```text
N
o
```

## `.copy(size [, index])`

Copia uma quantidade de caracteres.

```noki
s = "abcdef"

s.copy(3)       # "abc"
s.copy(3, 2)    # "cde"
s.copy(2, -2)   # "ef"
```

Retorna uma nova string.

## `.strip()`

Remove whitespace no início e no fim.

```noki
s = "   hello noki   "
s.strip()
```

Resultado:

```text
hello noki
```

Retorna `null`.

## `.reserve(size)`

Reserva capacidade interna para a string.

```noki
s = "hello"
s.reserve(100)
```

Não altera o tamanho atual.

Retorna `null`.

## `.reverse()`

Inverte a string.

```noki
s = "abcdef"
s.reverse()
```

Resultado:

```text
fedcba
```

Retorna `null`.

## `.popback(size)`

Remove `size` caracteres do fim.

```noki
s = "abcdef"

s.popback(2)
```

A string passa a ser:

```text
abcd
```

Retorna `true`.

Quando não é possível remover a quantidade pedida, retorna `false` e a string permanece inalterada.

## `.size()`

Obtém o número de caracteres.

```noki
s = "Noki"
println(s.size())
```

Resultado:

```text
4
```

## `.isempty()`

Verifica se a string está vazia.

```noki
s = ""
println(s.isempty())
```

Retorna `true` ou `false`.

## `.empty()`

Limpa a string.

```noki
s = "hello"
s.empty()

println(s.isempty())
```

Resultado:

```text
true
```

Retorna `null`.

## `.pop(index)`

Remove um carácter.

```noki
s = "abcdef"

s.pop(2)
```

Resultado:

```text
abdef
```

Retorna `true` ou `false`.

Índices negativos são aceites.

## `.pop(start, end)`

Remove um intervalo.

O `end` é **inclusivo**.

```noki
s = "abcdef"

s.pop(1, 3)
```

Remove `bcd`, ficando:

```text
aef
```

Retorna `true` ou `false`.

## Resumo

```text
format
cat
find
rfind
replace
rreplace
remove
at
copy
strip
reserve
reverse
popback
size
isempty
empty
pop
```
