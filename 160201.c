LibRaw_bigfile_datastream::~LibRaw_bigfile_datastream()
{
  if (f)
    fclose(f);
}