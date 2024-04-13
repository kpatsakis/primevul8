default_handle(
/*===========*/
	struct innodb_engine*	eng)
{
	return((struct default_engine*) eng->default_engine);
}