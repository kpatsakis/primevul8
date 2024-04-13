void imap_quote_string_and_backquotes (char *dest, size_t dlen, const char *src)
{
  _imap_quote_string (dest, dlen, src, "\"\\`");
}