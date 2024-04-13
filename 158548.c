transport_init(void)
{
transport_instance *t;

readconf_driver_init(US"transport",
  (driver_instance **)(&transports),     /* chain anchor */
  (driver_info *)transports_available,   /* available drivers */
  sizeof(transport_info),                /* size of info block */
  &transport_defaults,                   /* default values for generic options */
  sizeof(transport_instance),            /* size of instance block */
  optionlist_transports,                 /* generic options */
  optionlist_transports_size);

/* Now scan the configured transports and check inconsistencies. A shadow
transport is permitted only for local transports. */

for (t = transports; t; t = t->next)
  {
  if (!t->info->local && t->shadow)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "shadow transport not allowed on non-local transport %s", t->name);

  if (t->body_only && t->headers_only)
    log_write(0, LOG_PANIC_DIE|LOG_CONFIG,
      "%s transport: body_only and headers_only are mutually exclusive",
      t->name);
  }
}