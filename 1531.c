  Status GetMap(OpKernelContext* ctx, bool is_writing,
                SparseTensorsMap** sparse_tensors_map) {
    mutex_lock l(mu_);

    if (sparse_tensors_map_) {
      *sparse_tensors_map = sparse_tensors_map_;
      return Status::OK();
    }

    TF_RETURN_IF_ERROR(cinfo_.Init(ctx->resource_manager(), def(),
                                   is_writing /* use_node_name_as_default */));

    CreatorCallback sparse_tensors_map_creator = [this](SparseTensorsMap** c) {
      SparseTensorsMap* map = new SparseTensorsMap(cinfo_.name());
      *c = map;
      return Status::OK();
    };

    TF_RETURN_IF_ERROR(
        cinfo_.resource_manager()->LookupOrCreate<SparseTensorsMap>(
            cinfo_.container(), cinfo_.name(), &sparse_tensors_map_,
            sparse_tensors_map_creator));

    *sparse_tensors_map = sparse_tensors_map_;
    return Status::OK();
  }