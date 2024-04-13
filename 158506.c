deliver_make_addr(uschar *address, BOOL copy)
{
address_item *addr = store_get(sizeof(address_item));
*addr = address_defaults;
if (copy) address = string_copy(address);
addr->address = address;
addr->unique = string_copy(address);
return addr;
}