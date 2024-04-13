new_type_comment(const char *s, struct compiling *c)
{
  return PyUnicode_DecodeUTF8(s, strlen(s), NULL);
}