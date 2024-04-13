save_24 (FILE         *fp,
         gint          width,
         gint          height,
         const guchar *buffer)
{
  int     x, y, c;
  guchar *line;

  line = (guchar *) g_malloc (width);

  for (y = 0; y < height; ++y)
    {
      for (c = 0; c < 3; ++c)
        {
          for (x = 0; x < width; ++x)
            {
              line[x] = buffer[(3*x) + c];
            }
          writeline (fp, line, width);
        }
      buffer += width * 3;
      gimp_progress_update ((double) y / (double) height);
    }
  g_free (line);
}