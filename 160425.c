int LibRaw::is_jpeg_thumb()
{
  return thumb_load_raw == 0 && write_thumb == &LibRaw::jpeg_thumb;
}