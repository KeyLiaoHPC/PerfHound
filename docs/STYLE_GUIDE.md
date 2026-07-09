# PerfHound C Code Style Guide

This document defines the coding style conventions for the PerfHound (盘瓠) toolkit.
Follow these guidelines for consistency across probe, utils, examples, and related C sources.

Existing files still contain legacy headers, `vt_` / `varapi` / `pfh_*` names, and decorative
comment borders. **New and edited code should follow this guide**; do not expand legacy patterns.

## 1. Function Naming Conventions

### 1.1. Public Functions

**Public APIs for the serial probe** (`src/probe/include/perfhound.h`, implemented in `perfhound.c`):

- Use the `ph_` prefix. E.g., `ph_init()`, `ph_set_evt()`, `ph_read()`, `ph_finalize()`.

**Public APIs for the MPI probe** (`src/probe/include/ph_mpi.h`, implemented in `ph_mpi.c`):

- Use the `phmpi_` prefix with the same verb/noun as the serial API. E.g., `phmpi_init()`,
  `phmpi_read()`, `phmpi_finalize()`.

**I/O helpers** (used by probe, declared/defined around `ph_io.c`):

- Use `ph_io_<verb>[_<noun>]`. E.g., `ph_io_mkdir()`, `ph_io_wtrec()`, `ph_io_wtctag()`.

**Legacy names** (`pfh_*`, `vt_*`, `varapi_*`) in older examples and comments are deprecated. Use
`ph_*` / `phmpi_*` for all new code; when touching call sites, migrate names if the linked
library already exports `ph_*`.

### 1.2. Module Internal Functions

- Single-file static functions: use a short verb-noun name, optionally prefixed with `_sf_` for
  helpers that are clearly file-local. E.g., `_sf_min()`, or plain `init_buffer()` when already
  `static` and unambiguous.
- Cross-file internal symbols that are not part of the public headers: keep the `ph_` / `ph_io_`
  / `phmpi_` family, and do **not** put them in `perfhound.h` / `ph_mpi.h`.

### 1.3. Backend Macros (PMU / timers)

Low-level arch and PAPI backends live in `ph_pm_*.h` and use the `_ph_` macro prefix:

- `_ph_init_ts`, `_ph_fini_ts`, `_ph_init_cy`
- `_ph_read_cy`, `_ph_read_ns`
- `_ph_parse_event`, `_ph_config_event`
- `_ph_read_pm_<n>`, `_ph_read_pm_ev`, `_ph_read_pm_evx`

Do not expose these macros through public headers. Call them only from probe implementation files.

### 1.4. Polymorphism / Mode Variants

Prefer compile-time modes (`PH_OPT_TS`, `PH_OPT_EV`, `PH_OPT_EVX`, `PH_OPT_PAPI`, `PH_OPT_MPI`)
over runtime branching on the hot path. When a family of helpers is needed, use a clear suffix:
`_ph_read_pm_1` … `_ph_read_pm_12`, or `ph_fastread` vs `ph_read`.

## 2. Function

### 2.1. Declaration

- Declare public interfaces in header files (`*.h`).
- Declare prototypes of all local static functions at the beginning of the file, in alphabet
  ascending order.
- If a prototype exceeds the line limit, use one-line-per-parameter style and align to the first
  parameter.
- No space between the function name and `(`.

**Examples**

```c
/* Local Function Prototypes */
static void flush_buffer(void);
static int format_path(char *buf, size_t len, const char *root);
```

### 2.2. Function Implementation

- Put the function name on its own line, after the return type.
- If parameters exceed the line limit, wrap with one parameter per line, aligned to the first.

```c
<return_type>
<function_name>(<parameters>)
{
    <body>
}
```

### 2.3. Function Documentation

**General Rules**

- All public functions require Doxygen documentation.
- Document structure: 1) What it does; 2) Inputs; 3) Outputs/Effects; 4) Edge cases (if applicable).

**Public APIs (in headers)**

- Place Doxygen comments before the prototype.
- Include `@brief`, `@param`, `@return` (if applicable).
- Before implementation in `.c` files, repeat the `@brief` line only.

Example in header:

```c
/**
 * @brief Initialise PH-Probe. Call exactly once before collecting samples.
 * @param path Root directory for output data (NULL → ./ph_data)
 * @return 0 on success, non-zero on failure
 */
int ph_init(char *path);
```

Example in `.c` file:

```c
/**
 * @brief Initialise PH-Probe. Call exactly once before collecting samples.
 */
int
ph_init(char *path)
{
    if (path == NULL) {
        /* default data root */
    }
    /* ... */
}
```

**Static Functions**

- Use plain `/* */` block comments before the prototype.
- No repetition needed before implementation.

```c
/*
 * Create host subdirectory under the current run directory.
 * Returns 0 on success, errno-style code on failure.
 */
static int ensure_host_dir(void);
```

### 2.4. Function Calls

**Basic format:**

```c
result = function_name(arg1, arg2, arg3);
```

**Wrapped format** (exceeds 85 columns):

```c
result = long_function_name(first_argument,
                            second_argument,
                            third_argument,
                            fourth_argument);
```

## 3. Struct Member Documentation

### Format

Use Doxygen's `/**<` inline style after each member:

```c
typedef struct {
    uint32_t ctag[2]; /**< Group id and point id */
    int64_t cy;       /**< Cycle counter */
    int64_t ns;       /**< Wall-clock nanoseconds */
    double uval;      /**< User-defined FP64 payload */
} rec_t;
```

### Guidelines

- Place `/**<` comment immediately after the member
- Provide brief, descriptive explanations
- Align comments across members when practical

## 4. Comment Styles

### Block Comments

Use `/* */` for code block comments, even for single-line comments:

```c
/* Initialise the timestamp backend */
_ph_init_ts;

/* This is a multi-line comment explaining
   complex logic in the following code block */
```

### Inline Comments

Use `//` for line/variable comments:

```c
int ph_irec = 0;  // index of next free record slot
```

### Long Inline Comments

If an inline comment would exceed column 85, place it on the line before:

```c
/* Only dump when one empty slot remains to avoid recursion in ph_read */
if (ph_irec + 1 - buf_nrec == 0) {
    ph_dump();
}
```

### Prohibited Styles

- Do NOT use `===` or `---` splitter lines in comments
- Do NOT use decorative comment borders

```c
/* Wrong - don't do this */
/* ============================================== */
/* Section Header                                  */
/* ============================================== */

/* Correct */
/* Section Header */
```

## 5. Line Length

### General Rule

Keep lines within 85 columns.

### Exceptions

The following may exceed 85 columns:

- Contiguous macro / inline asm sequences in `ph_pm_*.h`
- Meaningful calculations
- String literals (especially `*** [PH-Probe]` messages)

```c
/* These lines may exceed 85 columns */
#define _ph_read_cy(_cy)  asm volatile("mrs %0, pmccntr_el0" "\n\t": "=r" (_cy)::);
```

## 6. Alignment of Block Macros and Multi-line Comments

### Rule

Always align the start of key definitions of block macros and multi-line consecutive comments to
the next tab stop after the longest column of the line block.

### Block Macros

For blocks of related `#define` statements, align the macro names (or values) to the next tab
stop after the longest line in the block:

```c
/* Correct - aligned to next tab after longest line */
#define _HOST_MAX       253
#define _PROJ_MAX       64
#define _TAG_MAX        16
#define _NCTAG_MAX      32
#define _NETAG_MAX      8
```

### Multi-line Consecutive Comments

For consecutive multi-line comments, align the comment text to the next tab stop after the
longest comment marker:

```c
/* Correct - aligned to next tab after longest marker */
/* Initialise timer subsystem */
/* Set default timeout value */
/* Configure callback handlers */
```

## 7. File Headers

### Format

Each file should have a minimal header with filename and description.

For implementation files (`*.c`):

```c
/*
 * perfhound.c
 * Serial PH-Probe API: init, read, dump, finalize.
 */
```

For header files (`*.h`) — include Doxygen tags:

```c
/**
 * @file perfhound.h
 * @brief Public serial API for PerfHound PH-Probe.
 */

#ifndef PERFHOUND_H
#define PERFHOUND_H

/* ... content ... */

#endif /* PERFHOUND_H */
```

### Existing GPL banners

Many sources already carry a full GPL copyright block. When editing those files, leave the
existing license banner intact. For **new** files, use the minimal header above unless the user
asks to attach a license text.

### Prohibited (for new headers)

- Do NOT invent decorative `====` frames around the file header
- Do NOT add author/date/email history blocks to new files
- Do NOT paste obsolete filenames (e.g. `varapi.c`) into new headers

## 8. Control Structures

### Format

Use space between keyword and parenthesis, space before opening brace:

```c
if (condition) {
    /* body */
} else {
    /* alternative */
}

for (int i = 0; i < n; i++) {
    /* body */
}

while (condition) {
    /* body */
}
```

### Brace Placement

- Opening brace on the same line as the statement
- Closing brace on its own line
- `else` on the same line as the closing brace

## 9. Include Guards

All header files must have include guards:

```c
#ifndef PH_CORE_H
#define PH_CORE_H

/* content */

#endif /* PH_CORE_H */
```

Prefer `PH_<NAME>_H` / `PERFHOUND_H` style. Legacy `__VARAPI_CORE_H__` guards may remain until
the header is deliberately renamed; do not introduce new double-underscore guard names.

## 10. Indentation

- Use 4 spaces for indentation
- Do NOT use tabs
- Align continuation lines appropriately

## 11. Section Comments

Use simple `/* Section Name */` comments to delineate code sections:

```c
/* Local Function Prototypes */

/* Public API */

/* Utility Functions */
```

## 12. Error Handling

PerfHound does not use layered module/cause error codes. Conventions:

### Setup and I/O paths

- Prefer `int` return codes: `0` success, non-zero failure (often `errno` or a small local code).
- Log with a consistent prefix: `*** [PH-Probe] ...`, then `fflush(stdout)` when the message must
  appear before a possible abort.
- Existing code may call `exit(1)` on fatal init/I/O failures. Prefer returning errors from new
  helpers when the caller can handle them; do not add new `exit()` calls on the hot path.

```c
err = ph_io_mkfile();
if (err) {
    printf("*** [PH-Probe] EXIT %d. Failed to create files.\n", err);
    fflush(stdout);
    return err;  /* preferred for new code when caller can abort */
}
```

### Hot path (`ph_read` / `ph_fastread` / `phmpi_read`)

- No heap allocation, no `printf`, no file open except via the intentional buffer-full → `ph_dump`
  path.
- Keep mode-specific PMU reads behind `PH_OPT_EV` / `PH_OPT_EVX` compile switches.

### MPI vs serial

- Mirror error messages and recovery behavior between `perfhound.c` and `ph_mpi.c` unless the
  difference is MPI-specific (rank, collective, IO group).

### Comparisons

```c
if (err != 0) { /* handle */ }
if (ph_ready) { /* config already committed */ }
```

---

## Quick Reference

| Element           | Style                                   |
| ----------------- | --------------------------------------- |
| Public serial API | `ph_<name>()`                         |
| Public MPI API    | `phmpi_<name>()`                      |
| I/O helpers       | `ph_io_<name>()`                      |
| Backend macros    | `_ph_<name>`                          |
| Static functions  | `<verb>_<noun>()` or `_sf_<name>()` |
| Block comments    | `/* comment */`                       |
| Inline comments   | `// comment`                          |
| Public API docs   | `/** @brief ... */`                   |
| Static func docs  | `/* description */`                   |
| File header (.c)  | `/* filename\n * description */`      |
| File header (.h)  | `/** @file ... @brief ... */`         |
| Line limit        | 85 columns (exceptions for macros/asm)  |
| Indentation       | 4 spaces                                |
| Log prefix        | `*** [PH-Probe]`                      |
