time_print(FILE *fp, struct timestamp_control *ctl)
{
  if (!ctl->already_rendered)
    {
      ctl->len = quagga_timestamp(ctl->precision, ctl->buf, sizeof(ctl->buf));
      ctl->already_rendered = 1;
    }
  fprintf(fp, "%s ", ctl->buf);
}