bool IsTransferCharacteristicsValueValid(uint64_t value) {
  switch (value) {
    case mkvmuxer::Colour::kIturBt709Tc:
    case mkvmuxer::Colour::kUnspecifiedTc:
    case mkvmuxer::Colour::kReservedTc:
    case mkvmuxer::Colour::kGamma22Curve:
    case mkvmuxer::Colour::kGamma28Curve:
    case mkvmuxer::Colour::kSmpte170MTc:
    case mkvmuxer::Colour::kSmpte240MTc:
    case mkvmuxer::Colour::kLinear:
    case mkvmuxer::Colour::kLog:
    case mkvmuxer::Colour::kLogSqrt:
    case mkvmuxer::Colour::kIec6196624:
    case mkvmuxer::Colour::kIturBt1361ExtendedColourGamut:
    case mkvmuxer::Colour::kIec6196621:
    case mkvmuxer::Colour::kIturBt202010bit:
    case mkvmuxer::Colour::kIturBt202012bit:
    case mkvmuxer::Colour::kSmpteSt2084:
    case mkvmuxer::Colour::kSmpteSt4281Tc:
    case mkvmuxer::Colour::kAribStdB67Hlg:
      return true;
  }
  return false;
}