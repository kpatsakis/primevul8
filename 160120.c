LibRaw_file_datastream::~LibRaw_file_datastream()
{
  if (jas_file)
    fclose(jas_file);
}