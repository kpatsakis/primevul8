readline (FILE   *fp,
          guchar *buffer,
          gint    bytes)
{
  static guchar count = 0, value = 0;

  if (pcx_header.compression)
    {
      while (bytes--)
        {
          if (count == 0)
            {
              value = fgetc (fp);
              if (value < 0xc0)
                {
                  count = 1;
                }
              else
                {
                  count = value - 0xc0;
                  value = fgetc (fp);
                }
            }
          count--;
          *(buffer++) = value;
        }
    }
  else
    {
      fread (buffer, bytes, 1, fp);
    }
}