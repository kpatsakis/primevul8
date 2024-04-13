bool IsMatrixCoefficientsValueValid(uint64_t value) {
  switch (value) {
    case mkvmuxer::Colour::kGbr:
    case mkvmuxer::Colour::kBt709:
    case mkvmuxer::Colour::kUnspecifiedMc:
    case mkvmuxer::Colour::kReserved:
    case mkvmuxer::Colour::kFcc:
    case mkvmuxer::Colour::kBt470bg:
    case mkvmuxer::Colour::kSmpte170MMc:
    case mkvmuxer::Colour::kSmpte240MMc:
    case mkvmuxer::Colour::kYcocg:
    case mkvmuxer::Colour::kBt2020NonConstantLuminance:
    case mkvmuxer::Colour::kBt2020ConstantLuminance:
      return true;
  }
  return false;
}