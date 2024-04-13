  explicit SparseTensorAccessingOp(OpKernelConstruction* context)
      : OpKernel(context), sparse_tensors_map_(nullptr) {}