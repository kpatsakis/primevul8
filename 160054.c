int LibRaw_bigfile_datastream::read(void *ptr, size_t size, size_t nmemb)
{
  LR_BF_CHK();
  return int(fread(ptr, size, nmemb, f));
}