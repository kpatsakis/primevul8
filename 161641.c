kthread_create_worker_on_cpu(int cpu, unsigned int flags,
			     const char namefmt[], ...)
{
	struct kthread_worker *worker;
	va_list args;

	va_start(args, namefmt);
	worker = __kthread_create_worker(cpu, flags, namefmt, args);
	va_end(args);

	return worker;
}