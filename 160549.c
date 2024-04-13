unsigned LibRaw::parse_custom_cameras(unsigned limit,
                                      libraw_custom_camera_t table[],
                                      char **list)
{
  if (!list)
    return 0;
  unsigned index = 0;
  for (int i = 0; i < limit; i++)
  {
    if (!list[i])
      break;
    if (strlen(list[i]) < 10)
      continue;
    char *string = (char *)malloc(strlen(list[i]) + 1);
    strcpy(string, list[i]);
    char *start = string;
    memset(&table[index], 0, sizeof(table[0]));
    for (int j = 0; start && j < 14; j++)
    {
      char *end = strchr(start, ',');
      if (end)
      {
        *end = 0;
        end++;
      } // move to next char
      while (isspace(*start) && *start)
        start++; // skip leading spaces?
      unsigned val = strtol(start, 0, 10);
      switch (j)
      {
      case 0:
        table[index].fsize = val;
        break;
      case 1:
        table[index].rw = val;
        break;
      case 2:
        table[index].rh = val;
        break;
      case 3:
        table[index].lm = val;
        break;
      case 4:
        table[index].tm = val;
        break;
      case 5:
        table[index].rm = val;
        break;
      case 6:
        table[index].bm = val;
        break;
      case 7:
        table[index].lf = val;
        break;
      case 8:
        table[index].cf = val;
        break;
      case 9:
        table[index].max = val;
        break;
      case 10:
        table[index].flags = val;
        break;
      case 11:
        strncpy(table[index].t_make, start, sizeof(table[index].t_make) - 1);
        break;
      case 12:
        strncpy(table[index].t_model, start, sizeof(table[index].t_model) - 1);
        break;
      case 13:
        table[index].offset = val;
        break;
      default:
        break;
      }
      start = end;
    }
    free(string);
    if (table[index].t_make[0])
      index++;
  }
  return index;
}