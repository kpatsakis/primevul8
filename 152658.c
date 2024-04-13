void int_handler(int sig)
{
	sigint_caught = 1;
	printf("\n");	/* Skip "^C" displayed on screen */

}