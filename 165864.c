static inline struct hlist_nulls_head *ping_hashslot(struct ping_table *table,
					     struct net *net, unsigned int num)
{
	return &table->hash[ping_hashfn(net, num, PING_HTABLE_MASK)];
}