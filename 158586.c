same_headers(header_line *one, header_line *two)
{
for (;; one = one->next, two = two->next)
  {
  if (one == two) return TRUE;   /* Includes the case where both NULL */
  if (!one || !two) return FALSE;
  if (Ustrcmp(one->text, two->text) != 0) return FALSE;
  }
}