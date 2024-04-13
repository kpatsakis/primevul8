bool IsChromaSitingVertValueValid(uint64_t value) {
  switch (value) {
    case mkvmuxer::Colour::kUnspecifiedCsv:
    case mkvmuxer::Colour::kTopCollocated:
    case mkvmuxer::Colour::kHalfCsv:
      return true;
  }
  return false;
}