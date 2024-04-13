swallow_until_crlf(int fd, uschar *base, int already, int capacity)
{
uschar *to = base + already;
uschar *cr;
int have = 0;
int ret;
int last = 0;

/* For "PROXY UNKNOWN\r\n" we, at time of writing, expect to have read
up through the \r; for the _normal_ case, we haven't yet seen the \r. */

cr = memchr(base, '\r', already);
if (cr != NULL)
  {
  if ((cr - base) < already - 1)
    {
    /* \r and presumed \n already within what we have; probably not
    actually proxy protocol, but abort cleanly. */
    return 0;
    }
  /* \r is last character read, just need one more. */
  last = 1;
  }

while (capacity > 0)
  {
  do { ret = recv(fd, to, 1, 0); } while (ret == -1 && errno == EINTR);
  if (ret == -1)
    return -1;
  have++;
  if (last)
    return have;
  if (*to == '\r')
    last = 1;
  capacity--;
  to++;
  }

/* reached end without having room for a final newline, abort */
errno = EOVERFLOW;
return -1;
}