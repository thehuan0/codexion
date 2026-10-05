*This project has been created as part of the 42 curriculum by jperez-s*

# Codexion

## Description

Codexion is a concurrency simulation written in C with POSIX threads. A ring
of coders shares a limited set of USB dongles. Every coder is a thread that
cycles through three phases: **compile**, **debug** and **refactor**.
Compiling needs two dongles at the same time (the one on the left and the one
on the right). There are exactly as many dongles as coders. A coder who does
not start a compile within `time_to_burnout` ms of the start of their previous
compile (or of the simulation) burns out and the simulation stops.

The goal is to share the dongles so that nobody burns out, while respecting:

- a mandatory **cooldown** after a dongle is released,
- a **fair arbitration policy** (`fifo` or `edf`) implemented with a
  hand-written binary heap (priority queue),
- a dedicated **monitor thread** that detects burnout within a few
  milliseconds,
- fully **serialized logging**.

The simulation ends when a coder burns out, or when every coder has compiled at
least `number_of_compiles_required` times.

### Overview of the code

| File(s) | Role |
|---|---|
| `main.c` | entry point, usage message, top-level error handling |
| `parsing_*.c` | strict argument parsing and range validation |
| `init_*.c`, `cleanup_simulation.c` | creation / destruction of all shared state |
| `threads.c` | start (monitor first, then coders) and join threads |
| `coder.c`, `coder_phases.c`, `coder_state.c` | coder life cycle and its protected state |
| `dongle_acquire.c`, `dongle_release.c` | arbitration, waiting, cooldown |
| `heap_*.c` | binary min-heap used as the waiting queue of each dongle |
| `monitor.c` | burnout / completion detection |
| `sim_state.c`, `log.c` | clock, stop flag, request sequence, serialized output |

## Instructions

### Build

```
make          # builds ./codexion with cc -Wall -Wextra -Werror -pthread
make clean    # removes object files
make fclean   # removes object files and the binary
make re       # fclean + all
```

The sources live in `srcs/`, the header in `includes/`. Objects are rebuilt
when `includes/codexion.h` changes, and nothing is relinked needlessly.

### Run

```
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Meaning |
|---|---|
| `number_of_coders` | number of coders = number of dongles (1 to 10000) |
| `time_to_burnout` | ms allowed between the starts of two compiles (>= 1) |
| `time_to_compile` | ms spent compiling, holding two dongles |
| `time_to_debug` | ms spent debugging |
| `time_to_refactor` | ms spent refactoring |
| `number_of_compiles_required` | stop when every coder reached this many compiles |
| `dongle_cooldown` | ms a released dongle stays unavailable |
| `scheduler` | exactly `fifo` or `edf` |

All values are non-negative decimal integers; every time value must be at most
`2147483647` ms. Anything else (negative numbers, signs, decimals, letters,
overflow, unknown scheduler, wrong argument count) is rejected with a usage
message and exit status 1. The limits keep every sum of two time values inside
a `long`, so no signed overflow can happen.

### Examples

```
./codexion 5 800 200 200 200 3 0 fifo     # nobody burns out, stops after 3 compiles
./codexion 5 800 200 200 200 3 0 edf
./codexion 1 300 100 100 100 3 0 fifo     # one dongle only: the coder burns out at 300 ms
./codexion 4 410 200 200 200 6 0 edf      # infeasible (cycle is 600 ms): burnout at 410 ms
```

Log format (one line per state change, never interleaved):

```
<timestamp_in_ms> <coder_id> has taken a dongle
<timestamp_in_ms> <coder_id> is compiling
<timestamp_in_ms> <coder_id> is debugging
<timestamp_in_ms> <coder_id> is refactoring
<timestamp_in_ms> <coder_id> burned out
```

### Technical choices

- **No global variable.** All shared state lives in a `t_simulation` structure
  passed by pointer to every thread.
- **Dongle numbering.** Coder `i` (1-based) uses dongle `i - 1` and dongle
  `(i - 2) mod n`. With one coder there is exactly one dongle.
- **Waiting queue capacity.** Only the two neighbours of a dongle can ever ask
  for it, and each has at most one pending request, so every heap has a fixed
  capacity of 2. Memory use is O(n), not O(n^2).
- **Time source.** `gettimeofday`, converted to elapsed milliseconds from the
  simulation start. Sleeps are computed against an absolute target time, so
  inaccuracy of `usleep` never accumulates from one chunk to the next.
- **Log before release.** A coder logs `is debugging` *before* putting the
  dongles back, so a neighbour's `is compiling` can never appear to start
  before the previous holder finished.

## Blocking cases handled

**Deadlock prevention (Coffman's conditions).** A deadlock needs all four
conditions at once.
- *Mutual exclusion*: inherent to a dongle (`taken` flag under a mutex); kept.
- *Hold and wait*: a coder holds its first dongle while waiting for the second;
  kept, but harmless because of the next point.
- *No preemption*: dongles are only released voluntarily; kept.
- *Circular wait*: **broken**. Every coder takes the dongle with the lower index
  first, then the higher one (`get_ordered_dongles`). The wait-for graph is
  therefore acyclic, so no cycle of coders can block each other. This covers
  the odd/even ring and the 2-coder case where both coders need the same pair.

**Starvation prevention.** Each dongle owns a heap of waiting requests. With
`fifo` the key is a global arrival sequence number. With `edf` the key is the
deadline `last_compile_start + time_to_burnout`, and ties are broken by the
arrival sequence number, so the order is fully deterministic. A dongle is only
granted to the request at the top of its heap.

**Cooldown handling.** `release_dongle` stores `available_at = now + cooldown`.
A waiter is only granted a dongle that is free, past its cooldown and at the
top of the heap. Waiters sleep on the dongle's condition variable with a
timeout that ends exactly when the cooldown expires, so no cycle is wasted and
no busy waiting occurs.

**Precise burnout detection.** The monitor thread polls every millisecond and
compares `now` with `last_compile_start + time_to_burnout` for each coder
(read through a mutex). Measured latency is 0 to 1 ms, well under the 10 ms
requirement. The monitor is started before the coders, so burnout is watched
from the very beginning even when many threads are being created.

**Log serialization.** A single `log_lock` protects every `printf`. The stop
flag is tested while holding that lock, so after the stop no state line is
printed and `burned out` is always the last line.

**Clean shutdown / lost wake-ups.** `set_stop_flag` sets the flag and then
broadcasts on every dongle condition variable while holding that dongle's
mutex, so a coder cannot check the flag, miss the broadcast and sleep forever.
Waits are also bounded by a 5 ms timeout and `sleep_interruptible` re-checks the
flag every 5 ms.

**Single coder.** One dongle exists. The coder takes it, then requests it
again as its "second" dongle while still holding it, so it can never compile
and burns out at `time_to_burnout`.

**Timing drift.** `sleep_interruptible` sleeps until an absolute end time
instead of adding nominal chunk lengths.

**Resource failures.** If an allocation, mutex/condition initialisation or
`pthread_create` fails, everything already created is released in the right
order, running threads are stopped and joined first, and the program exits with
an error message instead of leaking or crashing.

**Invalid input and overflow.** Strict digit-only parsing with overflow
detection, plus the limits above.

## Thread synchronization mechanisms

| Primitive | Protects | Used by |
|---|---|---|
| `pthread_mutex_t dongle.lock` | `taken`, `holder_id`, `available_at`, the waiting heap | coders, monitor (only to broadcast) |
| `pthread_cond_t dongle.cond` | waiting for a dongle to become grantable | coders (`pthread_cond_timedwait`), release and stop (`pthread_cond_broadcast`) |
| `pthread_mutex_t coder.state_lock` | `last_compile_start`, `compiles_done` | coder writes, monitor reads |
| `pthread_mutex_t log_lock` | standard output | every thread |
| `pthread_mutex_t stop_lock` | the `stop` flag | every thread |
| `pthread_mutex_t seq_lock` | the request sequence counter | coders |

**Lock ordering.** `dongle.lock` then `stop_lock`, and `log_lock` then
`stop_lock`. The reverse order never occurs: `set_stop_flag` releases
`stop_lock` before it takes any dongle lock. `state_lock` is a leaf lock held
only for one read or write.

**Examples of race prevention.**
- *Two coders race for one dongle*: both push a request into the dongle's heap
  under `dongle.lock`; only the heap top may set `taken`, and it does so inside
  the same critical section as the check, so a dongle can never be held twice.
- *Monitor versus coder on `last_compile_start`*: the coder updates it, and the
  monitor reads it, only through `state_lock` accessors.
- *Burnout versus an ongoing print*: the monitor sets the stop flag, then logs.
  Any coder log call that runs afterwards sees the flag under `log_lock` and
  prints nothing.
- *Fair ordering*: `seq_lock` makes the arrival number unique, which is also the
  EDF tie-breaker.

**Coder to monitor communication.** The monitor never writes coder state; it
only reads it through mutex-protected getters. The monitor stops the coders by
setting the stop flag and waking every waiting thread. Coders stop themselves
by polling the same flag between phases and inside every wait.

**Verification performed.** The project was built with ThreadSanitizer (no data
race reported) and with AddressSanitizer / LeakSanitizer / UndefinedBehavior
Sanitizer (no error, no leak), and failures were injected at every
`malloc`, `pthread_mutex_init`, `pthread_cond_init` and `pthread_create` call.

## Resources

- *POSIX Threads Programming*, Lawrence Livermore National Laboratory tutorial.
- `man 3 pthread_mutex_lock`, `pthread_cond_wait`, `pthread_cond_timedwait`,
  `pthread_create`, `man 2 gettimeofday`, `man 3 usleep`.
- E. G. Coffman, M. Elphick, A. Shoshani, *System Deadlocks*, ACM Computing
  Surveys, 1971 (the four conditions).
- E. W. Dijkstra, *Hierarchical ordering of sequential processes* (EWD 310),
  resource ordering and the dining philosophers.
- C. L. Liu, J. W. Layland, *Scheduling Algorithms for Multiprogramming in a
  Hard-Real-Time Environment*, 1973 (Earliest Deadline First).
- T. Cormen et al., *Introduction to Algorithms*, chapter on heapsort and
  priority queues.

### Use of AI

- generating throw-away test tooling (a log checker, sanitizer builds and a
  failure-injection harness) used to test the program;
- suggesting fixes for the issues found (absolute-time sleep, input limits);
- drafting this README.

🥭