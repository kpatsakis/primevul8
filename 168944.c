find_mech (const DBusString  *name,
           char             **allowed_mechs)
{
  int i;
  
  if (allowed_mechs != NULL &&
      !_dbus_string_array_contains ((const char**) allowed_mechs,
                                    _dbus_string_get_const_data (name)))
    return NULL;
  
  i = 0;
  while (all_mechanisms[i].mechanism != NULL)
    {      
      if (_dbus_string_equal_c_str (name,
                                    all_mechanisms[i].mechanism))

        return &all_mechanisms[i];
      
      ++i;
    }
  
  return NULL;
}