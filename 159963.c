int show_files(void *q)
{
  char *p;
  int cnt = 0;
  while (p = get_next_file())
  {
    printf("%s\n", p);
    cnt++;
  }
  return cnt;
}