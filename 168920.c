get_word (const DBusString *str,
          int              *start,
          DBusString       *word)
{
  int i;

  _dbus_string_skip_blank (str, *start, start);
  _dbus_string_find_blank (str, *start, &i);
  
  if (i > *start)
    {
      if (!_dbus_string_copy_len (str, *start, i - *start, word, 0))
        return FALSE;
      
      *start = i;
    }

  return TRUE;
}