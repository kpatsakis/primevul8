bdat_ungetc(int ch)
{
chunking_data_left++;
return lwr_receive_ungetc(ch);
}