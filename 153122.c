utf16_type_find (GstTypeFind * tf, gpointer unused)
{
  static const GstUnicodeTester utf16tester[2] = {
    {2, "\xff\xfe", check_utf16, 10, G_LITTLE_ENDIAN},
    {2, "\xfe\xff", check_utf16, 20, G_BIG_ENDIAN},
  };
  unicode_type_find (tf, utf16tester, G_N_ELEMENTS (utf16tester),
      "text/utf-16", TRUE);
}