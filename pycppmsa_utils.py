import numpy as np
import torch
import pycppmsa


def as_heads(weights, single_root=False, root_on_diag=False):
    is_np = isinstance(weights, np.ndarray)
    if not is_np and not isinstance(weights, torch.Tensor):
        raise RuntimeError("Unknown input type")
    if len(weights.shape) != 2 or weights.shape[0] != weights.shape[1]:
        raise RuntimeError("Invalid input shape")

    n_vertices = weights.shape[0] + (1 if root_on_diag else 0)

    # Big M trick to impose the single root constraint
    # => An additive  bounding  procedure for  the asymmetric travelling salesman problem (Fischetti and Toth)
    if single_root:
        big_m = weights.min() - weights.max() - 10
        weights = weights.copy() if is_np else weights.clone()
        if root_on_diag:
            if is_np:
                diag = np.empty(n_vertices - 1)
                diag.fill(big_m)
                diag = np.diag(diag)
            else:
                diag = torch.diag(torch.empty(n_vertices - 1).fill_(big_m))
            weights += diag
        else:
            weights[0] += big_m

    if is_np:
        weights = np.ascontiguousarray(weights.astype(np.float32))
        heads = np.empty(n_vertices, dtype=np.int32, order='C')

        weights_data_ptr, weights_readonly = weights.__array_interface__['data']
        heads_data_ptr, heads_readonly = heads.__array_interface__['data']
        if weights_readonly or heads_readonly:
            raise RuntimeError("Errors: cannot write on arrays!")

        pycppmsa.run(
            n_vertices,
            weights_data_ptr,
            heads_data_ptr,
            root_on_diag
        )

    else:
        device = weights.device
        weights = weights.to(dtype=torch.float32).cpu().contiguous()
        heads = torch.empty(n_vertices, dtype=torch.int32, device="cpu").contiguous()

        pycppmsa.run(
            n_vertices,
            weights.data_ptr(),
            heads.data_ptr(),
            root_on_diag
        )
        heads = heads.to(device).to(dtype=torch.long)

    return heads


def as_adjacency_matrix(weights, single_root=False, root_on_diag=False):
    heads = as_heads(weights, single_root=single_root, root_on_diag=root_on_diag)
    is_np = isinstance(weights, np.ndarray)
    n_vertices = weights.shape[0] + (1 if root_on_diag else 0)

    if root_on_diag:
        if is_np:
            matrix = np.zeros((n_vertices-1, n_vertices-1), dtype=np.int32)
            mods = np.arange(0, n_vertices)
            heads[heads == 0] = mods[heads == 0]
            matrix[heads[1:] - 1, mods[1:] - 1] = 1
        else:
            matrix = torch.zeros((n_vertices-1, n_vertices-1), device=heads.device, dtype=torch.long)
            mods = torch.arange(0, n_vertices, device=heads.device, dtype=torch.long)
            heads[heads == 0] = mods[heads == 0]
            matrix[heads[1:] - 1, mods[1:] - 1] = 1
    else:
        if is_np:
            matrix = np.zeros((n_vertices, n_vertices), dtype=np.int32)
            mods = np.arange(1, n_vertices)
            matrix[heads[1:], mods] = 1
        else:
            matrix = torch.zeros((n_vertices, n_vertices), device=heads.device, dtype=torch.long)
            mods = torch.arange(1, n_vertices, device=heads.device, dtype=torch.long)
            matrix[heads[1:], mods] = 1

    return matrix