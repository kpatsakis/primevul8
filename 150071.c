static void sprint_byte(char* s, unsigned int v)
{
  xsnprintf(s, 3, "%02x", (v & 0377));
}