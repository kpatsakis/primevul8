read_inline_file(FILE *fp, char **bufp, char **bufpp, int *buflp)
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
      if (!fgets(rp, resultl - (rp - result), fp))
	*rp = 0;
      rl = strlen(rp);
      if (rl && (rp == result || rp[-1] == '\n'))
	{
	  if (rl > 1 && rp[0] == '#' && rp[1] == '>')
	    {
	      memmove(rp, rp + 2, rl - 2);
	      rl -= 2;
	    }
	  else
	    {
	      while (rl + 16 > *buflp)
		{
		  *bufp = solv_realloc(*bufp, *buflp + 512);
		  *buflp += 512;
		}
	      memmove(*bufp, rp, rl);
	      if ((*bufp)[rl - 1] == '\n')
		{
		  ungetc('\n', fp);
		  rl--;
		}
	      (*bufp)[rl] = 0;
	      (*bufpp) = *bufp + rl;
	      rl = 0;
	    }
	}
      if (rl <= 0)
	{
	  *rp = 0;
	  break;
	}
      rp += rl;
    }
  return result;
}