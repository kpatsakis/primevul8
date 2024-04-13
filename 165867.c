	__releases(ping_table.lock)
{
	read_unlock_bh(&ping_table.lock);
}