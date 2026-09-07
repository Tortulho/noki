# Noki `time` Library — Basic Manual

> **Document versions:**
> - **[EN] English Version** — complete English reference
> - **[PT-PT] Versão Portuguesa** — referência completa em português europeu
>
> Use the version labels and section identifiers below to quickly locate the desired language and topic.

---

# [EN] English Version

The `time` library provides date and time values in Noki through `TimeObject` instances.

A `TimeObject` stores a time value internally as a number of nanoseconds. The library can create a time from the current system time, from another `TimeObject`, from an integer internal value, or from calendar components.

The library provides both functions called through `time` and methods called on a `TimeObject`.

To use the library:

```noki
import time
```

> **[EN-SYNTAX] Assignment and copying:** Noki uses `<-` as the assignment operator. The `=` operator is also permitted, but it performs a copy rather than the faster direct assignment operation represented by `<-`. For strings and large objects, `<-` is generally preferable when the value does not need to be copied.

---

## [EN-01] Assignment Operator `<-` and Copy Operator `=`

When a value is assigned directly to a variable, use `<-`:

```noki
t <- time.now()
year <- t.getyear()
parts <- t.formated()
```

The `=` operator remains valid in Noki, but it has copy semantics. Therefore, it can perform a copy of the value instead of the direct assignment performed by `<-`.

This distinction is especially relevant for strings and large objects, where avoiding an unnecessary copy can be more efficient:

```noki
a <- "texto"
b = a
```

In the example above, `b = a` represents a copy, while `<-` is the preferred operator when direct assignment is intended.

---

## [EN-02] Creating the Current Time with `time.now()`

### Syntax

```noki
time.now()
```

Creates a new `TimeObject` containing the current system time.

The function does not accept arguments.

### Example

```noki
import time

t <- time.now()

println(t.getyear())
println(t.getmonth())
println(t.getday())
println(t.gethour())
println(t.getmin())
println(t.getsec())
```

---

## [EN-03] Reading the Current Real Time Directly

The library also provides functions that return individual components of the current system time without creating a `TimeObject` first:

```noki
time.getrealtimeyear()
time.getrealtimemonth()
time.getrealtimeday()
time.getrealtimemin()
time.getrealtimesec()
```

All of these functions take no arguments.

The currently exposed direct functions provide year, month, day, minute and second. Hour, millisecond and nanosecond components are obtained through a `TimeObject`.

### Example

```noki
year <- time.getrealtimeyear()
month <- time.getrealtimemonth()
day <- time.getrealtimeday()
minute <- time.getrealtimemin()
second <- time.getrealtimesec()

println(year)
println(month)
println(day)
println(minute)
println(second)
```

---

## [EN-04] Reading Components from a `TimeObject`

A `TimeObject` provides these methods:

```noki
t.getyear()
t.getmonth()
t.getday()
t.gethour()
t.getmin()
t.getsec()
t.getms()
t.getns()
```

All of these methods take no additional arguments.

| Method | Meaning |
|---|---|
| `getyear()` | Year |
| `getmonth()` | Month, normally `1` to `12` |
| `getday()` | Day of the month |
| `gethour()` | Hour, `0` to `23` |
| `getmin()` | Minute, `0` to `59` |
| `getsec()` | Second |
| `getms()` | Millisecond component, `0` to `999` |
| `getns()` | Nanosecond remainder inside the current millisecond, `0` to `999999` |

`getns()` does **not** return the total nanoseconds stored by the `TimeObject`. It returns only the nanosecond remainder after the millisecond component.

### Example

```noki
t <- time.now()

year <- t.getyear()
month <- t.getmonth()
day <- t.getday()
hour <- t.gethour()
minute <- t.getmin()
second <- t.getsec()
ms <- t.getms()
ns <- t.getns()

println(year)
println(month)
println(day)
println(hour)
println(minute)
println(second)
println(ms)
println(ns)
```

---

## [EN-05] Setting a Time — `time.set()` and `t.set()`

`set()` is available both as a library function and as a method of a `TimeObject`. These two forms are related but have different purposes:

- `time.set(...)` creates a **new** `TimeObject`.
- `t.set(...)` changes the value of an **existing** `TimeObject`.

Both forms support the same three input styles: another `TimeObject`, an integer internal value, or eight calendar components. They also support the vector form containing those eight calendar components.

### [EN-05-A] From another `TimeObject`

Create a new object:

```noki
t <- time.set(otherTime)
```

Change an existing object:

```noki
t.set(otherTime)
```

The time object's internal nanosecond value is copied from `otherTime`.

### [EN-05-B] From an integer internal time value

Create a new object:

```noki
t <- time.set(value)
```

Change an existing object:

```noki
t.set(value)
```

When the argument is an `int64`, the integer is used **directly as the internal time value in nanoseconds**. The implementation stores that integer without converting it from seconds, milliseconds, or another unit.

This means:

```noki
t <- time.set(0)
```

creates a time object whose internal value is exactly `0` nanoseconds.

### [EN-05-C] From a vector of calendar components

```noki
t <- time.set([year, month, day, hour, min, sec, ms, ns])
```

or:

```noki
t.set([year, month, day, hour, min, sec, ms, ns])
```

The vector must contain exactly eight integer values in this order:

```text
[year, month, day, hour, min, sec, ms, ns]
```

### [EN-05-D] From eight calendar arguments

Create a new object:

```noki
t <- time.set(year, month, day, hour, min, sec, ms, ns)
```

Change an existing object:

```noki
t.set(year, month, day, hour, min, sec, ms, ns)
```

All eight values must be integers.

The implementation checks the following component ranges:

| Component | Accepted range |
|---|---:|
| `month` | `1` – `12` |
| `day` | `1` – `31` |
| `hour` | `0` – `23` |
| `min` | `0` – `59` |
| `sec` | `0` – `60` |
| `ms` | `0` – `999` |
| `ns` | `0` – `999999` |

The `year` value is not subject to a range check in the implementation.

These are component range checks, not a complete calendar-validity check. The underlying `std::mktime()` conversion may normalize calendar combinations such as a day value that does not exist in a particular month.

### Example

```noki
t <- time.set(2026, 9, 7, 14, 30, 15, 250, 500)

println(t.getyear())
println(t.getmonth())
println(t.getday())
println(t.gethour())
println(t.getmin())
println(t.getsec())
println(t.getms())
println(t.getns())
```

---

## [EN-06] Setting Individual Components

The following methods modify one component of an existing `TimeObject`:

```noki
t.setin_ns(value)
t.setin_ms(value)
t.setin_sec(value)
t.setin_min(value)
t.setin_hour(value)
t.setin_day(value)
t.setin_month(value)
```

Each method accepts exactly one integer argument.

### Ranges

| Method | Accepted range |
|---|---:|
| `setin_ns()` | `0` – `999999999` |
| `setin_ms()` | `0` – `999` |
| `setin_sec()` | `0` – `59` |
| `setin_min()` | `0` – `59` |
| `setin_hour()` | `0` – `23` |
| `setin_day()` | `1` – `31` |
| `setin_month()` | `1` – `12` |

`setin_ns()` sets the **complete nanosecond subsecond part**, not only the remainder returned by `getns()`.

For example, a value such as `123456789` is valid for `setin_ns()` and represents 123,456,789 nanoseconds within the current second. By contrast, `getns()` only returns the remainder after the millisecond portion, so its value is limited to `0` – `999999`.

`setin_ms()` changes the millisecond component while preserving the remaining nanoseconds inside that millisecond.

The second, minute, hour, day and month operations rebuild the local calendar representation with the selected component.

As with `set()`, the day check only enforces `1` – `31`; complete calendar validity is handled through `std::mktime()` and may result in normalization.

### Example

```noki
t <- time.now()

t.setin_hour(18)
t.setin_min(45)
t.setin_sec(30)
t.setin_ms(500)
t.setin_ns(123456789)
```

---

## [EN-07] Adding Time

The following methods add an integer amount to the existing `TimeObject`:

```noki
t.addin_ns(value)
t.addin_ms(value)
t.addin_sec(value)
t.addin_min(value)
t.addin_hour(value)
t.addin_day(value)
t.addin_month(value)
t.addin_year(value)
```

Each method accepts exactly one integer argument.

For nanoseconds through days, the implementation adds a fixed number of nanoseconds:

```text
1 ms    = 1,000,000 ns
1 sec   = 1,000,000,000 ns
1 min   = 60 sec
1 hour  = 60 min
1 day   = 24 hours
```

`addin_month()` and `addin_year()` use local calendar arithmetic through `std::tm` and `std::mktime()`. They therefore operate as calendar changes rather than as a fixed number of nanoseconds.

The implementation does not impose a range restriction on the integer passed to the `addin_*()` methods, so negative values can also be used to subtract time.

### Example

```noki
t <- time.now()

t.addin_sec(30)
t.addin_min(5)
t.addin_hour(2)
t.addin_day(1)
t.addin_month(1)
t.addin_year(1)
```

---

## [EN-08] Formatting a Time Value

Use:

```noki
t.formated()
```

The method returns a vector containing eight integer components in this order:

```text
[year, month, day, hour, min, sec, ms, ns]
```

### Example

```noki
t <- time.now()

parts <- t.formated()

println(parts[0])
println(parts[1])
println(parts[2])
println(parts[3])
println(parts[4])
println(parts[5])
println(parts[6])
println(parts[7])
```

The `formated()` name is intentional and matches the current Noki API.

---

## [EN-09] Complete Example

```noki
import time

println("================================")
println(" TIME LIBRARY EXAMPLE")
println("================================")

# Current time
t <- time.now()

println("Year:")
println(t.getyear())

println("Month:")
println(t.getmonth())

println("Day:")
println(t.getday())

println("Hour:")
println(t.gethour())

println("Minute:")
println(t.getmin())

println("Second:")
println(t.getsec())

println("Millisecond:")
println(t.getms())

println("Nanosecond remainder:")
println(t.getns())

# Set the complete calendar value
t.set(2026, 9, 7, 12, 30, 0, 0, 0)

# Add time
t.addin_hour(2)
t.addin_day(1)

# Format as a vector
parts <- t.formated()
println(parts)

println("================================")
println(" END")
println("================================")
```

---

## [EN-10] API Summary

### Library functions

```noki
time.now()
time.set(value)
time.set(timeObject)
time.set([year, month, day, hour, min, sec, ms, ns])
time.set(year, month, day, hour, min, sec, ms, ns)

time.getrealtimeyear()
time.getrealtimemonth()
time.getrealtimeday()
time.getrealtimemin()
time.getrealtimesec()
```

### `TimeObject` methods

```noki
t.getyear()
t.getmonth()
t.getday()
t.gethour()
t.getmin()
t.getsec()
t.getms()
t.getns()

t.set(value)
t.set(timeObject)
t.set([year, month, day, hour, min, sec, ms, ns])
t.set(year, month, day, hour, min, sec, ms, ns)

t.setin_ns(value)
t.setin_ms(value)
t.setin_sec(value)
t.setin_min(value)
t.setin_hour(value)
t.setin_day(value)
t.setin_month(value)

t.addin_ns(value)
t.addin_ms(value)
t.addin_sec(value)
t.addin_min(value)
t.addin_hour(value)
t.addin_day(value)
t.addin_month(value)
t.addin_year(value)

t.formated()
```

### Internal time unit

```text
TimeObject internal value = nanoseconds
```

### Assignment and copy operators

```noki
t <- time.now()    # assignment
copy = t            # copy
```

`<-` is the preferred operator for direct assignment, especially when avoiding copies of strings or large objects is desirable. `=` remains permitted and has copy semantics.

---

# [PT-PT] Versão Portuguesa

A biblioteca `time` fornece valores de data e hora no Noki através de instâncias de `TimeObject`.

Um `TimeObject` armazena internamente um valor temporal em nanossegundos. A biblioteca pode criar um valor temporal a partir da hora atual do sistema, de outro `TimeObject`, de um valor inteiro interno ou dos componentes de calendário.

A biblioteca disponibiliza tanto funções chamadas através de `time` como métodos chamados sobre um `TimeObject`.

Para utilizar a biblioteca:

```noki
import time
```

> **[PT-SINTAXE] Atribuição e cópia:** no Noki, `<-` é o operador de atribuição. O operador `=` também é permitido, mas possui semântica de cópia, ao contrário da atribuição direta feita com `<-`. Em particular, ao trabalhar com strings e objetos grandes, `<-` é normalmente preferível quando não é necessária uma cópia do valor.

---

## [PT-01] Operador de Atribuição `<-` e Operador de Cópia `=`

Quando se pretende atribuir diretamente um valor a uma variável, utiliza-se `<-`:

```noki
t <- time.now()
ano <- t.getyear()
partes <- t.formated()
```

O operador `=` continua a ser permitido no Noki, mas possui semântica de cópia. Assim, pode criar uma cópia do valor em vez da atribuição direta feita por `<-`.

Esta diferença é especialmente relevante para strings e objetos grandes, onde evitar uma cópia desnecessária pode ser mais eficiente:

```noki
a <- "texto"
b = a
```

No exemplo, `b = a` representa uma cópia, enquanto `<-` é o operador preferível quando se pretende uma atribuição direta.

---

## [PT-02] Criar a Hora Atual com `time.now()`

### Sintaxe

```noki
time.now()
```

Cria um novo `TimeObject` contendo a hora atual do sistema.

A função não aceita argumentos.

### Exemplo

```noki
import time

t <- time.now()

println(t.getyear())
println(t.getmonth())
println(t.getday())
println(t.gethour())
println(t.getmin())
println(t.getsec())
```

---

## [PT-03] Obter Diretamente a Hora Real Atual

A biblioteca disponibiliza também funções que devolvem componentes individuais da hora atual do sistema sem criar primeiro um `TimeObject`:

```noki
time.getrealtimeyear()
time.getrealtimemonth()
time.getrealtimeday()
time.getrealtimemin()
time.getrealtimesec()
```

Todas estas funções não aceitam argumentos.

As funções diretas atualmente disponíveis fornecem ano, mês, dia, minuto e segundo. A hora, os milissegundos e os nanossegundos são obtidos através de um `TimeObject`.

### Exemplo

```noki
ano <- time.getrealtimeyear()
mes <- time.getrealtimemonth()
dia <- time.getrealtimeday()
minuto <- time.getrealtimemin()
segundo <- time.getrealtimesec()

println(ano)
println(mes)
println(dia)
println(minuto)
println(segundo)
```

---

## [PT-04] Obter Componentes de um `TimeObject`

Um `TimeObject` disponibiliza os seguintes métodos:

```noki
t.getyear()
t.getmonth()
t.getday()
t.gethour()
t.getmin()
t.getsec()
t.getms()
t.getns()
```

Todos estes métodos não recebem argumentos adicionais.

| Método | Significado |
|---|---|
| `getyear()` | Ano |
| `getmonth()` | Mês, normalmente de `1` a `12` |
| `getday()` | Dia do mês |
| `gethour()` | Hora, de `0` a `23` |
| `getmin()` | Minuto, de `0` a `59` |
| `getsec()` | Segundo |
| `getms()` | Componente de milissegundos, de `0` a `999` |
| `getns()` | Restante de nanossegundos dentro do milissegundo atual, de `0` a `999999` |

`getns()` **não** devolve o total de nanossegundos armazenado no `TimeObject`. Devolve apenas o restante de nanossegundos depois da componente de milissegundos.

### Exemplo

```noki
t <- time.now()

ano <- t.getyear()
mes <- t.getmonth()
dia <- t.getday()
hora <- t.gethour()
minuto <- t.getmin()
segundo <- t.getsec()
ms <- t.getms()
ns <- t.getns()

println(ano)
println(mes)
println(dia)
println(hora)
println(minuto)
println(segundo)
println(ms)
println(ns)
```

---

## [PT-05] Definir um Valor Temporal — `time.set()` e `t.set()`

`set()` está disponível tanto como função da biblioteca como método de um `TimeObject`. As duas formas estão relacionadas, mas têm finalidades diferentes:

- `time.set(...)` cria um **novo** `TimeObject`.
- `t.set(...)` altera o valor de um `TimeObject` **existente**.

As duas formas suportam os mesmos três tipos de entrada: outro `TimeObject`, um valor inteiro interno ou oito componentes de calendário. Também suportam a forma com um vector que contém esses oito componentes.

### [PT-05-A] A partir de outro `TimeObject`

Criar um novo objeto:

```noki
t <- time.set(outroTime)
```

Alterar um objeto existente:

```noki
t.set(outroTime)
```

O valor temporal interno em nanossegundos é copiado de `outroTime`.

### [PT-05-B] A partir de um valor temporal inteiro interno

Criar um novo objeto:

```noki
t <- time.set(valor)
```

Alterar um objeto existente:

```noki
t.set(valor)
```

Quando o argumento é um `int64`, o inteiro é utilizado **diretamente como o valor temporal interno em nanossegundos**. A implementação armazena esse inteiro sem o converter de segundos, milissegundos ou outra unidade.

Isto significa que:

```noki
t <- time.set(0)
```

cria um objeto temporal cujo valor interno é exatamente `0` nanossegundos.

### [PT-05-C] A partir de um vector de componentes de calendário

```noki
t <- time.set([ano, mes, dia, hora, min, sec, ms, ns])
```

ou:

```noki
t.set([ano, mes, dia, hora, min, sec, ms, ns])
```

O vector tem de conter exatamente oito valores inteiros nesta ordem:

```text
[ano, mes, dia, hora, min, sec, ms, ns]
```

### [PT-05-D] A partir de oito argumentos de calendário

Criar um novo objeto:

```noki
t <- time.set(ano, mes, dia, hora, min, sec, ms, ns)
```

Alterar um objeto existente:

```noki
t.set(ano, mes, dia, hora, min, sec, ms, ns)
```

Todos os oito valores têm de ser inteiros.

A implementação verifica os seguintes limites dos componentes:

| Componente | Intervalo aceite |
|---|---:|
| `month` / `mes` | `1` – `12` |
| `day` / `dia` | `1` – `31` |
| `hour` / `hora` | `0` – `23` |
| `min` / `min` | `0` – `59` |
| `sec` / `sec` | `0` – `60` |
| `ms` | `0` – `999` |
| `ns` | `0` – `999999` |

O valor de `year` / `ano` não possui uma verificação de intervalo na implementação.

Estes são limites dos componentes, não uma validação completa do calendário. A conversão através de `std::mktime()` pode normalizar combinações de calendário como um dia que não exista no mês em causa.

### Exemplo

```noki
t <- time.set(2026, 9, 7, 14, 30, 15, 250, 500)

println(t.getyear())
println(t.getmonth())
println(t.getday())
println(t.gethour())
println(t.getmin())
println(t.getsec())
println(t.getms())
println(t.getns())
```

---

## [PT-06] Alterar Componentes Individualmente

Os seguintes métodos alteram um componente de um `TimeObject` existente:

```noki
t.setin_ns(valor)
t.setin_ms(valor)
t.setin_sec(valor)
t.setin_min(valor)
t.setin_hour(valor)
t.setin_day(valor)
t.setin_month(valor)
```

Cada método aceita exatamente um argumento inteiro.

### Intervalos

| Método | Intervalo aceite |
|---|---:|
| `setin_ns()` | `0` – `999999999` |
| `setin_ms()` | `0` – `999` |
| `setin_sec()` | `0` – `60` |
| `setin_min()` | `0` – `59` |
| `setin_hour()` | `0` – `23` |
| `setin_day()` | `1` – `31` |
| `setin_month()` | `1` – `12` |

`setin_ns()` define a **componente completa de nanossegundos dentro do segundo**, e não apenas o restante devolvido por `getns()`.

Por exemplo, `123456789` é um valor válido para `setin_ns()` e representa 123 456 789 nanossegundos dentro do segundo atual. Já `getns()` devolve apenas o restante depois da componente de milissegundos, pelo que o seu valor está limitado a `0` – `999999`.

`setin_ms()` altera a componente de milissegundos preservando os restantes nanossegundos dentro desse milissegundo.

As operações sobre segundo, minuto, hora, dia e mês reconstruem a representação do calendário local com a componente selecionada.

Tal como em `set()`, a verificação do dia limita-se a `1` – `31`; a validade completa do calendário é tratada através de `std::mktime()`, podendo ocorrer normalização.

### Exemplo

```noki
t <- time.now()

t.setin_hour(18)
t.setin_min(45)
t.setin_sec(30)
t.setin_ms(500)
t.setin_ns(123456789)
```

---

## [PT-07] Adicionar Tempo

Os seguintes métodos adicionam uma quantidade inteira ao `TimeObject` existente:

```noki
t.addin_ns(valor)
t.addin_ms(valor)
t.addin_sec(valor)
t.addin_min(valor)
t.addin_hour(valor)
t.addin_day(valor)
t.addin_month(valor)
t.addin_year(valor)
```

Cada método aceita exatamente um argumento inteiro.

Para nanossegundos até dias, a implementação adiciona quantidades fixas de nanossegundos:

```text
1 ms     = 1 000 000 ns
1 seg    = 1 000 000 000 ns
1 min    = 60 seg
1 hora   = 60 min
1 dia    = 24 horas
```

`addin_month()` e `addin_year()` utilizam aritmética de calendário local através de `std::tm` e `std::mktime()`. Por isso, funcionam como alterações de calendário e não como uma quantidade fixa de nanossegundos.

A implementação não impõe um limite de intervalo ao inteiro passado aos métodos `addin_*()`, pelo que também podem ser utilizados valores negativos para subtrair tempo.

### Exemplo

```noki
t <- time.now()

t.addin_sec(30)
t.addin_min(5)
t.addin_hour(2)
t.addin_day(1)
t.addin_month(1)
t.addin_year(1)
```

---

## [PT-08] Formatar um Valor Temporal

Utiliza:

```noki
t.formated()
```

O método devolve um vector com oito componentes inteiras nesta ordem:

```text
[ano, mes, dia, hora, min, sec, ms, ns]
```

### Exemplo

```noki
t <- time.now()

partes <- t.formated()

println(partes[0])
println(partes[1])
println(partes[2])
println(partes[3])
println(partes[4])
println(partes[5])
println(partes[6])
println(partes[7])
```

O nome `formated()` é intencional e corresponde à API atual do Noki.

---

## [PT-09] Exemplo Completo

```noki
import time

println("================================")
println(" EXEMPLO DA BIBLIOTECA TIME")
println("================================")

# Hora atual
t <- time.now()

println("Ano:")
println(t.getyear())

println("Mês:")
println(t.getmonth())

println("Dia:")
println(t.getday())

println("Hora:")
println(t.gethour())

println("Minuto:")
println(t.getmin())

println("Segundo:")
println(t.getsec())

println("Milissegundo:")
println(t.getms())

println("Restante de nanossegundos:")
println(t.getns())

# Definir o valor completo do calendário
t.set(2026, 9, 7, 12, 30, 0, 0, 0)

# Adicionar tempo
t.addin_hour(2)
t.addin_day(1)

# Formatar como vector
partes <- t.formated()
println(partes)

println("================================")
println(" FIM")
println("================================")
```

---

## [PT-10] Resumo da API

### Funções da biblioteca

```noki
time.now()
time.set(valor)
time.set(timeObject)
time.set([ano, mes, dia, hora, min, sec, ms, ns])
time.set(ano, mes, dia, hora, min, sec, ms, ns)

time.getrealtimeyear()
time.getrealtimemonth()
time.getrealtimeday()
time.getrealtimemin()
time.getrealtimesec()
```

### Métodos de `TimeObject`

```noki
t.getyear()
t.getmonth()
t.getday()
t.gethour()
t.getmin()
t.getsec()
t.getms()
t.getns()

t.set(valor)
t.set(timeObject)
t.set([ano, mes, dia, hora, min, sec, ms, ns])
t.set(ano, mes, dia, hora, min, sec, ms, ns)

t.setin_ns(valor)
t.setin_ms(valor)
t.setin_sec(valor)
t.setin_min(valor)
t.setin_hour(valor)
t.setin_day(valor)
t.setin_month(valor)

t.addin_ns(valor)
t.addin_ms(valor)
t.addin_sec(valor)
t.addin_min(valor)
t.addin_hour(valor)
t.addin_day(valor)
t.addin_month(valor)
t.addin_year(valor)

t.formated()
```

### Unidade temporal interna

```text
Valor interno do TimeObject = nanossegundos
```

### Operadores de atribuição e cópia

```noki
t <- time.now()    # atribuição
copia = t          # cópia
```

`<-` é o operador preferido para atribuição direta, especialmente quando se pretende evitar cópias de strings ou objetos grandes. `=` continua a ser permitido e possui semântica de cópia.
