bool IsColourRangeValueValid(uint64_t value) {
  switch (value) {
    case mkvmuxer::Colour::kUnspecifiedCr:
    case mkvmuxer::Colour::kBroadcastRange:
    case mkvmuxer::Colour::kFullRange:
    case mkvmuxer::Colour::kMcTcDefined:
      return true;
  }
  return false;
}