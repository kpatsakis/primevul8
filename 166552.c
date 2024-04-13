save_8 (FILE         *fp,
        gint          width,
        gint          height,
        const guchar *buffer)
{
  int row;

  for (row = 0; row < height; ++row)
    {
      writeline (fp, buffer, width);
      buffer += width;
      gimp_progress_update ((double) row / (double) height);
    }
}