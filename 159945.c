static inline void fuji_read_code(struct fuji_compressed_block *info, int *data,
                                  int bits_to_read)
{
  uchar bits_left = bits_to_read;
  uchar bits_left_in_byte = 8 - (info->cur_bit & 7);
  *data = 0;
  if (!bits_to_read)
    return;
  if (bits_to_read >= bits_left_in_byte)
  {
    do
    {
      *data <<= bits_left_in_byte;
      bits_left -= bits_left_in_byte;
      *data |= info->cur_buf[info->cur_pos] & ((1 << bits_left_in_byte) - 1);
      ++info->cur_pos;
      fuji_fill_buffer(info);
      bits_left_in_byte = 8;
    } while (bits_left >= 8);
  }
  if (!bits_left)
  {
    info->cur_bit = (8 - (bits_left_in_byte & 7)) & 7;
    return;
  }
  *data <<= bits_left;
  bits_left_in_byte -= bits_left;
  *data |= ((1 << bits_left) - 1) &
           ((unsigned)info->cur_buf[info->cur_pos] >> bits_left_in_byte);
  info->cur_bit = (8 - (bits_left_in_byte & 7)) & 7;
}