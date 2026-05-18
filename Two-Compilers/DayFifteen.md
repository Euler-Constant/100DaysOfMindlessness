# Day Fifteen: Going through (most of) the terminology in a bid to explain the algorithm

- Paper:
- Date: 2026-05-10
- Status: Paper analysis

## What exactly are we looking at?
Well, if you've been following this series, welcome back. We're still on Superword Level Parallelism, and to be completely honest, half my inactivity has been life throwing a very mean curveball at me, and the other half is just me not understanding the algorithm implementation properly, which is why I decided to go back to the base in a bid to explain how exactly all of this works, and in doing just that, it became really clear. I guess it was just a prerequisite gap xD

## So, back to base;

### What are compilers actually doing?
Specifically speaking, a compiler takes a program that conforms to a source grammar, which is the legal structure of the language, and produces a program that is legal according to the target grammar. These two programs are to be equivalent to one another (under the same ISA backend, of course)
Well, not quite, at least not in practice. See the thing is it does not go from source to destination just like that, they go through a series of Intermediate Representations (IRs), which are different formal languages with their own grammar and their own semantics, chosen to make a particular transformation easier. Different optimisation techniques use different IRs, like the three-address form which is explicitly stated in the paper to increase flexibility and code atomicity, making every single operation explicit, and every operation that would be worked on  would then be a visible, class-named statement. Others include the Single Static Assignment (SSA) format, although that would have to be a separate case study.

- Basic blocks
- Dependence 
- Isomorphism 

## The Algorithm itself;

### Loop Unrolling:
This is performed relatively early for reasons, as stated in the paper, is most easily done at a rather high level of the optimisation process. It is primarily used to turn vector parallelisms into basic blocks with SLP. In this technique, an unroll factor is used, as stated;

> "In order to ensure full optimisation of the superword datapath in the presence of a vectorizable loop, the unroll factor must be customised to the data sizes within the loop."

and is usually relative to the SIMD width (a bit of context I found scouring some more)

### Alignment Analysis:

Now this is just **a bit* nuanced, primarily because the reason we'd do this is not immediately obvious, which is for architectures that do not support unaligned memory accesses. This step helps to improve the performance gains by optimising with this particular technique. "Without it, memory accesses are assumed to be unaligned and the proper merging code must be emitted for every wide load and store"
SIMD 'load' instructions on most architectures require that the access itself is aligned to the SIMD width. Alignment analysis statically determines, for each load and store instruction, whether its address is guaranteed to be aligned at compile time. Basically, a type system. The result is an annotation on each load/store: either a known alignment value, or an unknown value (which has a symbol I cannot seem to find at the time of typing this).

### Pre-optimisation:

The IR is first flattened to three-address form, then classical optimisations are allowed to run after, some which could include;

- Constant and/or Copy propagation
- Dead Code Elimination
- Common Subexpression Elimination 
- Loop-Invariant Code Motion (Even I didn't know about this, would need to do research as to its implementation)
- Redundant Load/Store Elimination
- Scalar renaming

After this phase, which includes one or a few of the above (which is not an exhaustive list), it is in its cleanest state.

### Identifying Memory Adjacent References:

From here I began to have problems with how I viewed the algorithm, and it's quite simple, actually. This point begins the packing algorithm, forming PackSets. Basically, a PackSet is a set of packs, and a pack is an n-tuple where thee values are independent isomorphic statements in a basic block. The core of this step involves scanning each basic block to find pairs of independent statements, and checks for isomorphism, independence, position consistency and alignment consistency.

### PackSet Extension:

Now, once the PackSet has been initialised, we can continue in another direction; extending the set with more groups. It finds new candidates that can either;

- Produce needed source operands in packed form, or
- Use existing packed data as source operands.

This is usually only possible if the unit packs meet the following requirements: 

- Isomorphism
- Independence
- Left statements and right statements are not already packed in a left or right position
- Consistency in alignment information
- Execution time of the parallel operation is less than its sequential variant

which are usually steps that are fulfilled by the steps above.

### PackSet Combination:

After all the prior steps, they are then combined into larger groups. Combined groups are guaranteed to be less than or equal to the datapath size, as a result of earlier memory alignment.

### Scheduling:

The final step is one more place I got really confused, and I still am, so some thoughts are not completely.

Within groups, the resolution is mostly set, and we do not have to do much as per the algorithm, but the bid changes between groups. Between groups, there may be dependences. If group G₂ contains a statement that reads a value produced by a statement in group G₁, then G₁ must be scheduled before G₂.
The scheduler builds a dependence graph over groups; directed edges from producer groups to consumer groups, and performs a topological sort. As long as this graph is acyclic, a valid schedule exists.
