const wchar_t *LibRaw_file_datastream::wfname()
{
  return wfilename.size() > 0 ? wfilename.c_str() : NULL;
}