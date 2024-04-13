int LibRaw_file_datastream::eof()
{
  LR_STREAM_CHK();
  return f->sgetc() == EOF;
}