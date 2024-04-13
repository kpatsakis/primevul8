same_ugid(transport_instance *tp, address_item *addr1, address_item *addr2)
{
if (  !tp->uid_set && !tp->expand_uid
   && !tp->deliver_as_creator
   && (  testflag(addr1, af_uid_set) != testflag(addr2, af_gid_set)
      || (  testflag(addr1, af_uid_set)
         && (  addr1->uid != addr2->uid
	    || testflag(addr1, af_initgroups) != testflag(addr2, af_initgroups)
   )  )  )  )
  return FALSE;

if (  !tp->gid_set && !tp->expand_gid
   && (  testflag(addr1, af_gid_set) != testflag(addr2, af_gid_set)
      || (  testflag(addr1, af_gid_set)
         && addr1->gid != addr2->gid
   )  )  )
  return FALSE;

return TRUE;
}