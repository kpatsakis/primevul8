load_1 (FILE    *fp,
        gint     width,
        gint     height,
        guchar  *buffer,
        guint16  bytes)
{
  gint    x, y;
  guchar *line = g_new (guchar, bytes);

  for (y = 0; y < height; buffer += width, ++y)
    {
      readline (fp, line, bytes);
      for (x = 0; x < width; ++x)
        {
          if (line[x / 8] & (128 >> (x % 8)))
            buffer[x] = 1;
          else
            buffer[x] = 0;
        }
      gimp_progress_update ((double) y / (double) height);
    }

  g_free (line);
}