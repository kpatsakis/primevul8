  void DecodeBmpV2(OpKernelContext* context, StringPiece input) {
    OP_REQUIRES(
        context, channels_ != 1,
        errors::InvalidArgument(
            "`channels` must be 0, 3 or 4 for BMP, but got ", channels_));

    if (op_type_ != "DecodeBmp" && op_type_ != "DecodeImage") {
      if (op_type_ == "DecodeAndCropJpeg") {
        OP_REQUIRES(context, false,
                    errors::InvalidArgument(
                        "DecodeAndCropJpeg operation can run on JPEG only, but "
                        "detected BMP."));
      } else {
        OP_REQUIRES(context, false,
                    errors::InvalidArgument(
                        "Trying to decode BMP format using a wrong op. Use "
                        "`decode_bmp` or `decode_image` instead. Op used: ",
                        op_type_));
      }
    }

    OP_REQUIRES(context, (32 <= input.size()),
                errors::InvalidArgument("Incomplete bmp content, requires at "
                                        "least 32 bytes to find the header "
                                        "size, width, height, and bpp, got ",
                                        input.size(), " bytes"));

    const uint8* img_bytes = reinterpret_cast<const uint8*>(input.data());
    int32_t header_size_ = internal::SubtleMustCopy(
        *(reinterpret_cast<const int32*>(img_bytes + 10)));
    const int32_t header_size = ByteSwapInt32ForBigEndian(header_size_);
    int32_t width_ = internal::SubtleMustCopy(
        *(reinterpret_cast<const int32*>(img_bytes + 18)));
    const int32_t width = ByteSwapInt32ForBigEndian(width_);
    int32_t height_ = internal::SubtleMustCopy(
        *(reinterpret_cast<const int32*>(img_bytes + 22)));
    const int32_t height = ByteSwapInt32ForBigEndian(height_);
    int16_t bpp_ = internal::SubtleMustCopy(
        *(reinterpret_cast<const int16*>(img_bytes + 28)));
    const int16_t bpp = ByteSwapInt16ForBigEndian(bpp_);

    // `channels_` is desired number of channels. `img_channels` is number of
    // channels inherent in the image.
    int img_channels = bpp / 8;
    OP_REQUIRES(
        context, (img_channels == 1 || img_channels == 3 || img_channels == 4),
        errors::InvalidArgument(
            "Number of channels inherent in the image must be 1, 3 or 4, was ",
            img_channels));
    const int requested_channels = channels_ ? channels_ : img_channels;

    OP_REQUIRES(context, width > 0,
                errors::InvalidArgument("Width must be positive"));
    OP_REQUIRES(context, height != 0,
                errors::InvalidArgument("Height must be nonzero"));
    OP_REQUIRES(context, header_size >= 0,
                errors::InvalidArgument("header size must be nonnegative"));

    // The real requirement is < 2^31 minus some headers and channel data,
    // so rounding down to something that's still ridiculously big.
    OP_REQUIRES(
        context,
        (static_cast<int64_t>(width) * std::abs(static_cast<int64_t>(height))) <
            static_cast<int64_t>(std::numeric_limits<int32_t>::max() / 8),
        errors::InvalidArgument(
            "Total possible pixel bytes must be less than 2^30"));

    const int32_t abs_height = abs(height);

    // there may be padding bytes when the width is not a multiple of 4 bytes
    const int row_size = (img_channels * width + 3) / 4 * 4;

    // Make sure the size of input data matches up with the total size of
    // headers plus height * row_size.
    int size_diff = input.size() - header_size - (row_size * abs_height);
    OP_REQUIRES(
        context, size_diff == 0,
        errors::InvalidArgument(
            "Input size should match (header_size + row_size * abs_height) but "
            "they differ by ",
            size_diff));

    const int64_t last_pixel_offset = static_cast<int64_t>(header_size) +
                                      (abs_height - 1) * row_size +
                                      (width - 1) * img_channels;

    // [expected file size] = [last pixel offset] + [last pixel size=channels]
    const int64_t expected_file_size = last_pixel_offset + img_channels;

    OP_REQUIRES(
        context, (expected_file_size <= input.size()),
        errors::InvalidArgument("Incomplete bmp content, requires at least ",
                                expected_file_size, " bytes, got ",
                                input.size(), " bytes"));

    // if height is negative, data layout is top down
    // otherwise, it's bottom up.
    bool top_down = (height < 0);

    // Decode image, allocating tensor once the image size is known.
    Tensor* output = nullptr;
    OP_REQUIRES_OK(
        context,
        context->allocate_output(
            0, TensorShape({abs_height, width, requested_channels}), &output));

    const uint8* bmp_pixels = &img_bytes[header_size];

    if (data_type_ == DataType::DT_UINT8) {
      DecodeBMP(bmp_pixels, row_size, output->flat<uint8>().data(), width,
                abs_height, requested_channels, img_channels, top_down);
    } else {
      std::unique_ptr<uint8[]> buffer(
          new uint8[height * width * requested_channels]);
      DecodeBMP(bmp_pixels, row_size, buffer.get(), width, abs_height,
                requested_channels, img_channels, top_down);
      TTypes<uint8, 3>::UnalignedConstTensor buf(buffer.get(), height, width,
                                                 requested_channels);
      // Convert the raw uint8 buffer to desired dtype.
      // Use eigen threadpooling to speed up the copy operation.
      const auto& device = context->eigen_device<Eigen::ThreadPoolDevice>();
      if (data_type_ == DataType::DT_UINT16) {
        uint16 scale = floor((std::numeric_limits<uint16>::max() + 1) /
                             (std::numeric_limits<uint8>::max() + 1));
        // Fill output tensor with desired dtype.
        output->tensor<uint16, 3>().device(device) = buf.cast<uint16>() * scale;
      } else if (data_type_ == DataType::DT_FLOAT) {
        float scale = 1. / std::numeric_limits<uint8>::max();
        // Fill output tensor with desired dtype.
        output->tensor<float, 3>().device(device) = buf.cast<float>() * scale;
      }
    }
  }