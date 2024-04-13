ushort LibRaw::sget2Rev(uchar *s) // specific to some Canon Makernotes fields,
                                  // where they have endian in reverse
{
  if (order == 0x4d4d) /* "II" means little-endian, and we reverse to "MM" - big
                          endian */
    return s[0] | s[1] << 8;
  else /* "MM" means big-endian... */
    return s[0] << 8 | s[1];
}