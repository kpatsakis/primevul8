polkit_system_bus_name_equal (PolkitSubject *a,
                              PolkitSubject *b)
{
  PolkitSystemBusName *name_a;
  PolkitSystemBusName *name_b;

  name_a = POLKIT_SYSTEM_BUS_NAME (a);
  name_b = POLKIT_SYSTEM_BUS_NAME (b);

  return strcmp (name_a->name, name_b->name) == 0;
}