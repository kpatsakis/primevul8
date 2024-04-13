FileFormat ClassifyFileFormat(StringPiece data) {
  if (absl::StartsWith(data, kJpegMagicBytes)) return kJpgFormat;
  if (absl::StartsWith(data, kPngMagicBytes)) return kPngFormat;
  if (absl::StartsWith(data, kGifMagicBytes)) return kGifFormat;
  if (absl::StartsWith(data, kBmpMagicBytes)) return kBmpFormat;
  return kUnknownFormat;
}