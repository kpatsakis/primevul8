exim_errstr(int err)
{
return err < 0 ? exim_errstrings[-err] : CUS strerror(err);
}