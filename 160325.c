static void fuji_extend_generic(ushort *linebuf[_ltotal], int line_width,
                                int start, int end)
{
  for (int i = start; i <= end; i++)
  {
    linebuf[i][0] = linebuf[i - 1][1];
    linebuf[i][line_width + 1] = linebuf[i - 1][line_width];
  }
}