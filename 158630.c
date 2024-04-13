replicate_status(address_item *addr)
{
address_item *addr2;
for (addr2 = addr->next; addr2; addr2 = addr2->next)
  {
  addr2->transport =	    addr->transport;
  addr2->transport_return = addr->transport_return;
  addr2->basic_errno =	    addr->basic_errno;
  addr2->more_errno =	    addr->more_errno;
  addr2->delivery_usec =    addr->delivery_usec;
  addr2->special_action =   addr->special_action;
  addr2->message =	    addr->message;
  addr2->user_message =	    addr->user_message;
  }
}