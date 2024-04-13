bool Webm2Pes::ConvertToFile() {
  if (input_file_name_.empty() || output_file_name_.empty()) {
    std::fprintf(stderr, "Webm2Pes: input and/or output file name(s) empty.\n");
    return false;
  }

  output_file_ = FilePtr(fopen(output_file_name_.c_str(), "wb"), FILEDeleter());
  if (output_file_ == nullptr) {
    std::fprintf(stderr, "Webm2Pes: Cannot open %s for output.\n",
                 output_file_name_.c_str());
    return false;
  }

  if (InitWebmParser() != true) {
    std::fprintf(stderr, "Webm2Pes: Cannot initialize WebM parser.\n");
    return false;
  }

  // Walk clusters in segment.
  const mkvparser::Cluster* cluster = webm_parser_->GetFirst();
  while (cluster != nullptr && cluster->EOS() == false) {
    const mkvparser::BlockEntry* block_entry = nullptr;
    std::int64_t block_status = cluster->GetFirst(block_entry);
    if (block_status < 0) {
      std::fprintf(stderr, "Webm2Pes: Cannot parse first block in %s.\n",
                   input_file_name_.c_str());
      return false;
    }

    // Walk blocks in cluster.
    while (block_entry != nullptr && block_entry->EOS() == false) {
      const mkvparser::Block* block = block_entry->GetBlock();
      if (block->GetTrackNumber() == video_track_num_) {
        const int frame_count = block->GetFrameCount();

        // Walk frames in block.
        for (int frame_num = 0; frame_num < frame_count; ++frame_num) {
          const mkvparser::Block::Frame& mkvparser_frame =
              block->GetFrame(frame_num);

          // Read the frame.
          VideoFrame vpx_frame(block->GetTime(cluster), codec_);
          if (ReadVideoFrame(mkvparser_frame, &vpx_frame) == false) {
            fprintf(stderr, "Webm2Pes: frame read failed.\n");
            return false;
          }

          // Write frame out as PES packet(s).
          if (WritePesPacket(vpx_frame, &packet_data_) == false) {
            std::fprintf(stderr, "Webm2Pes: WritePesPacket failed.\n");
            return false;
          }

          // Write contents of |packet_data_| to |output_file_|.
          if (std::fwrite(&packet_data_[0], 1, packet_data_.size(),
                          output_file_.get()) != packet_data_.size()) {
            std::fprintf(stderr, "Webm2Pes: packet payload write failed.\n");
            return false;
          }
          bytes_written_ += packet_data_.size();
        }
      }
      block_status = cluster->GetNext(block_entry, block_entry);
      if (block_status < 0) {
        std::fprintf(stderr, "Webm2Pes: Cannot parse block in %s.\n",
                     input_file_name_.c_str());
        return false;
      }
    }

    cluster = webm_parser_->GetNext(cluster);
  }

  std::fflush(output_file_.get());
  return true;
}