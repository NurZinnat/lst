# Отчёт о программе `fem_lsq`

## Назначение

Консольная программа строит кусочно-линейное приближение функции двух переменных методом КЭ степени 1 и МНК. Решение СЛАУ — метод минимальных невязок с предобуславливателем Якоби (задача **4**). Параллельная модель — `pthread`, общая память.

## Сборка

```bash
cd my
make          # релиз: fem_lsq, -O3, строгие флаги
make debug    # отладка: fem_lsq_debug, -O0
make clean
```

Флаги компиляции совпадают с эталоном `my_code_style` (`FLAGS`, `OPT`, `-Werror`, `-pedantic`).

## Запуск

```text
./fem_lsq a b c d nx ny k eps maxit p
```

## Архитектура (как в `my_code_style`)

```text
main.cpp
  └── implementation::GO()
        ├── init_implementation()      — разбор argv
        ├── memory_allocate_implementation() — единственное выделение памяти (класс data)
        ├── thread_object[0].GO()      — главный поток
        └── pthread + thread_object[1..p-1]
              └── thread_object::GO()
                    ├── worker_build_system()  — матрица, RHS, решение
                    └── worker_compute_errors() — r1..r4
```

| Файл | Роль |
|------|------|
| `main.cpp` | Только `implementation IMP; IMP.GO(argc, argv);` |
| `implementation.h/cpp` | Параметры задачи, массивы `data`, потоки, итоговый `printf` |
| `thread_object.h/cpp` | Наследник `fem_core`, работа одного потока |
| `fem_core.h/cpp` | Сборка системы и вычисление погрешностей (аналог `application`) |
| `data.h/cpp` | Буфер `double*` + `memory_allocate` / `delete[]` |
| `synch.h/cpp` | Барьеры и редукции (`synchronization_*`) |
| `execution_status.h` | Коды возврата |
| `domain`, `mesh_index`, `msr_matrix`, `rhs_builder`, `solver_mr`, … | Численные модули |

## Память

Выделяется **только** в `implementation::memory_allocate_implementation()` через `data::memory_allocate`. Рабочие функции получают готовые указатели. `std::vector` и `std::unique_ptr` не используются.

## Стиль кода

- Классы в `lower_case` (`implementation`, `thread_object`, `fem_core`, `data`).
- Без `namespace`.
- Include guard: `#endif // HEADER_H`.
- Тип возврата функции на отдельной строке (где применимо).
- Запрет копирования: `= delete` для copy/move.
- Синхронизация — статические функции в `synch.cpp` (как в эталоне).

## Формат вывода

```text
имя : Task = 4 R1 = ... R2 = ... R3 = ... R4 = ... T1 = ... T2 = ... It = ... E = ... K = ... Nx = ... Ny = ... P = ...
```
