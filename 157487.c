std::int64_t NanosecondsTo90KhzTicks(std::int64_t nanoseconds) {
  const double pts_seconds = nanoseconds / kNanosecondsPerSecond;
  return static_cast<std::int64_t>(pts_seconds * 90000);
}