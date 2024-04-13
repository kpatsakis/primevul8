int LibRaw::parseLeicaLensName(unsigned len)
{
#define plln ilm.Lens
  if (!len)
  {
    strcpy(plln, "N/A");
    return 0;
  }
  stmread(plln, len, ifp);
  if ((plln[0] == ' ') || !strncasecmp(plln, "not ", 4) ||
      !strncmp(plln, "---", 3) || !strncmp(plln, "***", 3))
  {
    strcpy(plln, "N/A");
    return 0;
  }
  else
    return 1;
#undef plln
}