compression_name (gint compression)
{
  switch (compression)
    {
    case PSP_COMP_NONE:
      return "no compression";
    case PSP_COMP_RLE:
      return "RLE";
    case PSP_COMP_LZ77:
      return "LZ77";
    }
  g_assert_not_reached ();

  return NULL;
}