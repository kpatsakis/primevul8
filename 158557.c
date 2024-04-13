acl_checkcondition(uschar * name, condition_def * list, int end)
{
int start = 0;
while (start < end)
  {
  int mid = (start + end)/2;
  int c = Ustrcmp(name, list[mid].name);
  if (c == 0) return mid;
  if (c < 0) end = mid;
  else start = mid + 1;
  }
return -1;
}