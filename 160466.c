const wchar_t *LibRaw_bigfile_datastream::wfname()
{
  return wfilename.size() > 0 ? wfilename.c_str() : NULL;
}