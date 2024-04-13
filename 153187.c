utf32_type_find (GstTypeFind * tf, gpointer unused)
{
  static const GstUnicodeTester utf32tester[2] = {
    {4, "\xff\xfe\x00\x00", check_utf32, 10, G_LITTLE_ENDIAN},
    {4, "\x00\x00\xfe\xff", check_utf32, 20, G_BIG_ENDIAN}
  };
  unicode_type_find (tf, utf32tester, G_N_ELEMENTS (utf32tester),
      "text/utf-32", TRUE);
}