check_message_size(transport_instance *tp, address_item *addr)
{
int rc = OK;
int size_limit;

deliver_set_expansions(addr);
size_limit = expand_string_integer(tp->message_size_limit, TRUE);
deliver_set_expansions(NULL);

if (expand_string_message)
  {
  rc = DEFER;
  addr->message = size_limit == -1
    ? string_sprintf("failed to expand message_size_limit "
      "in %s transport: %s", tp->name, expand_string_message)
    : string_sprintf("invalid message_size_limit "
      "in %s transport: %s", tp->name, expand_string_message);
  }
else if (size_limit > 0 && message_size > size_limit)
  {
  rc = FAIL;
  addr->message =
    string_sprintf("message is too big (transport limit = %d)",
      size_limit);
  }

return rc;
}