*This project has been created as part of the 42 curriculum by yrafih.*

# Codexion

## Description

Codexion is a concurrency simulation built with POSIX threads. A configurable
number of **coders** (each represented by its own thread) sit around a circular
co-working hub and share a limited pool of **USB dongles** (one dongle between
each pair of coders, so there are as many dongles as coders).

To compile, a coder must hold **two dongles at once** — the one on its left and
the one on its right. After compiling it puts both dongles back, debugs, then
refactors, and finally tries to compile again. A coder **burns out** if it does
not start a new compile within `time_to_burnout` milliseconds of the start of
its previous compile (or of the start of the simulation).

The simulation stops when either:

- a coder burns out, or
- every coder has compiled at least `number_of_compiles_required` times.

Dongle access is arbitrated by a selectable scheduling policy (`fifo` or `edf`),
and a dedicated **monitor** thread is responsible for detecting burnout
precisely and stopping the simulation.

## Instructions

### Compilation

```sh
make            # builds the ./codexion binary
make clean      # removes object files
make fclean     # removes object files and the binary
make re         # full rebuild
```

The project is compiled with `cc -Wall -Wextra -Werror -pthread` and contains no
global variables.

### Execution

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All eight arguments are mandatory. The five timing values are in milliseconds,
counts are strictly positive integers, and `scheduler` must be exactly `fifo`
or `edf`. Invalid input is rejected with an error message and a non-zero exit
code.

Example:

```sh
./codexion 4 600 80 80 80 3 5 fifo
```

Each state change is logged as `timestamp_in_ms coder_number message`, e.g.:

```
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
81 1 is debugging
161 1 is refactoring
...
```

## Resources

- POSIX threads documentation: `man pthread_create`, `man pthread_mutex_init`,
  `man pthread_cond_wait`, `man pthread_cond_timedwait`, `man gettimeofday`.
- The classic *Dining Philosophers* problem (Dijkstra), which models the same
  shared-resource / deadlock / starvation concerns.
- *Earliest Deadline First* (EDF) real-time scheduling and Coffman's four
  conditions for deadlock.

### Use of AI

AI was used as an assistant for the following, all of which were reviewed and
are fully understood by the author:

- Bringing the source base into compliance with the 42 Norm (indentation,
  function and file size limits, header formatting, removing function-like
  macros).
- Reviewing the synchronization logic and fixing concurrency bugs (EDF queue
  insertion, scheduler-specific cleanup, monitor-based burnout detection).
- Drafting this README.

AI was **not** used to design the core algorithm without comprehension; every
synchronization decision is explained below.

## Blocking cases handled

- **Deadlock prevention (Coffman's conditions).** Each dongle is a separate
  resource guarded by its own mutex, so mutual exclusion is unavoidable, but the
  other three Coffman conditions are neutralised: a coder only attempts to grab
  dongles once the scheduler has granted it its compile turn, every blocking
  wait is a bounded `pthread_cond_timedwait`, and a global stop flag breaks any
  potential circular wait. As a result no set of coders can hold-and-wait on
  each other indefinitely.
- **Starvation prevention / liveness.** Under `fifo` requests are served in
  arrival order; under `edf` the coder with the earliest burnout deadline
  (`last_compile_start + time_to_burnout`) is served first, with a deterministic
  tie-breaker (higher coder id wins on equal deadlines). The most at-risk coder
  is therefore always prioritised.
- **Cooldown handling.** When a dongle is released its release timestamp is
  recorded; it cannot be taken again until `dongle_cooldown` milliseconds have
  elapsed. The waiting is implemented with `pthread_cond_timedwait` so a coder
  sleeps exactly until the dongle becomes available.
- **Precise burnout detection.** A dedicated monitor thread polls every
  millisecond and compares each coder's elapsed time since its last compile
  against `time_to_burnout`, so a burnout is reported well within the required
  10 ms window.
- **Log serialization.** Every log line is printed while holding a single output
  mutex, so two messages can never interleave on one line.

## Thread synchronization mechanisms

The implementation relies exclusively on `pthread_mutex_t` and `pthread_cond_t`.

- **`usb_mutex` + `usb_rec_cond` (one pair per dongle).** Protect each dongle's
  `is_available` flag and cooldown timestamp. This is what guarantees a dongle
  is never duplicated: a coder can only mark a dongle as taken while holding its
  mutex. The condition variable wakes coders when a dongle is released or its
  cooldown expires.
- **`sched_lock` + `sched_id` (scheduler).** Protect the scheduling queue
  (FIFO list / EDF ordered list) and signal coders when the front of the queue
  changes, so the next coder can take its compile turn.
- **`death_lock` (monitor state).** Protects the shared simulation state:
  `is_someone_dead`, `finished_coders`, and each coder's `last_time_comp` and
  `compiled_count`. This is the channel between the coders and the monitor: a
  coder publishes its progress under this lock and the monitor reads it under the
  same lock, so there is no data race on shared state. The monitor sets
  `is_someone_dead` under this lock, and every coder observes it through
  `check_death`.
- **`print_lock` (logging).** Serializes all output.

**Race prevention example.** `last_time_comp` is written by a coder only while
holding `death_lock` and read by the monitor only while holding the same lock,
so the read and the write are always ordered. **Thread-safe communication**
between coders and the monitor is achieved entirely through `death_lock`:
coders never call back into the monitor, they simply update protected fields,
and blocking coders use timed waits so they always notice the stop flag and
exit cleanly, allowing the main thread to join every thread without leaks.
