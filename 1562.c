  void DecodeJpegV2(OpKernelContext* context, StringPiece input) {
    OP_REQUIRES(context, channels_ == 0 || channels_ == 1 || channels_ == 3,
                errors::InvalidArgument("JPEG does not support 4 channels"));

    // Use local copy of flags to avoid race condition as the class member is
    // shared among different invocations.
    jpeg::UncompressFlags flags = flags_;
    flags.components = channels_;

    if (op_type_ == "DecodeAndCropJpeg") {
      flags.crop = true;
      // Update flags to include crop window.
      const Tensor& crop_window = context->input(1);
      OP_REQUIRES(context, crop_window.dims() == 1,
                  errors::InvalidArgument("crop_window must be 1-D, got shape ",
                                          crop_window.shape().DebugString()));
      OP_REQUIRES(context, crop_window.dim_size(0) == 4,
                  errors::InvalidArgument("crop_size must have four elements ",
                                          crop_window.shape().DebugString()));
      auto crop_window_vec = crop_window.vec<int32>();
      flags.crop_y = crop_window_vec(0);
      flags.crop_x = crop_window_vec(1);
      flags.crop_height = crop_window_vec(2);
      flags.crop_width = crop_window_vec(3);
    } else if (op_type_ == "DecodeBmp") {
      // TODO(b/171060723): Only DecodeBmp as op_type_ is not acceptable here
      // because currently `decode_(jpeg|png|gif)` ops can decode any one of
      // jpeg, png or gif but not bmp. Similarly, `decode_bmp` cannot decode
      // anything but bmp formats. This behavior needs to be revisited. For more
      // details, please refer to the bug.
      OP_REQUIRES(context, false,
                  errors::InvalidArgument(
                      "Trying to decode JPEG format using DecodeBmp op. Use "
                      "`decode_jpeg` or `decode_image` instead."));
    }

    // Output tensor and the image buffer size.
    Tensor* output = nullptr;
    int buffer_size = 0;

    // Decode JPEG. Directly allocate to the output buffer if data type is
    // uint8 (to save extra copying). Otherwise, allocate a new uint8 buffer
    // with buffer size. `jpeg::Uncompress` supports unit8 only.
    uint8* buffer = jpeg::Uncompress(
        input.data(), input.size(), flags, nullptr /* nwarn */,
        [&](int width, int height, int channels) -> uint8* {
          buffer_size = height * width * channels;
          Status status;
          // By the existing API, we support decoding JPEG with `DecodeGif`
          // op. We need to make sure to return 4-D shapes when using
          // `DecodeGif`.
          if (op_type_ == "DecodeGif") {
            status = context->allocate_output(
                0, TensorShape({1, height, width, channels}), &output);
          } else {
            status = context->allocate_output(
                0, TensorShape({height, width, channels}), &output);
          }
          if (!status.ok()) {
            VLOG(1) << status;
            context->SetStatus(status);
            return nullptr;
          }

          if (data_type_ == DataType::DT_UINT8) {
            return output->flat<uint8>().data();
          } else {
            return new uint8[buffer_size];
          }
        });

    OP_REQUIRES(
        context, buffer,
        errors::InvalidArgument(
            "jpeg::Uncompress failed. Invalid JPEG data or crop window."));

    // For when desired data type if unit8, the output buffer is already
    // allocated during the `jpeg::Uncompress` call above; return.
    if (data_type_ == DataType::DT_UINT8) {
      return;
    }
    // Make sure we don't forget to deallocate `buffer`.
    std::unique_ptr<uint8[]> buffer_unique_ptr(buffer);

    // Convert uint8 image data to desired data type.
    // Use eigen threadpooling to speed up the copy operation.
    const auto& device = context->eigen_device<Eigen::ThreadPoolDevice>();
    TTypes<uint8>::UnalignedConstFlat buffer_view(buffer, buffer_size);
    if (data_type_ == DataType::DT_UINT16) {
      uint16 scale = floor((std::numeric_limits<uint16>::max() + 1) /
                           (std::numeric_limits<uint8>::max() + 1));
      // Fill output tensor with desired dtype.
      output->flat<uint16>().device(device) =
          buffer_view.cast<uint16>() * scale;
    } else if (data_type_ == DataType::DT_FLOAT) {
      float scale = 1. / std::numeric_limits<uint8>::max();
      // Fill output tensor with desired dtype.
      output->flat<float>().device(device) = buffer_view.cast<float>() * scale;
    }
  }