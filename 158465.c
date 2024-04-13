inet_ntoa(struct in_addr sa)
{
static uschar addr[20];
sprintf(addr, "%d.%d.%d.%d",
        (US &sa.s_addr)[0],
        (US &sa.s_addr)[1],
        (US &sa.s_addr)[2],
        (US &sa.s_addr)[3]);
  return addr;
}