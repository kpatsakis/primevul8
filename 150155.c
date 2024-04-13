static void sprint_byte_with_x(char* s, unsigned int v)
{
  xsnprintf(s, 5, "\\x%02x", (v & 0377));
}