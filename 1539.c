  void Compute(OpKernelContext* context) override {
    const Tensor* input_indices;
    const Tensor* input_values;
    const Tensor* input_shape;
    SparseTensorsMap* map;

    OP_REQUIRES_OK(context, context->input("sparse_indices", &input_indices));
    OP_REQUIRES_OK(context, context->input("sparse_values", &input_values));
    OP_REQUIRES_OK(context, context->input("sparse_shape", &input_shape));
    OP_REQUIRES_OK(context, GetMap(context, true /* is_writing */, &map));

    OP_REQUIRES(context, TensorShapeUtils::IsMatrix(input_indices->shape()),
                errors::InvalidArgument(
                    "Input indices should be a matrix but received shape ",
                    input_indices->shape().DebugString()));

    OP_REQUIRES(context, TensorShapeUtils::IsVector(input_values->shape()),
                errors::InvalidArgument(
                    "Input values should be a vector but received shape ",
                    input_values->shape().DebugString()));

    OP_REQUIRES(context, TensorShapeUtils::IsVector(input_shape->shape()),
                errors::InvalidArgument(
                    "Input shape should be a vector but received shape ",
                    input_shape->shape().DebugString()));

    TensorShape input_shape_object;
    OP_REQUIRES_OK(
        context, TensorShapeUtils::MakeShape(input_shape->vec<int64_t>().data(),
                                             input_shape->NumElements(),
                                             &input_shape_object));
    SparseTensor st;
    OP_REQUIRES_OK(context, SparseTensor::Create(*input_indices, *input_values,
                                                 input_shape_object, &st));
    int64_t handle;
    OP_REQUIRES_OK(context, map->AddSparseTensor(context, st, &handle));

    Tensor sparse_handle(DT_INT64, TensorShape({}));
    auto sparse_handle_t = sparse_handle.scalar<int64_t>();

    sparse_handle_t() = handle;

    context->set_output(0, sparse_handle);
  }