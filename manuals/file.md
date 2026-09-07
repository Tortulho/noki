# Noki `file` Library — Basic Manual

> **Document versions:**
> - **[EN] English Version** — complete English reference
> - **[PT-PT] Versão Portuguesa** — referência completa em português europeu
>
> Use the version labels above to quickly locate the desired language.

---

# [EN] English Version

The `file` library provides operations for creating, opening, reading, writing, searching, renaming, and removing files in Noki.

To use the library:

```noki
import file
```

> **Syntax note:** in Noki, assignment is performed with the `<-` operator.
> The `=` operator should not be used for assignment.

---

## [EN-01] Assignment Operator `<-`

When the result of an operation is stored in a variable, use `<-`:

```noki
pathname <- "teste.txt"

exists <- file.exists(pathname)

fp <- file.open(pathname, "rw")
```

The `<-` operator is especially important when working with objects returned by the `file` library, such as `File` objects.

---

## [EN-02] Creating a File

### `file.create(filename)`

Creates a new empty file.

```noki
result <- file.create("teste.txt")
```

The argument must be a `string` containing the file path.

If the file already exists, it is not recreated.

Example:

```noki
import file

result <- file.create("teste.txt")
```

---

## [EN-03] Checking Whether a File Exists

### `file.exists(filename)`

Checks whether the specified path corresponds to a regular file.

```noki
exists <- file.exists("teste.txt")
```

The result is boolean and can be used directly in a condition:

```noki
if (file.exists("teste.txt")) {
    println("The file exists.")
}
```

---

## [EN-04] Opening a File

### `file.open(filename, mode)`

Creates a `File` object and opens the file.

```noki
fp <- file.open("teste.txt", "rw")
```

Available modes:

| Mode | Meaning |
|---|---|
| `"r"` | Read |
| `"w"` | Write |
| `"rw"` | Read and write |

The returned object can then be used through the `File` methods.

---

## [EN-05] `set`

`set` can be used both as a library function and as a method of a `File` object.

### `file.set(filename, mode)`

Creates a `File` object, sets its path and mode, and opens the file.

```noki
fp <- file.set("teste.txt", "rw")
```

With mode `"w"`, the file can be created when it does not already exist.

### `fp.set(filename, mode)`

Sets the path and mode of a closed `File` object and opens the file.

```noki
fp.set("outro.txt", "rw")
```

---

## [EN-06] Closing a File

### `fp.close()`

Explicitly closes the stream associated with the `File` object.

```noki
fp.close()
```

After `close()`, operations that work on the open stream, such as `read()`, `write()`, and `search()`, cannot be used until the file is opened again.

---

## [EN-07] Automatic Closing Inside Clauses

`File` objects follow the lifetime of the context/block in which they are used. When a file is associated with a variable inside a clause/block and that context ends, the file can be closed automatically by Noki's RAII system.

For example:

```noki
if (file.exists("teste.txt")) {
    fp <- file.open("teste.txt", "r")

    content <- fp.load()
    println(content)
}
```

When the block ends, the `File` object leaves its context and the stream is closed automatically.

Care must be taken with Noki's syntax rules for clauses and `{ ... }` blocks, especially regarding variable scope and the moment at which the object leaves its context.

When explicit control over the closing moment is required, `fp.close()` can be used.

---

## [EN-08] Writing to a File

### `fp.write(value)`

Writes a `string` at the current file position.

The file must be open and its mode cannot be `"r"`.

```noki
fp <- file.open("teste.txt", "rw")

fp.write("Line 1\n")
fp.write("Line 2\n")

fp.close()
```

The object's internal position is updated after the write.

---

## [EN-09] Reading Data

### `fp.read(size)`

Reads `size` bytes starting at the current file position.

```noki
fp <- file.open("teste.txt", "r")

text <- fp.read(10)
println(text)

fp.close()
```

### `fp.read(size, position)`

Reads `size` bytes starting at a specific position.

```noki
fp <- file.open("teste.txt", "r")

text <- fp.read(5, 0)
println(text)

fp.close()
```

---

## [EN-10] Loading the Complete File

### `fp.load()`

Loads the entire file contents into a `string`.

**`load()` is a self-contained operation:** it internally opens the file for reading, reads the contents, and automatically closes the file when the operation finishes.

The `File` object does not need to keep its internal stream open to use `load()`.

```noki
fp <- file.open("teste.txt", "r")

content <- fp.load()
println(content)
```

`load()` does not leave the `FileObject` internal stream open.

---

## [EN-11] Saving Content to a File

### `fp.save(value)`

Replaces the contents of the file with the supplied value.

**`save()` is also a self-contained operation:** it internally opens the file in write mode, writes the contents, and automatically closes the file.

```noki
fp <- file.open("teste.txt", "rw")

fp.save("New content\n")
```

Like `load()`, `save()` does not depend on the internal stream being open and does not leave it open after the operation finishes.

---

## [EN-12] `load()` and `save()` Do Not Require Manual `open()`/`close()`

Because these two operations handle opening and closing internally, they can be used directly on a `File` object:

```noki
fp <- file.set("teste.txt", "rw")

content <- fp.load()
fp.save("New content")
```

The need for `open()`/`close()` is related to operations that directly use the object's stream, such as `read()`, `write()`, and `search()`.

---

## [EN-13] Getting the File Size

### `fp.size()`

Returns the current file size in bytes.

```noki
size <- fp.size()
println(size)
```

---

## [EN-14] Searching for Text

### `fp.search(value)`

Searches for a `string` starting at the current position.

```noki
position <- fp.search("Noki")
```

### `fp.search(value, position)`

Starts the search at a specific position.

```noki
position <- fp.search("Noki", 0)
```

The result is the position where the text was found. When there is no match, the result is `-1`.

Example:

```noki
fp <- file.open("teste.txt", "r")

pos <- fp.search("Line 2")
println(pos)

fp.close()
```

---

## [EN-15] Renaming a File

### `fp.rename(newpath)`

Renames the file associated with the `File` object and updates the path stored in the object.

```noki
result <- fp.rename("new_name.txt")
```

If the file is open, its stream is closed before the rename operation.

After `rename()`, the object refers to the new path.

---

## [EN-16] Renaming Through the Library

### `file.rename(oldpath, newpath)`

Renames a file directly using the supplied paths.

```noki
result <- file.rename(
    "teste.txt",
    "teste_final.txt"
)
```

This form does not require a `File` object.

---

## [EN-17] Removing a File

### `file.remove(path)`

Removes the specified file.

```noki
result <- file.remove("teste.txt")
```

The result is boolean and indicates whether the file was removed.

---

## [EN-18] Complete Example

```noki
import file

println("================================")
println(" FILE LIBRARY EXAMPLE")
println("================================")

pathname <- "teste.txt"

# Create the file
created <- file.create(pathname)

# Check existence
if (file.exists(pathname)) {
    println("File created.")
}

# Open for reading and writing
fp <- file.open(pathname, "rw")

# Write
fp.write("Line 1\n")
fp.write("Line 2\n")
fp.write("Line 3\n")

println("Size:")
println(fp.size())

# Close the stream
fp.close()

# load() opens, reads, and closes internally
content <- fp.load()
println(content)

# Open for stream-based operations
fp.open()

# Read the content
content <- fp.read(fp.size())
println(content)

# Close before renaming
fp.close()
fp.rename("teste_renomeado.txt")

# Check the new path
if (file.exists("teste_renomeado.txt")) {
    println("File renamed.")
}

# Remove
file.remove("teste_renomeado.txt")

println("================================")
println(" END")
println("================================")
```

---

## [EN-19] API Summary

### Library functions

```noki
file.create(filename)
file.exists(filename)
file.open(filename, mode)
file.set(filename, mode)
file.rename(oldpath, newpath)
file.remove(path)
```

### `File` methods

```noki
fp.open()
fp.open(filename, mode)
fp.set(filename, mode)
fp.close()
fp.load()
fp.save(value)
fp.read(size)
fp.read(size, position)
fp.write(value)
fp.search(value)
fp.search(value, position)
fp.size()
fp.rename(newpath)
```

### File opening modes

```text
r   -> read
w   -> write
rw  -> read and write
```

### Assignment operator

```noki
variable <- value
```

---

# [PT-PT] Versão Portuguesa

A biblioteca `file` fornece operações para criar, abrir, ler, escrever, procurar, renomear e remover ficheiros no Noki.

Para utilizar a biblioteca:

```noki
import file
```

> **Nota de sintaxe:** no Noki, a atribuição é feita com o operador `<-`.
> Não deve ser utilizado `=` para atribuir valores.

---

## [PT-01] Operador de atribuição `<-`

Quando o resultado de uma operação é guardado numa variável, utiliza-se `<-`:

```noki
pathname <- "teste.txt"

existe <- file.exists(pathname)

fp <- file.open(pathname, "rw")
```

O operador `<-` é especialmente importante quando se trabalha com objetos retornados pela biblioteca `file`, como `File`.

---

## [PT-02] Criar um ficheiro

### `file.create(filename)`

Cria um novo ficheiro vazio.

```noki
resultado <- file.create("teste.txt")
```

O argumento deve ser uma `string` com o caminho do ficheiro.

Se o ficheiro já existir, não é recriado.

Exemplo:

```noki
import file

resultado <- file.create("teste.txt")
```

---

## [PT-03] Verificar se um ficheiro existe

### `file.exists(filename)`

Verifica se o caminho corresponde a um ficheiro regular.

```noki
existe <- file.exists("teste.txt")
```

O resultado é booleano e pode ser utilizado diretamente numa condição:

```noki
if (file.exists("teste.txt")) {
    println("O ficheiro existe.")
}
```

---

## [PT-04] Abrir um ficheiro

### `file.open(filename, mode)`

Cria um objeto `File` e abre o ficheiro.

```noki
fp <- file.open("teste.txt", "rw")
```

Os modos disponíveis são:

| Modo | Significado |
|---|---|
| `"r"` | Leitura |
| `"w"` | Escrita |
| `"rw"` | Leitura e escrita |

O objeto devolvido pode depois ser utilizado através dos métodos de `File`.

---

## [PT-05] `set`

`set` pode ser utilizado tanto como função da biblioteca como método de um objeto `File`.

### `file.set(filename, mode)`

Cria um objeto `File`, define o caminho e o modo e abre o ficheiro.

```noki
fp <- file.set("teste.txt", "rw")
```

No modo `"w"`, o ficheiro pode ser criado caso ainda não exista.

### `fp.set(filename, mode)`

Define o caminho e o modo de um objeto `File` fechado e abre o ficheiro.

```noki
fp.set("outro.txt", "rw")
```

---

## [PT-06] Fechar um ficheiro

### `fp.close()`

Fecha explicitamente a stream associada ao objeto `File`.

```noki
fp.close()
```

Depois de `close()`, operações que trabalham sobre a stream aberta, como `read()`, `write()` e `search()`, não podem ser utilizadas até o ficheiro voltar a ser aberto.

---

## [PT-07] Fecho automático dentro de clauses

Os objetos `File` respeitam o tempo de vida do contexto/bloco onde são utilizados. Quando um ficheiro está associado a uma variável dentro de uma clause/bloco e esse contexto termina, o ficheiro pode ser fechado automaticamente pelo sistema RAII do Noki.

Por exemplo:

```noki
if (file.exists("teste.txt")) {
    fp <- file.open("teste.txt", "r")

    conteudo <- fp.load()
    println(conteudo)
}
```

Ao sair do bloco, o objeto `File` deixa o seu contexto e a stream é fechada automaticamente.

Apesar disso, deve ter-se cuidado com as regras de sintaxe das clauses e dos blocos `{ ... }`, sobretudo relativamente ao âmbito das variáveis e ao momento em que o objeto deixa de estar disponível.

Quando for necessário controlar explicitamente o momento do fecho, pode ser utilizado `fp.close()`.

---

## [PT-08] Escrever num ficheiro

### `fp.write(value)`

Escreve uma `string` na posição atual do ficheiro.

O ficheiro tem de estar aberto e o modo não pode ser `"r"`.

```noki
fp <- file.open("teste.txt", "rw")

fp.write("Linha 1\n")
fp.write("Linha 2\n")

fp.close()
```

A posição interna do objeto é atualizada depois da escrita.

---

## [PT-09] Ler dados

### `fp.read(size)`

Lê `size` bytes a partir da posição atual do ficheiro.

```noki
fp <- file.open("teste.txt", "r")

texto <- fp.read(10)
println(texto)

fp.close()
```

### `fp.read(size, position)`

Lê `size` bytes começando numa posição específica.

```noki
fp <- file.open("teste.txt", "r")

texto <- fp.read(5, 0)
println(texto)

fp.close()
```

---

## [PT-10] Carregar o conteúdo completo

### `fp.load()`

Carrega todo o conteúdo do ficheiro para uma `string`.

**`load()` é uma operação autónoma:** abre internamente o ficheiro para leitura, lê o conteúdo e fecha o ficheiro automaticamente no final da operação.

Não é necessário manter o `File` aberto para utilizar `load()`.

```noki
fp <- file.open("teste.txt", "r")

conteudo <- fp.load()
println(conteudo)
```

O `load()` não deixa a stream interna do `FileObject` aberta.

---

## [PT-11] Guardar conteúdo no ficheiro

### `fp.save(value)`

Substitui o conteúdo do ficheiro pelo valor fornecido.

**`save()` também é uma operação autónoma:** abre internamente o ficheiro em modo de escrita, grava o conteúdo e fecha o ficheiro automaticamente.

```noki
fp <- file.open("teste.txt", "rw")

fp.save("Novo conteúdo\n")
```

Tal como `load()`, `save()` não depende da stream interna estar aberta e não a deixa aberta depois de terminar.

---

## [PT-12] `load()` e `save()` não exigem `open()`/`close()` manual

Como estas duas operações tratam internamente da abertura e do fecho, podem ser usadas de forma direta sobre um `File`:

```noki
fp <- file.set("teste.txt", "rw")

conteudo <- fp.load()
fp.save("Novo conteúdo")
```

A necessidade de `open()`/`close()` está relacionada com as operações que utilizam diretamente a stream do objeto, como `read()`, `write()` e `search()`.

---

## [PT-13] Obter o tamanho do ficheiro

### `fp.size()`

Devolve o tamanho atual do ficheiro em bytes.

```noki
tamanho <- fp.size()
println(tamanho)
```

---

## [PT-14] Procurar texto

### `fp.search(value)`

Procura uma `string` a partir da posição atual.

```noki
posicao <- fp.search("Noki")
```

### `fp.search(value, position)`

Começa a procura numa posição específica.

```noki
posicao <- fp.search("Noki", 0)
```

O resultado é a posição onde o texto foi encontrado. Quando não existe correspondência, o resultado é `-1`.

Exemplo:

```noki
fp <- file.open("teste.txt", "r")

pos <- fp.search("Linha 2")
println(pos)

fp.close()
```

---

## [PT-15] Renomear um ficheiro

### `fp.rename(newpath)`

Renomeia o ficheiro associado ao objeto `File` e atualiza o caminho guardado no objeto.

```noki
resultado <- fp.rename("novo_nome.txt")
```

Se o ficheiro estiver aberto, a stream é fechada antes da operação de renomeação.

Depois do `rename()`, o objeto passa a referir o novo caminho.

---

## [PT-16] Renomear através da biblioteca

### `file.rename(oldpath, newpath)`

Renomeia diretamente um ficheiro através dos caminhos fornecidos.

```noki
resultado <- file.rename(
    "teste.txt",
    "teste_final.txt"
)
```

Esta forma não requer um objeto `File`.

---

## [PT-17] Remover um ficheiro

### `file.remove(path)`

Remove o ficheiro indicado.

```noki
resultado <- file.remove("teste.txt")
```

O resultado é booleano e indica se o ficheiro foi removido.

---

## [PT-18] Exemplo completo

```noki
import file

println("================================")
println(" EXEMPLO DA BIBLIOTECA FILE")
println("================================")

pathname <- "teste.txt"

# Criar o ficheiro
criado <- file.create(pathname)

# Verificar existência
if (file.exists(pathname)) {
    println("Ficheiro criado.")
}

# Abrir para leitura e escrita
fp <- file.open(pathname, "rw")

# Escrever
fp.write("Linha 1\n")
fp.write("Linha 2\n")
fp.write("Linha 3\n")

println("Tamanho:")
println(fp.size())

# Fechar a stream
fp.close()

# load() abre, lê e fecha internamente
conteudo <- fp.load()
println(conteudo)

# Abrir para operações sobre a stream
fp.open()

# Ler o conteúdo
conteudo <- fp.read(fp.size())
println(conteudo)

# Fechar antes de renomear
fp.close()
fp.rename("teste_renomeado.txt")

# Verificar novo caminho
if (file.exists("teste_renomeado.txt")) {
    println("Ficheiro renomeado.")
}

# Remover
file.remove("teste_renomeado.txt")

println("================================")
println(" FIM")
println("================================")
```

---

## [PT-19] Resumo da API

### Funções da biblioteca

```noki
file.create(filename)
file.exists(filename)
file.open(filename, mode)
file.set(filename, mode)
file.rename(oldpath, newpath)
file.remove(path)
```

### Métodos de `File`

```noki
fp.open()
fp.open(filename, mode)
fp.set(filename, mode)
fp.close()
fp.load()
fp.save(value)
fp.read(size)
fp.read(size, position)
fp.write(value)
fp.search(value)
fp.search(value, position)
fp.size()
fp.rename(newpath)
```

### Modos de abertura

```text
r   -> leitura
w   -> escrita
rw  -> leitura e escrita
```

### Operador de atribuição

```noki
variavel <- valor
```
