bbuf_init(BBuf* buf, int size)
{
  if (size <= 0) {
    size   = 0;
    buf->p = NULL;
  }
  else {
    buf->p = (UChar* )xmalloc(size);
    if (IS_NULL(buf->p)) return(ONIGERR_MEMORY);
  }

  buf->alloc = size;
  buf->used  = 0;
  return 0;
}