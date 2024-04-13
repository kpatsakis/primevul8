same_strings(uschar *one, uschar *two)
{
if (one == two) return TRUE;   /* Includes the case where both NULL */
if (!one || !two) return FALSE;
return (Ustrcmp(one, two) == 0);
}