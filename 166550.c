load_24 (FILE    *fp,
         gint     width,
         gint     height,
         guchar  *buffer,
         guint16  bytes)
{
  gint    x, y, c;
  guchar *line = g_new (guchar, bytes);

  for (y = 0; y < height; buffer += width * 3, ++y)
    {
      for (c = 0; c < 3; ++c)
        {
          readline (fp, line, bytes);
          for (x = 0; x < width; ++x)
            {
              buffer[x * 3 + c] = line[x];
            }
        }
      gimp_progress_update ((double) y / (double) height);
    }

  g_free (line);
}