MOCK_IMPL(size_t,
buf_datalen, (const buf_t *buf))
{
  return buf->datalen;
}