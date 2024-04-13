bdat_getbuf(unsigned * len)
{
uschar * buf;

if (chunking_data_left <= 0)
  { *len = 0; return NULL; }

if (*len > chunking_data_left) *len = chunking_data_left;
buf = lwr_receive_getbuf(len);	/* Either smtp_getbuf or tls_getbuf */
chunking_data_left -= *len;
return buf;
}