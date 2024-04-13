writeline (FILE         *fp,
           const guchar *buffer,
           gint          bytes)
{
  const guchar *finish = buffer + bytes;
  guchar        value;
  guchar        count;

  while (buffer < finish)
    {
      value = *(buffer++);
      count = 1;

      while (buffer < finish && count < 63 && *buffer == value)
        {
          count++; buffer++;
        }

      if (value < 0xc0 && count == 1)
        {
          fputc (value, fp);
        }
      else
        {
          fputc (0xc0 + count, fp);
          fputc (value, fp);
        }
    }
}