std::int64_t Khz90TicksToNanoseconds(std::int64_t ticks) {
  const double seconds = ticks / 90000.0;
  return static_cast<std::int64_t>(seconds * kNanosecondsPerSecond);
}