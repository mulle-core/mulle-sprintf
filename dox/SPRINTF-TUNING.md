# mulle-sprintf Tuning notes

A practical guide to measuring `mulle-sprintf` speed against the system C
library (`glibc` here) and to reading the `perf` profile to see where the time
actually goes.

This project is a library; you can't run it by itself. All timing is done via a
small benchmark executable placed in the test harness. The benchmark lives at:

```
test/benchmark/mulle_vs_stdio.c
```

It compares, case by case, `mulle_snprintf` against the system `snprintf`
(glibc), prints the ratio `mulle_speed / glibc_speed` (lower is better, `< 1.0`
means mulle is faster), and returns 0.

---

## 1. Build configuration caveat: Release vs Debug

This was the single biggest source of confusion. Two traps:

1. **`--release` only counts if the benchmark's `.exe` actually links the
   Release library.** The harness keeps separate `Debug` and `Release` build
   trees under
   `$HOME/.mulle/var/cache/sde/mulle-sprintf-<hash>/dependency/{Debug,lib}/`.
   Before trusting numbers, confirm which library the `.exe` resolves:
   ```console
   ldd test/benchmark/mulle_vs_stdio.exe | grep mulle-sprintf
   ```
   You want `.../dependency/lib/libmulle-sprintf.so...` (Release), **not**
   `.../dependency/Debug/lib/...`. I once "optimized" the code, rebuilt with
   `--release`, and my before/after numbers were byte-identical because my
   crafts had silently rebuilt the *Release* tree while the benchmark was
   still loading the untouched *Debug* library.

2. **`-O0` (Debug) makes mulle-sprintf look ~8x slower than glibc.** glibc is a
   prebuilt optimized library, so its cost barely moves between build modes;
   mulle-sprintf is compiled from source, so Debug `-O0` tanked it to ratios
   like 8.6x that were purely a build-mode artifact. Always benchmark
   Release (`--release`) for meaningful numbers.

---

## 2. Running the benchmark

Rebuild and run (fresh `.exe`):

```console
mulle-sde test --platform linux craft --release
mulle-sde test --platform linux run --release --keep-exe test/benchmark/mulle_vs_stdio.c
```

Notes:

- `--platform linux` avoids the Windows cross-build / wine.
- `--keep-exe` leaves `test/benchmark/mulle_vs_stdio.exe` behind. Without it the
  harness can leave a stale `.exe` around, and you will benchmark old code.
- The run prints only the final pass (once the loop has scaled up to the time
  budget) to `stdout`. That output is also saved to
  `test/benchmark/mulle_vs_stdio.tmp.stdout`.

The exe takes positional args: `iterations  budget  caseIndex`

| arg | default | meaning |
|-----|---------|---------|
| `iterations` | `100000` | starting loop count, doubled each pass |
| `budget` | `1.0` | seconds; run passes until one pass reaches it |
| `caseIndex` | `-1` (all) | run a single case only (0..6) |

Case index table:

| idx | case | conversions |
|-----|------|-------------|
| 0 | `int %d` | 1 |
| 1 | `int %d x3` | 3 |
| 2 | `int %d x16` | 16 |
| 3 | `mixed 8 convs` | 8 |
| 4 | `float %.6f x3` | 3 |
| 5 | `string %s + int` | 2 |
| 6 | `string %s x8` | 8 |

Benchmark a single case (e.g. `%d`) with a `1s` budget:

```console
./mulle_vs_stdio.exe  100000  1  0
```

Example output:

```
int %d    conv= 1  mulle=   30.466ms  glibc=   16.128ms  ratio=  1.89x
```

The numbers are run-to-run noisy (single process, one in-process measurement
each pass). Take ratios as ~±0.1 and rely on trends, not absolute one-shot
values.

---

## 3. Profiling with perf

`perf` is available on this Linux box. Run the single-case benchmark under
`perf record`, then look at **self-only** percentages.

```console
cd test/benchmark
perf record -o /tmp/mulle-sprintf-perf.data ./mulle_vs_stdio.exe 100000 1 0 >/dev/null 2>&1
perf report -i /tmp/mulle-sprintf-perf.data --stdio --sort=symbol --no-children
```

Critical:

- Use `--no-children`. Without it, `perf report` prints *cumulative* time
  (self + all callees), and functions like `mulle_vsnprintf` look like 7% when
  most of that is work it *calls* into. Only `--no-children` gives self time
  that adds up.
- The benchmark measures glibc and mulle in the same run, so the table mixes
  both. Filter per side with grep, e.g.:
  ```console
  perf report -i /tmp/mulle-sprintf-perf.data --stdio --sort=symbol --no-children 2>/dev/null \
      | grep -iE "mulle|convert_decimal|justified"
  ```

Useful: `--call-graph` for history, `perf stat` for wall counts.

---

## 4. Where the time goes (measured, Release, single `%d`)

Self-only shares of samples (mulle side):

| function | self % | role |
|----------|--------|------|
| `setup_context` | ~13% | format scan + inline `parse_conversion_info` state machine |
| `convert_decimal_unsigned_int` | ~4.6% | writing the decimal digits |
| `_mulle_buffer_vsprintf` | ~3.7% | per-conversion dispatch |
| `_mulle_sprintf_int_decimal_conversion` | ~3.6% | conversion wrapper |
| `mulle_vsnprintf` | ~3.3% | snprintf entry/setup |
| `_mulle__buffer_make_string` | ~2.4% | writes the terminating NUL |
| `mulle_vsprintf_set_values` | ~2.3% | vararg unpack |
| `_mulle_sprintf_justified*` | ~2-3% | width/precision padding logic |

`glibc` (for contrast) spends its time in its own machinery:
`__printf_buffer` (~11%), `_itoa_word` (~4.5%), `__printf_buffer_write` (~4%).

Interpretation:

- **Format scanning/parsing is the #1 cost**, not digit conversion. The state
  machine is the single biggest mulle-side hotspot.
- **The gap widens as conversions *decrease***. Single `%d` worse than 16×`%d`.
  Fixed per-call overhead dominates; throughput at many conversions is near
  parity.
- **glibc's own per-call overhead is comparable** to mulle's; the single-`%d`
  ratio is largely an overhead arms race.

---

## 5. What was tried and its effect

Changes are in the working tree (not committed unless you commit them).

### Single-pass format parse (general) — KEEP, modest win

`src/mulle-sprintf.c`

Replaced the two-stage parse in `setup_context`:

- old: `number_of_conversions()` scanned the whole format to *count* n, then a
  separate loop called `parse_conversion_info()` on each specifier again.
- new: a single `parse_all_conversions()` counts *and* parses in one forward
  pass, advancing the cursor by `info->length` after each conversion. Stack
  buffers for <= 16 conversions, migrated to the existing reusable
  thread-local storage beyond that.

Effect (Release, single run): `%d` 1.89→1.81, 16×`%d` 1.32→1.28,
`%s`+int 1.74→1.61, 8×`%s` 1.67→1.58. The string-heavy cases gained most
because they contain many literal bytes that used to be scanned twice and are
now walked once. `setup_context` is now the single consolidated hotspot.

Validation: `mulle-sde test --platform linux run --release` (all suites) passes.

### Justified/`%d` fast path (general, cheap) — KEEP

`src/functions/mulle-sprintf-integer.c` (`_mulle_sprintf_justified`, ~line 180)

For the common case (no width, no precision padding, no sign/hash prefix,
`precision <= p_length`) it now short-circuits straight to
`mulle_buffer_add_bytes` instead of walking the full padding logic. Halved
`_mulle_sprintf_justified` self time (~4% → ~1.9%) and moved 16×`%d` from 1.44x
→ 1.32x.

### Two-digits-at-once lookup table (decimal) — KEPT DISABLED via `#if 0`

`src/functions/mulle-sprintf-integer.c` (~line 73)

A 200-byte `digits[]` table + `%100`/`/100` to write two digits per division.
It tests *worse* than the naive `%10` loop at `-O0` (Debug), and at `-O3` the
compiler already optimizes the naive `%10`/`/10` loop so well that the table
bought nothing. The digit loop is not the bottleneck. Flip the two `#if 0`
blocks to `#if 1` to experiment.

### `memchr` literal skip — REVERTED

Replacing the literal `while(*p && *p!='%')` scan with `memchr` did not help
and slightly hurt multi-conversion short formats (per-call overhead). The
cost is parsing, not scanning. Reverted.

### Static prefix fast path — KEEP

`src/mulle-sprintf.c` (`copy_static_prefix` + `scan_static_prefix` +
`scan_static_prefix_bounded`)

The front of the format string — literal bytes, plus `%%` escapes while `%`
is still registered as the standard escape function — is copied directly
into the buffer before any parsing. Only the remainder (starting at the
first real conversion) goes through `setup_context` and friends. When the
whole format is static, no conversion machinery runs at all.

- The scanners are pure string functions (no buffer access). The first scan
  is byte-wise and returns both the total string length and the prefix
  length up to the first `%`. The known length then makes later scans of
  the remainder safe for SWAR word-at-a-time reads (see next section).
- `before` (the buffer length used by `%n` and the return value) is
  captured *before* the prefix copy, so both stay correct with the prefix
  counted. Verified by `test/misc/staticprefix.c`.
- Effect (Release): `long lit + %d` went 4.58x → 3.26x (14.1ms → 11.0ms),
  everything else unchanged.

A micro-benchmark of the remaining literal-heavy cost:

| case | mulle | glibc |
|------|-------|-------|
| `lit %` (11 chars + `%%`) | 79.5ms | 48.8ms |
| `lit long %` (118 chars + `%%`) | 239ms | 57.6ms |
| `d only` | 129.7ms | 76.8ms |

mulle is ~40ns fixed + ~0.65ns/byte for literal text; glibc is ~29ns flat
(memchr/memcpy based). The byte-wise first scan is most of the per-byte
cost, and the fixed part is the `mulle_vsnprintf` → buffer-init →
`make_string` call chain.

### SWAR literal scan — the length problem (finding)

The classic word-at-a-time scan (the disabled `find_next_percent`) reads up
to 7 bytes past the NUL. That is not valgrind-clean (flagged for heap
strings that end at an allocation boundary) and unaligned 8-byte loads can
even cross a page boundary into unmapped memory. In principle,
word-at-a-time scanning of a NUL-terminated string of unknown length can
*never* avoid this: loading 8 bytes requires knowing 8 bytes are valid.

Hence the split design above: the first scan is byte-wise (no length known
yet), and every later scan is bounded by the known remaining length
(`scan_static_prefix_bounded`), so full words are only read while they are
entirely within the string — valgrind-clean, no overread. This only
matters for formats with several `%%` escapes; the common case does one
byte-wise pass.

---

## 6. Remaining levers (if you escalate further)

The single biggest mulle cost is `setup_context` (~13% self, the parser). Ideas
in rough order of expected ROI:

1. Cheap shortcut for the ubiquitous single `%d`/`%s` argument: in
   `setup_context`, skip `determine_all_conversion_argument_types()` +
   `argc`/`max_arg` bookkeeping when it reduces to one simple conversion.
2. Trim `mulle_vsnprintf` + `_mulle__buffer_make_string` fixed call overhead.
3. Only if throughput (many conversions) ultimately matters: touch the digit
   loop — but that's already at parity, so it won't show on single `%d`.

Reality check: the per-call overhead of mulle and glibc are now roughly equal,
and the multi-conversion cases are at/near parity. Expect diminishing returns;
single-`%d` grinding is an overhead race, not an algorithmic one.

---

## 7. Command cheat sheet

```console
# build + fresh benchmark exe
mulle-sde test --platform linux craft --release
mulle-sde test --platform linux run --release --keep-exe test/benchmark/mulle_vs_stdio.c

# confirm which library the exe loads (want: .../dependency/lib/...)
ldd test/benchmark/mulle_vs_stdio.exe | grep mulle-sprintf

# run all cases (final pass to stdout), or a single case
./mulle_vs_stdio.exe  100000  1  -1
./mulle_vs_stdio.exe  100000  1   0    # int %d only

# perf the single %d case (self-only percentages)
cd test/benchmark
perf record -o /tmp/p.data ./mulle_vs_stdio.exe 100000 1 0 >/dev/null 2>&1
perf report -i /tmp/p.data --stdio --sort=symbol --no-children

# validate correctness after touching src/
mulle-sde test --platform linux run --release
```

----

> ## Proposal: a generic “common lane” with the existing engine as fallback

The best remaining approach is not another %d-specific optimization. It is to recognize ordinary sequential formats early and remove bookkeeping they do not need, while preserving the
current table-driven callbacks for every conversion.

### Architectural invariants

The optimization should preserve:

- Runtime registration of conversion functions and modifiers.
- Custom conversion tables passed to _mulle_buffer_vsprintf.
- Existing callback signatures and public structs.
- Positional parameters, * width/precision, mulle_vararg, %n, UTF conversions, and error behavior.
- No hard-coded fast implementations for %d, %s, etc.

A custom %@ should receive exactly the same acceleration as %d when its syntax is simple.

## Phase 1: low-risk fixed-overhead cleanup

1. Return the format-end pointer from parse_all_conversions.
  It already reaches the terminating NUL. Save that pointer in the context so:
   - literal-only formats do not call strlen and scan twice;
   - context_print emits the trailing literal using a known length instead of mulle_buffer_add_string.

2. Fix capacities around STACKABLE_N.
  There are 16 conversion slots but only 16 argument slots, including unused argument zero. Consequently, exactly 16 arguments spill into TLS storage. Use:
   - 16 conversion slots;
   - 17 argument value/type slots.

3. Grow overflow storage geometrically.
  parse_all_conversions currently increases cap by one after 16 conversions, causing repeated calls into TLS storage and potentially repeated reallocations. Grow to 32, 64, etc.

4. Specialize final NUL termination for mulle_vsnprintf.
  Initialize the inflexible buffer with size - 1 usable output bytes, then terminate directly at the resulting length. This could remove the measured _mulle__buffer_make_string cost,
provided boundary tests confirm identical overflow semantics.

Expected effect: roughly 2–5% overall, more for literal-only calls and formats around 16 conversions.

## Phase 2: generic simple-sequential lane

Classify a complete format as simple sequential when it has:

- no positional $ arguments;
- no argument-derived * width or precision;
- no unsupported vector modifier;
- otherwise any flags, constant width, constant precision, registered modifier, and registered conversion.

While parsing such a format:

1. Resolve the conversion through table->jumps, as today.
2. Invoke that conversion’s determine_argument_type callback immediately after its info is complete.
3. Place the resulting type directly in the sequential argument-type array.
4. Record that no second argument-resolution pass is needed.

Then skip determine_all_conversion_argument_types() entirely.

If any complex feature appears later in the format, abandon the provisional classification and use the existing full argument-resolution pass. Thus formats such as %2$*3$d remain
entirely on the current path.

### Fast printing

For a simple-sequential context, context_print also knows that each conversion consumes exactly the next argument. It can therefore avoid, per conversion:

- checking positional indices;
- update_width_and_precision;
- three checks for a v modifier;
- argument-index rewrites.

It still calls the dynamically registered convert_argument callback. Only the argument routing becomes cheaper.

This should help both one-conversion calls and multi-conversion throughput without weakening extensibility.

## Phase 3: parser fast lanes

### 3.1 Direct two-character conversion

Before entering the state machine, recognize:

text
%X


when X is a registered conversion and is not a registered modifier. Construct a zeroed formatconversioninfo with:

- conversion = X;
- length = 2;
- no flags, width, precision, or modifier.

This is completely generic:

- %d benefits;
- %s benefits;
- a registered %@ benefits;
- anything ambiguous falls back to the full parser.

This directly targets the dominant benchmark case without baking built-in semantics into the core.

### 3.2 Linear parser for ordinary specifications

Add a straightforward parser for:

text
% [flags] [constant width] [.constant precision] [modifiers] conversion


It should consult the runtime modifier and conversion tables exactly as the current parser does. On seeing $, *, separators, or another uncommon construct, restart at the original %
using the existing state machine.

This avoids repeatedly executing the large switch(parser.state) for ordinary formats while leaving the complicated, well-tested grammar untouched.

### 3.3 Parse integers in place

Replace positive_int_value_from_memo’s:

- copy into a 64-byte temporary;
- NUL termination;
- atoi;

with checked digit accumulation during parsing. That mainly improves width, precision, and positional formats rather than plain %d.

Expected effect from Phases 2 and 3: reduce setup_context self-time by perhaps 35–60%, translating to approximately 8–15% total improvement for simple one-conversion calls.

## What I would not do

- **No more digit lookup-table work.** The profile shows decimal conversion is not the limiting factor.
- **No built-in %d or %s shortcut in the core.** That would undermine extension symmetry.
- **No automatic format-pointer cache.** Format strings may be mutable, conversion registrations may change, and validating cached text can consume much of the saved work.
- **No public “compiled format” API yet.** It could be fast, but it introduces a second usage model and arguably changes the architecture. Consider it only after exhausting internal
optimizations.
- **No large rewrite of the full parser initially.** A generic common lane plus exact fallback has a much safer correctness profile.

## Implementation sequence and gates

I would do this as independent, benchmarkable patches:

1. Capacity growth, stored format end, and 17 stack argument slots.
2. Direct %X parser shortcut.
3. Simple-sequential type-resolution and print lane.
4. Ordinary linear parser with complex fallback.
5. Specialized snprintf termination.

For each patch:

- Run the full Release test suite.
- Differentially test common and complex formats against the forced old path.
- Include custom conversions and modifiers in the differential tests.
- Exercise positional arguments, indexed stars, %n, BOOL, UTF-16/32, truncation, and malformed formats.
- Benchmark pinned to one CPU with multiple runs and medians.
- Record cycles, instructions, branches, and branch-misses, not just wall time.
- Reject a patch if any existing benchmark case regresses consistently by more than about 2%.

A reasonable target is moving single %d from roughly 1.81× glibc toward 1.5–1.6×, with perhaps 5–10% improvement in multi-conversion cases. Reaching parity for single %d is unlikely
without format caching or a less extensible interface.

One repository detail: the current git diff shows the single-pass parser as the uncommitted tuning change in src/mulle-sprintf.c. I would use that exact state as the baseline and keep
each additional optimization isolated so noisy benchmark results remain attributable.



----


> Yes. Looking at the updated code, the next macro-scale step is to turn the parsed context into an execution plan. Don’t merely merge setup_context() and context_print() physically;
compute all format-invariant decisions once, then make execution almost mechanical.

## Work still duplicated now

The new cached info->function is useful, but the pipeline still repeats several decisions:

| Decision | Where it happens now |
|---|---|
| Conversion validity/function lookup | determine_is_valid_conversion_character, then functions_for_conversion again |
| Effective argument positions | parse_conversion_info, determine_all_conversion_argument_types, then convert_argument |
| Width/precision argument routing | Parse, type determination, then update_width_and_precision |
| Conversion argument type | Separate pass after parsing |
| Literal boundaries | Stored as starts, then lengths recalculated during printing |

Also, table is now effectively dead in both determine_argument_type() and jump_convert_argument() because they use info->function. Consequently, context_print(..., table) no longer
really needs the table.

## Recommended model: prepare → load values → execute

Define a private prepared conversion—not necessarily public API:

c
struct mulle_sprintf_prepared_conversion
{
   struct mulle_sprintf_formatconversioninfo   info;
   struct mulle_sprintf_function               *function;

   int   literal_offset;
   int   literal_length;

   int   value_argument;
   int   width_argument;       // 0 when not from '*'
   int   precision_argument;   // 0 when not from '*'

   mulle_sprintf_argumenttype_t   value_type;
};


And a format plan:

c
struct mulle_sprintf_plan
{
   int   n;
   int   argc;
   int   format_length;

   struct mulle_sprintf_prepared_conversion   *conversions;
   mulle_sprintf_argumenttype_t                *argument_types;
};


The precise representation can use the existing stack/TLS arrays. The important part is that the plan contains every decision that depends only on the format and conversion table.

## 1. Normalize argument indices during parsing

parse_conversion_info() already knows exactly when an argument is consumed. At those points, record the effective indices:

- value_argument
- width_argument
- precision_argument

Do this for sequential and positional arguments alike.

For example:

text
"%*.*d"
 width_argument     = 1
 precision_argument = 2
 value_argument     = 3


text
"%3$*1$.*2$d"
 width_argument     = 1
 precision_argument = 2
 value_argument     = 3


The memory flags still preserve whether an index was explicitly written, but execution no longer has to reconstruct it.

That lets you delete the mutable arg walk from:

- determine_all_conversion_argument_types
- convert_argument
- context_print

This is the largest remaining duplicated bookkeeping.

You could normalize the existing argv_index[3] fields to always contain effective indices, but a private prepared record is cleaner and avoids changing the meaning extensions may
expect from those fields.

## 2. Determine each conversion type during preparation

Once parsing resolves function, immediately call:

c
type = (*function->determine_argument_type)( &info);


Store that type in the prepared conversion.

After the entire format is parsed and argc is known:

1. Initialize the argument-type array to int for positional gaps and star arguments.
2. Scatter each prepared conversion’s type directly into:

c
argument_types[ conversion->value_argument]


This changes determine_all_conversion_argument_types() from a second semantic interpretation of the format into, at worst, a trivial scatter loop. Better yet, build the stack type
array during parsing and migrate it if the argument count exceeds stack capacity.

The key is that determine_argument_type should run once per preparation, not during a second walk that recalculates argument routing.

## 3. Make execution use direct indices

Execution then becomes approximately:

c
for each prepared conversion
{
   emit known literal span;

   runtime_info = conversion->info;

   if( conversion->width_argument)
      apply_width( &runtime_info,
                   values[ conversion->width_argument].i);

   if( conversion->precision_argument)
      apply_precision( &runtime_info,
                       values[ conversion->precision_argument].i);

   runtime_info.mystery = before;

   (*conversion->function->convert_argument)(
      buffer,
      &runtime_info,
      arguments,
      conversion->value_argument);
}


No running argument counter. No positional checks. No jump-table lookup. No modifier scan. No reconstruction of which stars consumed which argument.

Using a local runtime_info copy is important because dynamic width currently mutates:

- width
- precision
- minus_found
- left_justify
- zero_found
- mystery

An immutable template plus a runtime copy makes the plan safely reusable.

## 4. Resolve validity and function only once

Currently parsing does approximately:

1. determine_is_valid_conversion_character() looks up the conversion.
2. functions_for_conversion() looks it up again.
3. Later stages use the cached function.

After parse_conversion_info() identifies the final conversion character, perform one lookup:

c
function = functions_for_conversion( table->jumps, info->conversion);


Then validity is determined from that result, with the required special handling for %% and modifiers. Store the pointer and never consult the jump table again during this invocation.

This also means:

- determine_argument_type() no longer needs a jump table;
- jump_convert_argument() can disappear;
- context_print() no longer needs table;
- convert_argument() can be reduced to runtime-star application plus one callback.

## 5. Separate plan data from invocation data

setup_context() currently mixes two kinds of information.

### Format-invariant

- Parsed conversion information
- Function pointers
- Argument types
- Effective argument indices
- Literal offsets
- Format length
- Number of conversions and arguments

### Per invocation

- Argument values
- Dynamic width and precision values
- before
- Output buffer state
- Failure state

Separating these is what enables both deduplication and caching. In particular, setup_context() should not need the output buffer; it only uses it to initialize before. Compute before
immediately before execution instead.

The resulting core becomes:

c
prepare_format( &plan, format, table);
load_argument_values( &values, plan.argument_types, plan.argc, va);
execute_plan( buffer, format, &plan, &values);


The va_list and mulle_vararg_list variants should retain separate loaders so no indirect loader callback is introduced into the hot path. They can share preparation and execution
completely.

## What can actually be cached

Do not cache argument values. They vary on every call and can contain transient pointers.

Cache this instead:

- parsed conversions;
- effective argument indices;
- conversion function pointers;
- argument-type array;
- literal offsets;
- format length.

A cache hit becomes:

text
validate cached plan
load current values
execute plan


That skips essentially all of setup_context().

## Safe cache design

An automatic cache cannot safely use only the format pointer because callers may modify a format buffer in place.

A conservative first version would be a one-entry thread-local cache containing:

c
struct mulle_sprintf_cached_plan
{
   char                             *original_format_pointer;
   char                             *format_copy;
   struct mulle_sprintf_conversion  *table;
   unsigned long                    table_generation;
   struct mulle_sprintf_plan        plan;
};


Hit conditions:

1. Same format pointer.
2. Same conversion table.
3. Same table generation.
4. strcmp(format, cached_format_copy) == 0.

The string comparison still scans the format, but it is substantially cheaper than reparsing, resolving callbacks, rebuilding argument routing, and determining types. The benchmark
repeatedly uses the same short literal, so it is an ideal cache case—although TLS lookup and comparison overhead must be measured for two-byte formats.

### Conversion-table generation

Registration can replace conversion functions, so cached plans need invalidation. Add a generation counter incremented by:

- conversion registration;
- modifier registration.

The plan records the generation used to build it. Concurrent mutation is already not safe in the present design, so this need not create stronger synchronization guarantees.

If changing struct mulle_sprintf_conversion is undesirable, initially cache only the default table and maintain a global/default-table generation counter.

### Start with one cache entry

One-entry “last plan” caching catches tight loops and has minimal lookup cost. If real applications alternate two or four formats, consider a tiny direct-mapped TLS cache later. Do not
start with an LRU implementation.

## Extensibility concern: type callback purity

Caching determine_argument_type assumes the callback’s result depends on:

- the parsed info;
- the registered function implementation.

That is strongly implied by its API, but should be made explicit. If custom callbacks are allowed to return different types based on unrelated mutable global state, cache only parsing
and argument indices, and rerun type determination on each invocation.

I would document the callback as deterministic and invalidate plans when registration changes.

## About fusing parsing and printing

For non-positional formats, it is technically possible to parse, consume one argument, and immediately print it. I would not do that because:

- invalid syntax later in the format would produce partial output where the current implementation validates first;
- callbacks currently receive the full argument array and could conceivably inspect it;
- va_list portability makes one-at-a-time extraction across helper calls awkward;
- positional formats still require the full preparation path.

An immutable plan removes nearly the same duplicated work without changing behavior.

## Suggested implementation order

1. Normalize effective argument indices during parsing.
2. Store type and function in the prepared conversion.
3. Remove determine_all_conversion_argument_types() semantic routing.
4. Make context_print() execute using direct indices.
5. Move before out of format preparation.
6. Store literal offsets/lengths rather than recalculating them.
7. Benchmark the non-cached execution plan.
8. Only then add a one-entry TLS plan cache.

This is the macro-scale optimization I would pursue. The current implementation has already started becoming a plan by caching info->function; finishing that transformation should
remove most of the repeated work without sacrificing the extensible callback architecture.
