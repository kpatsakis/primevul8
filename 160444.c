static inline void fuji_fill_buffer(struct fuji_compressed_block *info)
{
  if (info->cur_pos >= info->cur_buf_size)
  {
    info->cur_pos = 0;
    info->cur_buf_offset += info->cur_buf_size;
#ifdef LIBRAW_USE_OPENMP
#pragma omp critical
#endif
    {
#ifndef LIBRAW_USE_OPENMP
      info->input->lock();
#endif
      info->input->seek(info->cur_buf_offset, SEEK_SET);
      info->cur_buf_size = info->input->read(
          info->cur_buf, 1, _min(info->max_read_size, XTRANS_BUF_SIZE));
#ifndef LIBRAW_USE_OPENMP
      info->input->unlock();
#endif
      if (info->cur_buf_size < 1) // nothing read
      {
        if (info->fillbytes > 0)
        {
          int ls = _max(1, _min(info->fillbytes, XTRANS_BUF_SIZE));
          memset(info->cur_buf, 0, ls);
          info->fillbytes -= ls;
        }
        else
          throw LIBRAW_EXCEPTION_IO_EOF;
      }
      info->max_read_size -= info->cur_buf_size;
    }
  }
}