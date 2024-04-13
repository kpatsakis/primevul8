_poppler_goo_string_from_utf8(const gchar *src)
{
  if (src == nullptr) {
    return nullptr;
  }

  gsize outlen;

  gchar *utf16 = g_convert (src, -1, "UTF-16BE", "UTF-8", nullptr, &outlen, nullptr);
  if (utf16 == nullptr) {
    return nullptr;
  }

  GooString *result = new GooString (utf16, outlen);
  g_free (utf16);

  if (!result->hasUnicodeMarker()) {
    result->prependUnicodeMarker();
  }

  return result;
}