int LibRaw_file_datastream::scanf_one(const char *fmt, void *val)
{
  LR_STREAM_CHK();

  std::istream is(f.get());

  /* HUGE ASSUMPTION: *fmt is either "%d" or "%f" */
  if (strcmp(fmt, "%d") == 0)
  {
    int d;
    is >> d;
    if (is.fail())
      return EOF;
    *(static_cast<int *>(val)) = d;
  }
  else
  {
    float f;
    is >> f;
    if (is.fail())
      return EOF;
    *(static_cast<float *>(val)) = f;
  }

  return 1;
}