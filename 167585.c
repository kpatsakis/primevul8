poppler_password_to_latin1 (const gchar *password)
{
  gchar *password_latin;
  GooString *password_g;

  if (!password)
    return nullptr;

  password_latin = g_convert(password, -1, "ISO-8859-1", "UTF-8",
                             nullptr, nullptr, nullptr);
  password_g = new GooString (password_latin);
  g_free (password_latin);

  return password_g;
}