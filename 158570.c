bdat_flush_data(void)
{
while (chunking_data_left)
  {
  unsigned n = chunking_data_left;
  if (!bdat_getbuf(&n)) break;
  }

receive_getc = lwr_receive_getc;
receive_getbuf = lwr_receive_getbuf;
receive_ungetc = lwr_receive_ungetc;

if (chunking_state != CHUNKING_LAST)
  {
  chunking_state = CHUNKING_OFFERED;
  DEBUG(D_receive) debug_printf("chunking state %d\n", (int)chunking_state);
  }
}