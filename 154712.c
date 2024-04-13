bool IsChromaSitingHorzValueValid(uint64_t value) {
  switch (value) {
    case mkvmuxer::Colour::kUnspecifiedCsh:
    case mkvmuxer::Colour::kLeftCollocated:
    case mkvmuxer::Colour::kHalfCsh:
      return true;
  }
  return false;
}