bool Webm2Pes::ConvertToPacketReceiver() {
  if (input_file_name_.empty() || packet_sink_ == nullptr) {
    std::fprintf(stderr, "Webm2Pes: input file name empty or null sink.\n");
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
          VideoFrame frame(block->GetTime(cluster), codec_);
          if (ReadVideoFrame(mkvparser_frame, &frame) == false) {
            fprintf(stderr, "Webm2Pes: frame read failed.\n");
            return false;
          }

          // Write frame out as PES packet(s).
          if (WritePesPacket(frame, &packet_data_) == false) {
            std::fprintf(stderr, "Webm2Pes: WritePesPacket failed.\n");
            return false;
          }
          if (packet_sink_->ReceivePacket(packet_data_) != true) {
            std::fprintf(stderr, "Webm2Pes: ReceivePacket failed.\n");
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

  return true;
}