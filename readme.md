HPCE 2015 CW5
=============

- Issued: Mon 12th Oct
- Due: Mon 30th Oct, 22:00


Specification
-------------

You have been given the included code with the
goal of making things faster. For our purposes,
faster means the wall-clock execution time of
`puzzler::Puzzle::Execute`, across a broad spectrum
of scale factors. The target platform is an
AWS GPU (g2.2xlarge) instance, and the target AMI
will be the public HPCE-2015-v2 AMI. The AMI has
OpenCL GPU and software providers installed, alongside
TBB. You can determine the location of headers and
libraries by starting up the AMI.

Meta-specification
------------------

You've now got some experience in different methods
for acceleration, and a decent working knowledge
about how to transform code in reasonable reliable
ways. This coursework represents a fairly common
situation - you haven't got much time, either to analyse
the problem or to do low-level optimisation, and the problem
is actually a large number of sub-problems. So the goal
here is to identify and capture as much of the low-hanging
performance fruit as possible while not breaking anything.

The code-base I've given you is somewhat baroque,
(though not as convoluted as my original version,
I took pity), and despite having some rather iffy
OOP practises, actually has things quite reasonably
isolated. You will probably encounter the problem
that sometimes the reference solution starts to take
a very long time at large scales, but the persistence
framework gives you a way of dealing with that.

Beyond that, there isn't a lot more guidance, either
in terms of what you should focus on, or how
_exactly_ it will be measured. Part of the assesment
is in seeing whether you can work out what can be
accelerated, and where you should spend your time.

The allocation of marks I'm using is:

- Compilation/Execution: 20%

  - How much work do I have to do to get it to compile and run.

- Performance: 40%

  - You are competing with each other here, so there is an element of
    judgement in terms of how much you think others are doing or are
    capable of.

- Correctness: 40%

  - As far as I'm aware the ReferenceExecute is always correct, though slow.

Deliverable format
------------------

- Your repository should contain a readme.txt, readme.pdf, or readme.md covering:

    - What is the approach used to improve performance, in terms of algorithms, patterns, and optimisations.

    - A description of any testing methodology or verification.

    - A summary of how work was partitioned within the pair, including planning, analysis, design, and testing, as well as coding.

- Anything in the `include` directory is not owned by you, and subject to change
  
  - Any changes will happen in an additive way (none are expected for this CW)
  
  - Bug-fixes to `include` stuff are still welcome.

- The public entry point to your code is via `puzzler::PuzzleRegistrar::UserRegisterPuzzles`,
    which must be compiled into the static library `lib/libpuzzler.a`.
    
    - Clients will not directly include your code, they will only `#include "puzzler/puzzles.h`,
      then access puzzles via the registrar. They will get access to the registrar implementation
      by linking against `lib/libpuzzler.a`.
    
    - **Note**: If you do something complicated in your building of libpuzzler, it should still be
      possible to build it by going into `lib` and calling `make all`.

    - The current working directory during execution will be the root of the repository. So
      it will be executed as if typing `bin/execute_puzzle`, and an opencl kernel could be
      loaded using the relative path `provider/something.kernel`.
      
- The programs in `src` have no special meaning or status, they are just example programs 

The reason for all this strange indirection is that I want to give
maximum freedom for you to do strange things within your implementation
(example definitions of "strange" include CMake) while still having a clean
abstraction layer between your code and the client code.

Intermediate Testing
--------------------

As mentioned, I'm occasionally pulling and running tests on all the repositories, and
pushing the results back. These tests do _not_ check for correctness, they only check
that the implementations build and run correctly (are also for my own interest
in seeing how performance evolves over time) I will push the results into
the `dt10_runs` directory.

If you are interested in seeing comparitive performance results, you can opt in
by commiting a file called `dt10_runs/count_me_in`. This will result in graphs with
lines for your implementation versus others who also opted in, but you will only be able
to identify your line on the graph graph.

We had some discussion on how to pick which version to pull. I have decided
that I will just pull from master, as this reflects better working practise - if
there is a testing branch, then that is where the unstable code should
be. The master branch should ideally always be compilable and correct, and
branches only merged into master once they are stable.

Finally, to re-iterate: the tests I am doing do _no_ testing at all for correctness, they
don't even look at the output of the tests.
