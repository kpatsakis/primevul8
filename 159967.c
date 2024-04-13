int LibRaw_buffer_datastream::scanf_one(const char *fmt, void *val)
{
  int scanf_res;
  if (streampos > streamsize)
    return 0;
#ifndef WIN32SECURECALLS
  scanf_res = sscanf((char *)(buf + streampos), fmt, val);
#else
  scanf_res = sscanf_s((char *)(buf + streampos), fmt, val);
#endif
  if (scanf_res > 0)
  {
    int xcnt = 0;
    while (streampos < streamsize)
    {
      streampos++;
      xcnt++;
      if (buf[streampos] == 0 || buf[streampos] == ' ' ||
          buf[streampos] == '\t' || buf[streampos] == '\n' || xcnt > 24)
        break;
    }
  }
  return scanf_res;
}