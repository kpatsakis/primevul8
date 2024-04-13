bool Projection::Parse(IMkvReader* reader, long long start, long long size,
                       Projection** projection) {
  if (!reader || *projection)
    return false;

  std::unique_ptr<Projection> projection_ptr(new Projection());
  if (!projection_ptr.get())
    return false;

  const long long end = start + size;
  long long read_pos = start;

  while (read_pos < end) {
    long long child_id = 0;
    long long child_size = 0;

    const long long status =
        ParseElementHeader(reader, read_pos, end, child_id, child_size);
    if (status < 0)
      return false;

    if (child_id == libwebm::kMkvProjectionType) {
      long long projection_type = kTypeNotPresent;
      projection_type = UnserializeUInt(reader, read_pos, child_size);
      if (projection_type < 0)
        return false;

      projection_ptr->type = static_cast<ProjectionType>(projection_type);
    } else if (child_id == libwebm::kMkvProjectionPrivate) {
      unsigned char* data = SafeArrayAlloc<unsigned char>(1, child_size);

      if (data == NULL)
        return false;

      const int status =
          reader->Read(read_pos, static_cast<long>(child_size), data);

      if (status) {
        delete[] data;
        return false;
      }

      projection_ptr->private_data = data;
      projection_ptr->private_data_length = static_cast<size_t>(child_size);
    } else {
      double value = 0;
      const long long value_parse_status =
          UnserializeFloat(reader, read_pos, child_size, value);
      // Make sure value is representable as a float before casting.
      if (value_parse_status < 0 || value < -FLT_MAX || value > FLT_MAX ||
          (value > 0.0 && value < FLT_MIN)) {
        return false;
      }

      switch (child_id) {
        case libwebm::kMkvProjectionPoseYaw:
          projection_ptr->pose_yaw = static_cast<float>(value);
          break;
        case libwebm::kMkvProjectionPosePitch:
          projection_ptr->pose_pitch = static_cast<float>(value);
          break;
        case libwebm::kMkvProjectionPoseRoll:
          projection_ptr->pose_roll = static_cast<float>(value);
          break;
        default:
          return false;
      }
    }

    read_pos += child_size;
    if (read_pos > end)
      return false;
  }

  *projection = projection_ptr.release();
  return true;
}