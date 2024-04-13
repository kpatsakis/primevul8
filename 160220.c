int LibRaw_bigfile_datastream::eof()
{
  LR_BF_CHK();
  return feof(f);
}