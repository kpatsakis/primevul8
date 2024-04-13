bool WriteUint8(std::uint8_t val, std::FILE* fileptr) {
  if (fileptr == nullptr)
    return false;
  return (std::fputc(val, fileptr) == val);
}