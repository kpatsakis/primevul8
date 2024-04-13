char *LibRaw_bigfile_datastream::gets(char *str, int sz)
{
  LR_BF_CHK();
  return fgets(str, sz, f);
}