char *LibRaw_file_datastream::gets(char *str, int sz)
{
  LR_STREAM_CHK();
  std::istream is(f.get());
  is.getline(str, sz);
  if (is.fail())
    return 0;
  return str;
}