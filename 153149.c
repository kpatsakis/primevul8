q3gp_type_find_get_profile (const guint8 * data)
{
  switch (GST_MAKE_FOURCC (data[0], data[1], data[2], 0)) {
    case GST_MAKE_FOURCC ('3', 'g', 'g', 0):
      return "general";
    case GST_MAKE_FOURCC ('3', 'g', 'p', 0):
      return "basic";
    case GST_MAKE_FOURCC ('3', 'g', 's', 0):
      return "streaming-server";
    case GST_MAKE_FOURCC ('3', 'g', 'r', 0):
      return "progressive-download";
    default:
      break;
  }
  return NULL;
}