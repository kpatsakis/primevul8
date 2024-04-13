int LibRaw_file_datastream::seek(INT64 o, int whence)
{
  LR_STREAM_CHK();
  std::ios_base::seekdir dir;
  switch (whence)
  {
  case SEEK_SET:
    dir = std::ios_base::beg;
    break;
  case SEEK_CUR:
    dir = std::ios_base::cur;
    break;
  case SEEK_END:
    dir = std::ios_base::end;
    break;
  default:
    dir = std::ios_base::beg;
  }
  return f->pubseekoff((long)o, dir) < 0;
}