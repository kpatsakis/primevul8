strip_special_chars (std::string& str, char* special_chars, bool start)
{
  std::string::size_type idx;

  unsigned i = 0;
  while (i < strlen (special_chars)) {
    idx = str.find_first_of (special_chars[i]);
    if (idx != std::string::npos) {
      if (start)
        str = str.substr (idx+1);
      else
        str = str.substr (0, idx);
    }
    i++;
  }
}