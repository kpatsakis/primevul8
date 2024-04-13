kthread_create_worker(unsigned int flags, const char namefmt[], ...)
{
	struct kthread_worker *worker;
	va_list args;

	va_start(args, namefmt);
	worker = __kthread_create_worker(-1, flags, namefmt, args);
	va_end(args);

	return worker;
}