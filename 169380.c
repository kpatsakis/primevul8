static void tag_dirty(PgSocket *sk)
{
	sk->close_needed = 1;
}