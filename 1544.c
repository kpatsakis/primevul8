  ~SparseTensorAccessingOp() override {
    if (sparse_tensors_map_) sparse_tensors_map_->Unref();
  }