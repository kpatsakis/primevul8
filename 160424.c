static void fuji_decode_interpolation_even(int line_width, ushort *line_buf,
                                           int pos)
{
  ushort *line_buf_cur = line_buf + pos;
  int Rb = line_buf_cur[-2 - line_width];
  int Rc = line_buf_cur[-3 - line_width];
  int Rd = line_buf_cur[-1 - line_width];
  int Rf = line_buf_cur[-4 - 2 * line_width];
  int diffRcRb = _abs(Rc - Rb);
  int diffRfRb = _abs(Rf - Rb);
  int diffRdRb = _abs(Rd - Rb);
  if (diffRcRb > diffRfRb && diffRcRb > diffRdRb)
    *line_buf_cur = (Rf + Rd + 2 * Rb) >> 2;
  else if (diffRdRb > diffRcRb && diffRdRb > diffRfRb)
    *line_buf_cur = (Rf + Rc + 2 * Rb) >> 2;
  else
    *line_buf_cur = (Rd + Rc + 2 * Rb) >> 2;
}