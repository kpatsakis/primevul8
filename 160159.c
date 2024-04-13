const char *LibRaw_bigfile_datastream::fname()
{
  return filename.size() > 0 ? filename.c_str() : NULL;
}