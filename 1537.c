  void Compute(OpKernelContext* context) override {
    SparseTensorsMap* map = nullptr;
    OP_REQUIRES_OK(context, GetMap(context, false /* is_writing */, &map));

    const Tensor& sparse_handles = context->input(0);

    OP_REQUIRES(context, TensorShapeUtils::IsVector(sparse_handles.shape()),
                errors::InvalidArgument(
                    "sparse_handles should be a vector but received shape ",
                    sparse_handles.shape().DebugString()));

    int64_t N = sparse_handles.shape().dim_size(0);

    OP_REQUIRES(
        context, N > 0,
        errors::InvalidArgument("Must have at least 1 serialized SparseTensor, "
                                "but input matrix has 0 rows"));

    std::vector<Tensor> indices_to_concat;
    std::vector<Tensor> values_to_concat;
    std::vector<TensorShape> shapes_to_concat;

    const auto& sparse_handles_t = sparse_handles.vec<int64_t>();

    std::vector<SparseTensor> sparse_tensors;

    OP_REQUIRES_OK(context, map->RetrieveAndClearSparseTensors(
                                context, sparse_handles_t, &sparse_tensors));

    for (int64_t i = 0; i < N; ++i) {
      const SparseTensor& st = sparse_tensors[i];
      const Tensor& output_indices = st.indices();
      const Tensor& output_values = st.values();
      const auto output_shape = st.shape();

      OP_REQUIRES(context, TensorShapeUtils::IsMatrix(output_indices.shape()),
                  errors::InvalidArgument(
                      "Expected sparse_handles[", i,
                      "] to represent an index matrix but received shape ",
                      output_indices.shape().DebugString()));
      OP_REQUIRES(context, TensorShapeUtils::IsVector(output_values.shape()),
                  errors::InvalidArgument(
                      "Expected sparse_handles[", i,
                      "] to represent a values vector but received shape ",
                      output_values.shape().DebugString()));
      OP_REQUIRES(
          context, DataTypeToEnum<T>::value == output_values.dtype(),
          errors::InvalidArgument(
              "Requested SparseTensor of type ",
              DataTypeString(DataTypeToEnum<T>::value), " but SparseTensor[", i,
              "].values.dtype() == ", DataTypeString(output_values.dtype())));

      int64_t num_entries = output_indices.dim_size(0);
      OP_REQUIRES(context, num_entries == output_values.dim_size(0),
                  errors::InvalidArgument(
                      "Expected row counts of SparseTensor[", i,
                      "].indices and SparseTensor[", i,
                      "].values to match but they do not: ", num_entries,
                      " vs. ", output_values.dim_size(0)));
      int rank = output_indices.dim_size(1);
      OP_REQUIRES(
          context, rank == output_shape.size(),
          errors::InvalidArgument("Expected column counts of SparseTensor[", i,
                                  "].indices to match size of SparseTensor[", i,
                                  "].shape "
                                  "but they do not: ",
                                  rank, " vs. ", output_shape.size()));

      // Now we expand each SparseTensors' indices and shape by
      // prefixing a dimension
      Tensor expanded_indices(
          DT_INT64, TensorShape({num_entries, 1 + output_indices.dim_size(1)}));
      Tensor expanded_shape(DT_INT64, TensorShape({1 + rank}));
      const auto& output_indices_t = output_indices.matrix<int64_t>();
      auto expanded_indices_t = expanded_indices.matrix<int64_t>();
      auto expanded_shape_t = expanded_shape.vec<int64_t>();
      expanded_indices_t.chip<1>(0).setZero();
      Eigen::DSizes<Eigen::DenseIndex, 2> indices_start(0, 1);
      Eigen::DSizes<Eigen::DenseIndex, 2> indices_sizes(num_entries, rank);
      expanded_indices_t.slice(indices_start, indices_sizes) = output_indices_t;
      expanded_shape_t(0) = 1;
      // TODO: copy shape from TensorShape to &expanded_shape_t(1)
      // std::copy_n(&output_shape_t(0), rank, &expanded_shape_t(1));
      for (int i = 0; i < rank; ++i) {
        expanded_shape_t(i + 1) = output_shape[i];
      }
      TensorShape expanded_tensor_shape(expanded_shape_t);

      indices_to_concat.push_back(std::move(expanded_indices));
      values_to_concat.push_back(output_values);
      shapes_to_concat.push_back(std::move(expanded_tensor_shape));
    }

    int rank = -1;
    for (int i = 0; i < N; ++i) {
      if (rank < 0) rank = shapes_to_concat[i].dims();
      OP_REQUIRES(context, rank == shapes_to_concat[i].dims(),
                  errors::InvalidArgument(
                      "Inconsistent rank across SparseTensors: rank prior to "
                      "SparseTensor[",
                      i, "] was: ", rank, " but rank of SparseTensor[", i,
                      "] is: ", shapes_to_concat[i].dims()));
    }

    // SparseTensor::Concat requires consistent shape for all but the
    // primary order dimension (dimension 0 in this case).  So we get
    // the maximum value across all the input SparseTensors for each
    // dimension and use that.
    TensorShape preconcat_shape(shapes_to_concat[0]);
    for (int i = 0; i < N; ++i) {
      for (int d = 0; d < rank; ++d) {
        preconcat_shape.set_dim(d, std::max(preconcat_shape.dim_size(d),
                                            shapes_to_concat[i].dim_size(d)));
      }
    }

    // Dimension 0 is the primary dimension.
    gtl::InlinedVector<int64_t, 8> std_order(rank);
    std::iota(std_order.begin(), std_order.end(), 0);

    std::vector<SparseTensor> tensors_to_concat;
    tensors_to_concat.reserve(N);
    for (int i = 0; i < N; ++i) {
      SparseTensor tensor;
      OP_REQUIRES_OK(context,
                     SparseTensor::Create(std::move(indices_to_concat[i]),
                                          std::move(values_to_concat[i]),
                                          preconcat_shape, std_order, &tensor));
      tensors_to_concat.push_back(std::move(tensor));
    }

    auto output = SparseTensor::Concat<T>(tensors_to_concat);
    Tensor final_output_shape(DT_INT64, TensorShape({output.dims()}));

    std::copy_n(output.shape().data(), output.dims(),
                final_output_shape.vec<int64_t>().data());

    context->set_output(0, output.indices());
    context->set_output(1, output.values());
    context->set_output(2, final_output_shape);
  }