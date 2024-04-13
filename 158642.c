smtp_fflush(void)
{
if (tls_in.active.sock < 0 && fflush(smtp_out) != 0) smtp_write_error = -1;
return smtp_write_error;
}