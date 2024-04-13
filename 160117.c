const char *LibRaw_file_datastream::fname()
{
  return filename.size() > 0 ? filename.c_str() : NULL;
}