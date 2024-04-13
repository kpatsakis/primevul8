static inline void fuji_zerobits(struct fuji_compressed_block *info, int *count)
{
  uchar zero = 0;
  *count = 0;
  while (zero == 0)
  {
    zero = (info->cur_buf[info->cur_pos] >> (7 - info->cur_bit)) & 1;
    info->cur_bit++;
    info->cur_bit &= 7;
    if (!info->cur_bit)
    {
      ++info->cur_pos;
      fuji_fill_buffer(info);
    }
    if (zero)
      break;
    ++*count;
  }
}