read_file(FILE *fp)
{
  char *result = solv_malloc(1024);
  char *rp = result;
  int resultl = 1024;

  for (;;)
    {
      size_t rl;
      if (rp - result + 256 >= resultl)
	{
	  resultl = rp - result;
	  result = solv_realloc(result, resultl + 1024);
	  rp = result + resultl;
	  resultl += 1024;
	}
      rl = fread(rp, 1, resultl - (rp - result), fp);
      if (rl <= 0)
	{
	  *rp = 0;
	  break;
	}
      rp += rl;
    }
  return result;
}