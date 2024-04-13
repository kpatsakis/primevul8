acl_checkname(uschar *name, uschar **list, int end)
{
int start = 0;

while (start < end)
  {
  int mid = (start + end)/2;
  int c = Ustrcmp(name, list[mid]);
  if (c == 0) return mid;
  if (c < 0) end = mid; else start = mid + 1;
  }

return -1;
}