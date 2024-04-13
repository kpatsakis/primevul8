make_valid_utf8 (string & str)
{
  const char *pos;
  if (!g_utf8_validate (str.c_str(), -1, &pos)) {
    PTRACE (4, "Ekiga\tTrimming invalid UTF-8 string: " << str.c_str());
    str = str.substr (0, pos - str.c_str()).append ("...");
  }
}