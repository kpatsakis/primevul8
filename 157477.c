bool Webm2Pes::InitWebmParser() {
  if (webm_reader_.Open(input_file_name_.c_str()) != 0) {
    std::fprintf(stderr, "Webm2Pes: Cannot open %s as input.\n",
                 input_file_name_.c_str());
    return false;
  }

  using mkvparser::Segment;
  Segment* webm_parser = nullptr;
  if (Segment::CreateInstance(&webm_reader_, 0 /* pos */,
                              webm_parser /* Segment*& */) != 0) {
    std::fprintf(stderr, "Webm2Pes: Cannot create WebM parser.\n");
    return false;
  }
  webm_parser_.reset(webm_parser);

  if (webm_parser_->Load() != 0) {
    std::fprintf(stderr, "Webm2Pes: Cannot parse %s.\n",
                 input_file_name_.c_str());
    return false;
  }

  // Make sure there's a video track.
  const mkvparser::Tracks* tracks = webm_parser_->GetTracks();
  if (tracks == nullptr) {
    std::fprintf(stderr, "Webm2Pes: %s has no tracks.\n",
                 input_file_name_.c_str());
    return false;
  }

  timecode_scale_ = webm_parser_->GetInfo()->GetTimeCodeScale();

  for (int track_index = 0;
       track_index < static_cast<int>(tracks->GetTracksCount());
       ++track_index) {
    const mkvparser::Track* track = tracks->GetTrackByIndex(track_index);
    if (track && track->GetType() == mkvparser::Track::kVideo) {
      if (std::string(track->GetCodecId()) == std::string("V_VP8")) {
        codec_ = VideoFrame::kVP8;
      } else if (std::string(track->GetCodecId()) == std::string("V_VP9")) {
        codec_ = VideoFrame::kVP9;
      } else {
        fprintf(stderr, "Webm2Pes: Codec must be VP8 or VP9.\n");
        return false;
      }
      video_track_num_ = track_index + 1;
      break;
    }
  }
  if (video_track_num_ < 1) {
    std::fprintf(stderr, "Webm2Pes: No video track found in %s.\n",
                 input_file_name_.c_str());
    return false;
  }
  return true;
}