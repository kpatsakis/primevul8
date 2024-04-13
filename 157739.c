assert_buf_ok(buf_t *buf)
{
  tor_assert(buf);
  tor_assert(buf->magic == BUFFER_MAGIC);

  if (! buf->head) {
    tor_assert(!buf->tail);
    tor_assert(buf->datalen == 0);
  } else {
    chunk_t *ch;
    size_t total = 0;
    tor_assert(buf->tail);
    for (ch = buf->head; ch; ch = ch->next) {
      total += ch->datalen;
      tor_assert(ch->datalen <= ch->memlen);
      tor_assert(ch->data >= &ch->mem[0]);
      tor_assert(ch->data <= &ch->mem[0]+ch->memlen);
      if (ch->data == &ch->mem[0]+ch->memlen) {
        static int warned = 0;
        if (! warned) {
          log_warn(LD_BUG, "Invariant violation in buf.c related to #15083");
          warned = 1;
        }
      }
      tor_assert(ch->data+ch->datalen <= &ch->mem[0] + ch->memlen);
      if (!ch->next)
        tor_assert(ch == buf->tail);
    }
    tor_assert(buf->datalen == total);
  }
}