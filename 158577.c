smtp_ferror(void)
{
errno = smtp_had_error;
return smtp_had_error;
}