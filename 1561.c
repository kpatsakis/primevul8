  explicit DecodeImageV2Op(OpKernelConstruction* context) : OpKernel(context) {
    // Keep track of op string information because:
    // [1] Currently by the API, PNG, JPEG and GIF can decode each other and
    //     depending on the op type, we need to return either 3-D or 4-D shapes.
    // [2] Different ops have different attributes. e.g. `DecodeImage` op has
    //     `expand_animations` attribute that other ops don't.
    //     `DecodeAndDropJpeg` also has additional attributes.
    op_type_ = type_string();

    // Validate op type.
    OP_REQUIRES(context,
                op_type_ == "DecodeJpeg" || op_type_ == "DecodeAndCropJpeg" ||
                    op_type_ == "DecodePng" || op_type_ == "DecodeGif" ||
                    op_type_ == "DecodeBmp" || op_type_ == "DecodeImage",
                errors::InvalidArgument("Bad op type ", op_type_));

    // Get attributes from `DecodeJpeg` and `DecodeAndCropJpeg` op
    // invocations. For `DecodeImage` op, set JPEG decoding setting to TF
    // default.
    if (op_type_ == "DecodeJpeg" || op_type_ == "DecodeAndCropJpeg") {
      OP_REQUIRES_OK(context, context->GetAttr("ratio", &flags_.ratio));
      OP_REQUIRES(context,
                  flags_.ratio == 1 || flags_.ratio == 2 || flags_.ratio == 4 ||
                      flags_.ratio == 8,
                  errors::InvalidArgument("ratio must be 1, 2, 4, or 8, got ",
                                          flags_.ratio));
      OP_REQUIRES_OK(context, context->GetAttr("fancy_upscaling",
                                               &flags_.fancy_upscaling));
      OP_REQUIRES_OK(context,
                     context->GetAttr("try_recover_truncated",
                                      &flags_.try_recover_truncated_jpeg));
      OP_REQUIRES_OK(context,
                     context->GetAttr("acceptable_fraction",
                                      &flags_.min_acceptable_fraction));
      string dct_method;
      OP_REQUIRES_OK(context, context->GetAttr("dct_method", &dct_method));
      OP_REQUIRES(
          context,
          (dct_method.empty() || dct_method == "INTEGER_FAST" ||
           dct_method == "INTEGER_ACCURATE"),
          errors::InvalidArgument("dct_method must be one of "
                                  "{'', 'INTEGER_FAST', 'INTEGER_ACCURATE'}"));
      // The TensorFlow-chosen default for JPEG decoding is IFAST, sacrificing
      // image quality for speed.
      if (dct_method.empty() || dct_method == "INTEGER_FAST") {
        flags_.dct_method = JDCT_IFAST;
      } else if (dct_method == "INTEGER_ACCURATE") {
        flags_.dct_method = JDCT_ISLOW;
      }
    } else {
      flags_ = jpeg::UncompressFlags();
      flags_.dct_method = JDCT_IFAST;
    }

    // Get `dtype` attribute from `DecodePng` or `DecodeImage` op invocations.
    if (op_type_ == "DecodePng" || op_type_ == "DecodeImage") {
      OP_REQUIRES_OK(context, context->GetAttr("dtype", &data_type_));
      if (op_type_ == "DecodePng") {
        OP_REQUIRES(
            context,
            data_type_ == DataType::DT_UINT8 ||
                data_type_ == DataType::DT_UINT16,
            errors::InvalidArgument(
                "`dtype` for `DecodePng` must be unit8, unit16 but got: ",
                data_type_));
      } else {
        OP_REQUIRES(context,
                    data_type_ == DataType::DT_UINT8 ||
                        data_type_ == DataType::DT_UINT16 ||
                        data_type_ == DataType::DT_FLOAT,
                    errors::InvalidArgument("`dtype` for `DecodeImage` must be "
                                            "unit8, unit16, float but got: ",
                                            data_type_));
        OP_REQUIRES_OK(context, context->GetAttr("expand_animations",
                                                 &expand_animations_));
      }
    }

    // Get `channels` attribute for all ops except `DecodeGif` op.
    // `DecodeGif` doesn't have `channels` attribute but it supports 3
    // channels by default.
    if (op_type_ != "DecodeGif") {
      OP_REQUIRES_OK(context, context->GetAttr("channels", &channels_));
      OP_REQUIRES(
          context,
          channels_ == 0 || channels_ == 1 || channels_ == 3 || channels_ == 4,
          errors::InvalidArgument("`channels` must be 0, 1, 3 or 4 but got ",
                                  channels_));
    } else {
      channels_ = 3;
    }
  }