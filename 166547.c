load_8 (FILE    *fp,
        gint     width,
        gint     height,
        guchar  *buffer,
        guint16  bytes)
{
  gint    row;
  guchar *line = g_new (guchar, bytes);

  for (row = 0; row < height; buffer += width, ++row)
    {
      readline (fp, line, bytes);
      memcpy (buffer, line, width);
      gimp_progress_update ((double) row / (double) height);
    }

  g_free (line);
}