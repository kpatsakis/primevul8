INT64 LibRaw_file_datastream::tell()
{
  LR_STREAM_CHK();
  return f->pubseekoff(0, std::ios_base::cur);
}