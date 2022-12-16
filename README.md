# Symbolic Version of Fast Downward with Axiom Support
Axioms are an extension for classical planning models that allow for modeling complex preconditions and goals exponentially more compactly. Although axioms were introduced in planning more than a decade ago, modern planning techniques rarely support axioms, especially in cost-optimal planning. Symbolic search is a popular and competitive optimal planning technique based on the manipulation of sets of states. In this work, we extend symbolic search algorithms to support axioms natively. We analyze different ways of encoding derived variables and axiom rules to evaluate them in a symbolic representation. We prove that all encodings are sound and complete, and empirically show that the presented approach outperforms the previous state of the art in cost-optimal classical planning with axioms.

 - Speck, D.; Geißer, F.; Mattmüller, R.; and Torralba, Á. 2019. Symbolic planning with axioms. In Lipovetzky, N.; Onaindia, E.; and Smith, D. E., eds., Proceedings of the Twenty-Ninth International Conference on Automated Planning and Scheduling (ICAPS 2019), 464–572. AAAI Press. ([pdf](https://github.com/speckdavid/fd-symbolic-axioms/blob/master/paper.pdf))
 - Symbolic Fast Downward: https://fai.cs.uni-saarland.de/torralba/software.html
 - Fast Downward: https://www.fast-downward.org/

##  Recommendations and Hints

This repository is no longer maintained, but we have updated it so that the planner should build on modern hardware. You can find a maintained, improved, and up-to-date version of this work as part of the Symk planner. Note, however, that we have only migrated symbolic compilation to Symk and the other approaches are only available here. 

- Recommended repository for symbolic search with axiom support: [SymK Planner](https://github.com/speckdavid/symk) 
- Original version we ran the experiments with: [Tag ICAPS2019](https://github.com/speckdavid/fd-symbolic-axioms/tree/icaps-2019)
- Benchmarks we used for the original experiments: [Benchmarks](https://github.com/speckdavid/fd-symbolic-axioms/tree/icaps-2019/benchmarks)


## Dependencies

Currently we only support Linux systems. The following should install all necessary dependencies.
```console
$ sudo apt-get -y install cmake g++ make python autotools-dev automake gcc g++-multilib
```
 
## Compiling the Planner

```console
$ ./buildy.py 
```

## Runing the Planner

We recommend to use the symbolic compilation with bidirectional search.

```console
$ ./fast-downward.py domain.pddl problem.pddl --search "sbd()"
```

Other configurations are as follows.

### Symbolic Compilation

```console
# Bidirectional Search
$ ./fast-downward.py domain.pddl problem.pddl --search "sbd()"

# Forward Search
$ ./fast-downward.py domain.pddl problem.pddl --search "sfw()"

#Backward Search
$ ./fast-downward.py domain.pddl problem.pddl --search "sbw()"
```

### Action-Based Encoding
```console
# Forward Search
$ ./fast-downward.py domain.pddl problem.pddl --search "sfw(axiom_type=interleaved_rules)"
```

### Variable-Based Encoding
```console
# Forward Search
$ ./fast-downward.py domain.pddl problem.pddl --search "sfw(axiom_type=varbdds)"
```


# Fast Downward

Fast Downward is a domain-independent planning system.

For documentation and contact information see http://www.fast-downward.org/.

The following directories are not part of Fast Downward as covered by this
license:

* ./src/search/ext

For the rest, the following license applies:

```
Copyright (C) 2003-2016 Malte Helmert
Copyright (C) 2008-2016 Gabriele Roeger
Copyright (C) 2010-2016 Jendrik Seipp
Copyright (C) 2010, 2011, 2013-2016 Silvan Sievers
Copyright (C) 2012-2016 Florian Pommerening
Copyright (C) 2016 Martin Wehrle
Copyright (C) 2013, 2015 Salome Simon
Copyright (C) 2014, 2015 Patrick von Reth
Copyright (C) 2015 Manuel Heusner, Thomas Keller
Copyright (C) 2009-2014 Erez Karpas
Copyright (C) 2014 Robert P. Goldman
Copyright (C) 2010-2012 Andrew Coles
Copyright (C) 2010, 2012 Patrik Haslum
Copyright (C) 2003-2011 Silvia Richter
Copyright (C) 2009-2011 Emil Keyder
Copyright (C) 2010, 2011 Moritz Gronbach, Manuela Ortlieb
Copyright (C) 2011 Vidal Alcázar Saiz, Michael Katz, Raz Nissim
Copyright (C) 2010 Moritz Goebelbecker
Copyright (C) 2007-2009 Matthias Westphal
Copyright (C) 2009 Christian Muise

Fast Downward is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

Fast Downward is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.
```
