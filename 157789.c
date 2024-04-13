socks_request_new(void)
{
  return tor_malloc_zero(sizeof(socks_request_t));
}