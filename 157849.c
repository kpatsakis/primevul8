new_type_comment(const char *s)
{
  return PyUnicode_DecodeUTF8(s, strlen(s), NULL);
}