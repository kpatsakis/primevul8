bool MasteringMetadata::Parse(IMkvReader* reader, long long mm_start,
                              long long mm_size, MasteringMetadata** mm) {
  if (!reader || *mm)
    return false;

  std::unique_ptr<MasteringMetadata> mm_ptr(new MasteringMetadata());
  if (!mm_ptr.get())
    return false;

  const long long mm_end = mm_start + mm_size;
  long long read_pos = mm_start;

  while (read_pos < mm_end) {
    long long child_id = 0;
    long long child_size = 0;

    const long long status =
        ParseElementHeader(reader, read_pos, mm_end, child_id, child_size);
    if (status < 0)
      return false;

    if (child_id == libwebm::kMkvLuminanceMax) {
      double value = 0;
      const long long value_parse_status =
          UnserializeFloat(reader, read_pos, child_size, value);
      if (value < -FLT_MAX || value > FLT_MAX ||
          (value > 0.0 && value < FLT_MIN)) {
        return false;
      }
      mm_ptr->luminance_max = static_cast<float>(value);
      if (value_parse_status < 0 || mm_ptr->luminance_max < 0.0 ||
          mm_ptr->luminance_max > 9999.99) {
        return false;
      }
    } else if (child_id == libwebm::kMkvLuminanceMin) {
      double value = 0;
      const long long value_parse_status =
          UnserializeFloat(reader, read_pos, child_size, value);
      if (value < -FLT_MAX || value > FLT_MAX ||
          (value > 0.0 && value < FLT_MIN)) {
        return false;
      }
      mm_ptr->luminance_min = static_cast<float>(value);
      if (value_parse_status < 0 || mm_ptr->luminance_min < 0.0 ||
          mm_ptr->luminance_min > 999.9999) {
        return false;
      }
    } else {
      bool is_x = false;
      PrimaryChromaticity** chromaticity;
      switch (child_id) {
        case libwebm::kMkvPrimaryRChromaticityX:
        case libwebm::kMkvPrimaryRChromaticityY:
          is_x = child_id == libwebm::kMkvPrimaryRChromaticityX;
          chromaticity = &mm_ptr->r;
          break;
        case libwebm::kMkvPrimaryGChromaticityX:
        case libwebm::kMkvPrimaryGChromaticityY:
          is_x = child_id == libwebm::kMkvPrimaryGChromaticityX;
          chromaticity = &mm_ptr->g;
          break;
        case libwebm::kMkvPrimaryBChromaticityX:
        case libwebm::kMkvPrimaryBChromaticityY:
          is_x = child_id == libwebm::kMkvPrimaryBChromaticityX;
          chromaticity = &mm_ptr->b;
          break;
        case libwebm::kMkvWhitePointChromaticityX:
        case libwebm::kMkvWhitePointChromaticityY:
          is_x = child_id == libwebm::kMkvWhitePointChromaticityX;
          chromaticity = &mm_ptr->white_point;
          break;
        default:
          return false;
      }
      const bool value_parse_status = PrimaryChromaticity::Parse(
          reader, read_pos, child_size, is_x, chromaticity);
      if (!value_parse_status)
        return false;
    }

    read_pos += child_size;
    if (read_pos > mm_end)
      return false;
  }

  *mm = mm_ptr.release();
  return true;
}