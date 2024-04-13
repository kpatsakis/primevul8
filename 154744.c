bool Colour::Parse(IMkvReader* reader, long long colour_start,
                   long long colour_size, Colour** colour) {
  if (!reader || *colour)
    return false;

  std::unique_ptr<Colour> colour_ptr(new Colour());
  if (!colour_ptr.get())
    return false;

  const long long colour_end = colour_start + colour_size;
  long long read_pos = colour_start;

  while (read_pos < colour_end) {
    long long child_id = 0;
    long long child_size = 0;

    const long status =
        ParseElementHeader(reader, read_pos, colour_end, child_id, child_size);
    if (status < 0)
      return false;

    if (child_id == libwebm::kMkvMatrixCoefficients) {
      colour_ptr->matrix_coefficients =
          UnserializeUInt(reader, read_pos, child_size);
      if (colour_ptr->matrix_coefficients < 0)
        return false;
    } else if (child_id == libwebm::kMkvBitsPerChannel) {
      colour_ptr->bits_per_channel =
          UnserializeUInt(reader, read_pos, child_size);
      if (colour_ptr->bits_per_channel < 0)
        return false;
    } else if (child_id == libwebm::kMkvChromaSubsamplingHorz) {
      colour_ptr->chroma_subsampling_horz =
          UnserializeUInt(reader, read_pos, child_size);
      if (colour_ptr->chroma_subsampling_horz < 0)
        return false;
    } else if (child_id == libwebm::kMkvChromaSubsamplingVert) {
      colour_ptr->chroma_subsampling_vert =
          UnserializeUInt(reader, read_pos, child_size);
      if (colour_ptr->chroma_subsampling_vert < 0)
        return false;
    } else if (child_id == libwebm::kMkvCbSubsamplingHorz) {
      colour_ptr->cb_subsampling_horz =
          UnserializeUInt(reader, read_pos, child_size);
      if (colour_ptr->cb_subsampling_horz < 0)
        return false;
    } else if (child_id == libwebm::kMkvCbSubsamplingVert) {
      colour_ptr->cb_subsampling_vert =
          UnserializeUInt(reader, read_pos, child_size);
      if (colour_ptr->cb_subsampling_vert < 0)
        return false;
    } else if (child_id == libwebm::kMkvChromaSitingHorz) {
      colour_ptr->chroma_siting_horz =
          UnserializeUInt(reader, read_pos, child_size);
      if (colour_ptr->chroma_siting_horz < 0)
        return false;
    } else if (child_id == libwebm::kMkvChromaSitingVert) {
      colour_ptr->chroma_siting_vert =
          UnserializeUInt(reader, read_pos, child_size);
      if (colour_ptr->chroma_siting_vert < 0)
        return false;
    } else if (child_id == libwebm::kMkvRange) {
      colour_ptr->range = UnserializeUInt(reader, read_pos, child_size);
      if (colour_ptr->range < 0)
        return false;
    } else if (child_id == libwebm::kMkvTransferCharacteristics) {
      colour_ptr->transfer_characteristics =
          UnserializeUInt(reader, read_pos, child_size);
      if (colour_ptr->transfer_characteristics < 0)
        return false;
    } else if (child_id == libwebm::kMkvPrimaries) {
      colour_ptr->primaries = UnserializeUInt(reader, read_pos, child_size);
      if (colour_ptr->primaries < 0)
        return false;
    } else if (child_id == libwebm::kMkvMaxCLL) {
      colour_ptr->max_cll = UnserializeUInt(reader, read_pos, child_size);
      if (colour_ptr->max_cll < 0)
        return false;
    } else if (child_id == libwebm::kMkvMaxFALL) {
      colour_ptr->max_fall = UnserializeUInt(reader, read_pos, child_size);
      if (colour_ptr->max_fall < 0)
        return false;
    } else if (child_id == libwebm::kMkvMasteringMetadata) {
      if (!MasteringMetadata::Parse(reader, read_pos, child_size,
                                    &colour_ptr->mastering_metadata))
        return false;
    } else {
      return false;
    }

    read_pos += child_size;
    if (read_pos > colour_end)
      return false;
  }
  *colour = colour_ptr.release();
  return true;
}