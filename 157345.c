void imap_quote_string (char *dest, size_t dlen, const char *src)
{
  _imap_quote_string (dest, dlen, src, "\"\\");
}