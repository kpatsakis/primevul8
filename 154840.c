bool IsPrimariesValueValid(uint64_t value) {
  switch (value) {
    case mkvmuxer::Colour::kReservedP0:
    case mkvmuxer::Colour::kIturBt709P:
    case mkvmuxer::Colour::kUnspecifiedP:
    case mkvmuxer::Colour::kReservedP3:
    case mkvmuxer::Colour::kIturBt470M:
    case mkvmuxer::Colour::kIturBt470Bg:
    case mkvmuxer::Colour::kSmpte170MP:
    case mkvmuxer::Colour::kSmpte240MP:
    case mkvmuxer::Colour::kFilm:
    case mkvmuxer::Colour::kIturBt2020:
    case mkvmuxer::Colour::kSmpteSt4281P:
    case mkvmuxer::Colour::kJedecP22Phosphors:
      return true;
  }
  return false;
}