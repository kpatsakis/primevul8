ushort LibRaw::sget2(uchar *s)
{
  if (order == 0x4949) /* "II" means little-endian */
    return s[0] | s[1] << 8;
  else /* "MM" means big-endian */
    return s[0] << 8 | s[1];
}