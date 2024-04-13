void LibRaw::merror(void *ptr, const char *where)
{
  if (ptr)
    return;
  if (callbacks.mem_cb)
    (*callbacks.mem_cb)(callbacks.memcb_data,
                        libraw_internal_data.internal_data.input
                            ? libraw_internal_data.internal_data.input->fname()
                            : NULL,
                        where);
  throw LIBRAW_EXCEPTION_ALLOC;
}