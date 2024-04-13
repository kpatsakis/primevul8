  Status AddSparseTensor(OpKernelContext* ctx, const SparseTensor& sp,
                         int64_t* handle) {
    Tensor ix;
    TF_RETURN_IF_ERROR(
        ctx->allocate_temp(sp.indices().dtype(), sp.indices().shape(), &ix));
    ix = sp.indices();

    Tensor values;
    TF_RETURN_IF_ERROR(ctx->allocate_temp(sp.indices().dtype(),
                                          sp.indices().shape(), &values));
    values = sp.values();
    {
      mutex_lock l(mu_);
      int64_t unique_st_handle = counter_++;  // increment is guarded on purpose
      sp_tensors_[unique_st_handle] = PersistentSparseTensor{
          ix, values,
          gtl::InlinedVector<int64_t, 8>(sp.shape().begin(), sp.shape().end())};
      *handle = unique_st_handle;
    }
    return Status::OK();
  }