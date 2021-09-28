This tool can be used to compute the maximum spanning arborescence in a graph.
It is based on the implementation of André Martins, as distributed in AD3.

# Insallation

python3 setup.py install --user

This will install two packages: pycppmsa and pycppmsa_utils.

# Usages

pycppmsa_utils contains two functions:

- pycppmsa_utils.as_heads(weights, single_root=False, root_on_diag=True)
- pycppmsa_utils.as_adjacency_matrix(weights, single_root=False, root_on_diag=True)

The argument weights must be a n\*n matrix (either a pytorch tensor or a numpy array).
It assumes that the first coordinate index heads and the second coordinate index modifiers.
The number of vertices in the graph is automatically deduced from the weight matrix:

- if root_on_diag=True: v = number of vertices = n
- if root_on_diag=False: v = number of vertices = n - 1

The as_heads function returns heads as a vector of size v whose first element is -1 (the root as no head).
The as_adjacency_matrix returns the arborescence as an adjacency matrix of size v\*v is root_on_diag=True, otherwise (v-1)\*(v-1)
