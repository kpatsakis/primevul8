load_4 (FILE    *fp,
        gint     width,
        gint     height,
        guchar  *buffer,
        guint16  bytes)
{
  gint    x, y, c;
  guchar *line = g_new (guchar, bytes);

  for (y = 0; y < height; buffer += width, ++y)
    {
      for (x = 0; x < width; ++x)
        buffer[x] = 0;
      for (c = 0; c < 4; ++c)
        {
          readline(fp, line, bytes);
          for (x = 0; x < width; ++x)
            {
              if (line[x / 8] & (128 >> (x % 8)))
                buffer[x] += (1 << c);
            }
        }
      gimp_progress_update ((double) y / (double) height);
    }

  g_free (line);
}