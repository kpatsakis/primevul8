bool Webm2Pes::ReadVideoFrame(const mkvparser::Block::Frame& mkvparser_frame,
                              VideoFrame* frame) {
  if (mkvparser_frame.len < 1 || frame == nullptr)
    return false;

  const std::size_t mkv_len = static_cast<std::size_t>(mkvparser_frame.len);
  if (mkv_len > frame->buffer().capacity) {
    const std::size_t new_size = 2 * mkv_len;
    if (frame->Init(new_size) == false) {
      std::fprintf(stderr, "Webm2Pes: Out of memory.\n");
      return false;
    }
  }
  if (mkvparser_frame.Read(&webm_reader_, frame->buffer().data.get()) != 0) {
    std::fprintf(stderr, "Webm2Pes: Error reading VPx frame!\n");
    return false;
  }
  return frame->SetBufferLength(mkv_len);
}