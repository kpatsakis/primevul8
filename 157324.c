int imap_mxcmp (const char* mx1, const char* mx2)
{
  char* b1;
  char* b2;
  int rc;

  if (!mx1 || !*mx1)
    mx1 = "INBOX";
  if (!mx2 || !*mx2)
    mx2 = "INBOX";
  if (!ascii_strcasecmp (mx1, "INBOX") && !ascii_strcasecmp (mx2, "INBOX"))
    return 0;

  b1 = safe_malloc (strlen (mx1) + 1);
  b2 = safe_malloc (strlen (mx2) + 1);

  imap_fix_path (NULL, mx1, b1, strlen (mx1) + 1);
  imap_fix_path (NULL, mx2, b2, strlen (mx2) + 1);

  rc = mutt_strcmp (b1, b2);
  FREE (&b1);
  FREE (&b2);

  return rc;
}