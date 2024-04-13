  Status RetrieveAndClearSparseTensors(
      OpKernelContext* ctx, const TTypes<int64_t>::ConstVec& handles,
      std::vector<SparseTensor>* sparse_tensors) {
    sparse_tensors->clear();
    sparse_tensors->reserve(handles.size());
    {
      mutex_lock l(mu_);
      for (size_t i = 0; i < handles.size(); ++i) {
        const int64_t handle = handles(i);
        auto sp_iter = sp_tensors_.find(handle);
        if (sp_iter == sp_tensors_.end()) {
          return errors::InvalidArgument(
              "Unable to find SparseTensor: ", handle, " in map: ", name_);
        }
        const Tensor* ix = &sp_iter->second.indices;
        const Tensor* values = &sp_iter->second.values;
        const auto& shape = sp_iter->second.shape;
        SparseTensor tensor;
        TF_RETURN_IF_ERROR(SparseTensor::Create(*ix, *values, shape, &tensor));
        sparse_tensors->push_back(std::move(tensor));
        sp_tensors_.erase(sp_iter);
      }
    }

    return Status::OK();
  }