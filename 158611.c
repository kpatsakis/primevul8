find_control(const uschar * name, control_def * ol, int last)
{
int first = 0;
while (last > first)
  {
  int middle = (first + last)/2;
  uschar * s =  ol[middle].name;
  int c = Ustrncmp(name, s, Ustrlen(s));
  if (c == 0) return middle;
  else if (c > 0) first = middle + 1;
  else last = middle;
  }
return -1;
}