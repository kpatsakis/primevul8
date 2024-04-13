void set_worker_desc(const char *fmt, ...)
{
	struct worker *worker = current_wq_worker();
	va_list args;

	if (worker) {
		va_start(args, fmt);
		vsnprintf(worker->desc, sizeof(worker->desc), fmt, args);
		va_end(args);
		worker->desc_valid = true;
	}
}