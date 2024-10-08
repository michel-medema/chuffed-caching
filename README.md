# Chuffed with Caching

A clone of version 0.13.2 of [Chuffed](https://github.com/chuffed/chuffed) to which a caching option has been added to exploit subproblem dominance as defined by Chu et al [1]. The code is a clone of the corresponding branch of the original repository with full history to make it easier to integrate any future changes from the original repository into this version of Chuffed.

## Usage

The Dockerfile defines a build environment that can be used to compile the project and a runtime environment that can be used to run the executable obtained from the compilation step.

First, create the build environment: 

`docker build --target build-env -t cmake .`.

The `cmake` container can then be used to compile the project:

`docker run -v /mnt/chuffed/:/mnt/chuffed/ -w /mnt/chuffed/ cmake /bin/bash -c "mkdir -p build && cmake -B build -S . && cmake --build build"`.

Alternatively, the project can be compiled with the logging of cache events enabled:

`docker run -v /mnt/chuffed/:/mnt/chuffed/ -w /mnt/chuffed/ cmake /bin/bash -c "mkdir -p build && cmake -DLOG_CACHE_EVENTS=1 -B build -S . && cmake --build build"`.

Once the project has been compiled and the executable is available in the build directory, the solver can be executed:

`docker run -v /mnt/chuffed/:/mnt/chuffed/ -v /mnt/minizinc/:/mnt/minizinc/:ro -w /mnt/chuffed/build/ cmake ./fzn-chuffed [options] [fzn file]`.

The bind mounts and paths should be adjusted as necessary (`/mnt/chuffed/` points to the top level directory and `/mnt/minizinc/` to the directory that contains the FlatZinc problems).

## Caching Options

`--caching [on|off]` enables or disables caching (the default is `off`).

`--variable-dominance-check [on|off]` controls whether the domains of the unfixed variables are included in the equivalence part (`off`) or the dominance part (`on`) of the projection key (the default is `off`).

`--cache-strategy [unbounded|random|lru|fifo|threshold|event]` the strategy to use for managing the cache (the default is `unbounded`).

`--cache-size <n>` an integer specifying the maximum size of the cache (the default is `-1`). Only used for `--cache-strategy [random|lru|fifo]`.

`--prune-size <n>>` an integer specifying the number of entries that are removed from the cache when the maximum cache size is reached (the default is `0`). Only used for `--cache-strategy [random|lru|fifo]`.

`--prune-interval <n>` the interval, expressed as the number of nodes developed, with which entries are pruned from the cache (the default is `0`). Only used for `--cache-strategy threshold`.

`--prune-threshold <n>` when pruning is triggered, entries that have not resulted in a cache hit for more than `n` iterations are removed (the default is `1000`). Only used for `--cache-strategy threshold`.

`--cache-event-out <file>` a path to the file in which the cache-related events are stored (the default is `./events.csv`). Only used when recording cache events is enabled, which is the case for the `fzn-chuffed-cache-events` executable.

`--cache-events <file>` a path to a file that stores previously recorded cache events (the default is `""`). Only used for `--cache-strategy event`.

## Changes

The largest part of the caching-related code can be found in the [caching](./chuffed/caching) subdirectory. However, adding support for caching also required a number of existing files to be modified. In particular, the propagators have been extended and modified to make it possible to include their representations in the projection keys. These modifications include attaching additional events to the variables, adjusting the `wakeup` method to update the internal state of the propagator (or the parent class), and keeping track of internal state relevant to the projection key representation. Attaching additional events to propagators requires care in terms of adjusting the `wakeup` method, as these often change the internal state of the propagator. Triggering additional calls to this method may result in incorrect behaviour. As such, it is important to ensure that such changes are only applied when the originally attached events are triggered and not when the additional events are triggered. It is also important to note that for BoolView variables, the change variable passed to the `wakeup` method is always zero, which means that it is not possible to rely on the change events. Note also that if a variable is fixed upon construction, it immediately calls the wakeup method of attached propagators. The following provides an overview of the files that have changed and the reason for the change.

* [engine.h](./chuffed/core/engine.h) Several class members and methods have been added that are used to keep track of subproblems, add them to the cache when encountering a conflict, and search the cache for dominating subproblems.
* [engine.cpp](./chuffed/core/engine.cpp) The search loop has been modified to use the cache to search for dominating subproblems. The backtracking function has been adjusted so that it adds subproblems that have been fully explored to the cache. The solve method checks caching support for all constraints that occur in a problem.
* [init.cpp](./chuffed/core/init.cpp) The functions that initialise the propagators have been moved to the top to ensure that fixed variables are correctly propagated. A block has also been added to initialise the cache.
* [options.h](./chuffed/core/options.h) Several options related to caching have been added. See below for more details. Lazy and learn default to false.
* [options.cpp](./chuffed/core/options.cpp) Added cases to parse the caching options.
* [propagator.h](./chuffed/core/propagator.h) Added a Boolean value that indicates whether the propagator supports caching along with functions to enable and check caching support. These are convenience methods that make it easier to identify propagators for which caching support has not been added, which is needed to ensure that the solver does not run with such propagators while caching is enabled, as that may result in incorrect behaviour.
* [stats.cpp](./chuffed/core/stats.cpp) Included caching-related stats, including the maximum memory consumption of the solver.
* [arithmetic.cpp](./chuffed/primitives/arithmetic.cpp) The Abs propagator now extends the `CachingConstraint` class (which is a subclass of the `Propagator` class). Times, TimesAll, and Divide have been converted into an `EquivalenceConstraint`. Min2 has been turned into a `DominanceConstraint`.
* [binary.cpp](./chuffed/primitives/binary.cpp) BinGE and BinNE have been converted into an `EquivalenceConstraint`.
* [bool.cpp](./chuffed/primitives/bool.cpp) Boolean variables are added to the engine and special Boolean propagators are created for caching purposes.
* [element.cpp](./chuffed/primitives/element.cpp) IntElemBounds has been converted into an `EquivalenceConstraint`. The projection key includes all the values of the fixed variables of the constraint, the most strict representation that does not allow for any dominance detection. This representation is used because it is, at this point, not clear whether a more relaxed representation is possible.
* [linear.cpp](./chuffed/primitives/linear.cpp) LinearGE has been converted into a `DominanceConstraint`. LinearNE has been converted into an `EquivalenceConstraint`.
* [alldiff.cpp](./chuffed/global/alldif.cpp) AllDiffValue and AllDiffDomain have been converted into a `CachingConstraint`. AllDiffBounds has been converted into an `EquivalenceConstraint` with the most restrictive key, as it is unclear whether a better projection key can be obtained for this propagator.
* [linear-bool.cpp](./chuffed/global/linear-bool.cpp) BoolLinearLE has been converted into a `DominanceConstraint`.
* [minimum.cpp](./chuffed/global/minimum.cpp) Minimum has been converted into a `DominanceConstraint`.
* [table.cpp](./chuffed/global/table.cpp) Added special propagator for table constraints.
* [mip.h](./chuffed/global/mip.h) Converted into a `CachingConstraint`. This propagator always exists, even though its usage needs to be enabled explicitly with a command line argument. (Using it with caching may not be correct, so it should not be enabled whenever caching is used.)
* [mznlib](./chuffed/flatzinc/mznlib/) Removed the `fzn_seq_precede_chain_int.mzn` file: it is not clear what projection key should be used for these constraints, and if the file is removed, the compiler decomposes the constraints into simpler ones for which projection keys already exist.

## Issues

The variable class in [int-var.h](./chuffed/vars/int-var.h) defines a guard INT_DOMAIN_LIST with which a different version of the domain can be enabled. However, setting the value of this guard to 1 results in compilation errors. This is a problem with Chuffed itself (i.e. unrelated to caching).

## Bibliography

[1] G. Chu, M. G. de la Banda, and P. J. Stuckey, ‘Exploiting subproblem dominance in constraint programming’, Constraints, vol. 17, no. 1, pp. 1–38, Jan. 2012, doi: 10.1007/s10601-011-9112-9.