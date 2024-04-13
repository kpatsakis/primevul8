swab_rect (guint32 *rect)
{
  rect[0] = GUINT32_FROM_LE (rect[0]);
  rect[1] = GUINT32_FROM_LE (rect[1]);
  rect[2] = GUINT32_FROM_LE (rect[2]);
  rect[3] = GUINT32_FROM_LE (rect[3]);
}