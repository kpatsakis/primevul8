static MYSQL_BIND *alloc_bind(int num_params)
{
  MYSQL_BIND *bind;

  if (num_params)
    Newz(908, bind, (unsigned int) num_params, MYSQL_BIND);
  else
    bind= NULL;

  return bind;
}