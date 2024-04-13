static void free_bind(MYSQL_BIND *bind)
{
  if (bind)
    Safefree(bind);
}