# Noki — Vector

# ENG version

Vectors are intrinsic Noki values, and their methods are accessed through the `VECTOR` pseudo-object.

```noki
v = [1, 2, 3]
v.size()
```

## `.pushback(value, ...)`

Appends one or more values to the end of the vector.

```noki
v = [1, 2, 3]

v.pushback(4)
v.pushback(5, 6)

println(v)
```

Output:

```text
[1, 2, 3, 4, 5, 6]
```

Returns `null`.

## `.insert(index, value, ...)`

Inserts one or more values at the specified position.

```noki
v = [1, 2, 3]

v.insert(0, 9)
```

Result:

```text
[9, 1, 2, 3]
```

Multiple values can also be inserted:

```noki
v.insert(1, 7, 8)
```

## `.pop(index)`

Removes the element at the specified index.

```noki
v = [10, 20, 30]

v.pop(1)
```

Result:

```text
[10, 30]
```

## `.remove(value, count)`

Removes occurrences of a value.

```noki
v = [1, 2, 2, 3, 2]

v.remove(2, 2)
```

`count` specifies how many occurrences are removed.

`-1` means remove all occurrences:

```noki
v.remove(2, -1)
```

Returns `null`.

## `.find(value)`

Finds the first occurrence of a value.

```noki
v = [10, 20, 30]

println(v.find(20))
println(v.find(99))
```

Output:

```text
1
-1
```

Returns the index or `-1`.

## `.size()`

Returns the number of elements in the vector.

```noki
v = [10, 20, 30]

println(v.size())
```

Output:

```text
3
```

## Complete example

```noki
import vector

v = [1, 2, 3]

println(v.size())

v.pushback(4)
println(v)

v.insert(0, 9)
println(v)

println(v.find(3))

v.pop(0)
println(v)

v.remove(2, -1)
println(v)
```

## Summary

```text
pushback
insert
pop
remove
find
size
```

## Type-based dispatch

Vector methods belong to the `VECTOR` type and are not global builtins.

For example:

```noki
s.find("abc")
```

uses the `STRING` implementation, while:

```noki
v.find(123)
```

uses the `VECTOR` implementation.

The dispatch is based on the runtime value type:

```text
STRING
    → RuntimeValueTypeID::STRING
    → String methods

VECTOR
    → RuntimeValueTypeID::VECTOR
    → Vector methods
```


# PT version

Vectors são valores intrínsecos do Noki e os seus métodos são acedidos através do pseudo-objecto `VECTOR`.

```noki
v = [1, 2, 3]
v.size()
```

## `.pushback(value, ...)`

Adiciona elementos ao fim do vector.

```noki
v = [1, 2, 3]

v.pushback(4)
v.pushback(5, 6)

println(v)
```

Resultado:

```text
[1, 2, 3, 4, 5, 6]
```

Retorna `null`.

## `.insert(index, value, ...)`

Insere elementos na posição indicada.

```noki
v = [1, 2, 3]

v.insert(0, 9)
```

Resultado:

```text
[9, 1, 2, 3]
```

Também aceita vários elementos:

```noki
v.insert(1, 7, 8)
```

## `.pop(index)`

Remove o elemento na posição indicada.

```noki
v = [10, 20, 30]

v.pop(1)
```

Resultado:

```text
[10, 30]
```

## `.remove(value, count)`

Remove ocorrências de um valor.

```noki
v = [1, 2, 2, 3, 2]

v.remove(2, 2)
```

O `count` controla o número de ocorrências removidas.

`-1` significa remover todas:

```noki
v.remove(2, -1)
```

Retorna `null`.

## `.find(value)`

Procura a primeira ocorrência de um valor.

```noki
v = [10, 20, 30]

println(v.find(20))
println(v.find(99))
```

Resultado:

```text
1
-1
```

Retorna o índice ou `-1`.

## `.size()`

Obtém o número de elementos.

```noki
v = [10, 20, 30]

println(v.size())
```

Resultado:

```text
3
```

## Exemplo completo

```noki
import vector

v = [1, 2, 3]

println(v.size())

v.pushback(4)
println(v)

v.insert(0, 9)
println(v)

println(v.find(3))

v.pop(0)
println(v)

v.remove(2, -1)
println(v)
```

## Resumo

```text
pushback
insert
pop
remove
find
size
```

## Dispatch por tipo

Os métodos pertencem ao tipo `VECTOR`, não são builtins globais.

Assim:

```noki
s.find("abc")
```

usa o método de `STRING`, enquanto:

```noki
v.find(123)
```

usa o método de `VECTOR`.

```text
STRING
    → RuntimeValueTypeID::STRING
    → String methods

VECTOR
    → RuntimeValueTypeID::VECTOR
    → Vector methods
```
