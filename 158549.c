smtp_ungetc(int ch)
{
*--smtp_inptr = ch;
return ch;
}