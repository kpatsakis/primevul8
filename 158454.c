continue_closedown(void)
{
if (continue_transport)
  {
  transport_instance *t;
  for (t = transports; t; t = t->next)
    if (Ustrcmp(t->name, continue_transport) == 0)
      {
      if (t->info->closedown) (t->info->closedown)(t);
      break;
      }
  }
return DELIVER_NOT_ATTEMPTED;
}