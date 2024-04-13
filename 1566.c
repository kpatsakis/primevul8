  void Compute(OpKernelContext* context) override {
    const Tensor& contents = context->input(0);
    OP_REQUIRES(
        context, TensorShapeUtils::IsScalar(contents.shape()),
        errors::InvalidArgument("`contents` must be scalar but got shape",
                                contents.shape().DebugString()));
    const StringPiece input = contents.scalar<tstring>()();
    OP_REQUIRES(context, !input.empty(),
                errors::InvalidArgument("Input is empty."));
    OP_REQUIRES(context, input.size() <= std::numeric_limits<int>::max(),
                errors::InvalidArgument(
                    "Input contents are too large for int: ", input.size()));

    // Parse magic bytes to determine file format.
    switch (ClassifyFileFormat(input)) {
      case kJpgFormat:
        DecodeJpegV2(context, input);
        break;
      case kPngFormat:
        DecodePngV2(context, input);
        break;
      case kGifFormat:
        DecodeGifV2(context, input);
        break;
      case kBmpFormat:
        DecodeBmpV2(context, input);
        break;
      case kUnknownFormat:
        OP_REQUIRES(context, false,
                    errors::InvalidArgument("Unknown image file format. One of "
                                            "JPEG, PNG, GIF, BMP required."));
        break;
    }
  }