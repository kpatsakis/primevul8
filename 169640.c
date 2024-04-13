perf_output_sample_ustack(struct perf_output_handle *handle, u64 dump_size,
			  struct pt_regs *regs)
{
	/* Case of a kernel thread, nothing to dump */
	if (!regs) {
		u64 size = 0;
		perf_output_put(handle, size);
	} else {
		unsigned long sp;
		unsigned int rem;
		u64 dyn_size;
		mm_segment_t fs;

		/*
		 * We dump:
		 * static size
		 *   - the size requested by user or the best one we can fit
		 *     in to the sample max size
		 * data
		 *   - user stack dump data
		 * dynamic size
		 *   - the actual dumped size
		 */

		/* Static size. */
		perf_output_put(handle, dump_size);

		/* Data. */
		sp = perf_user_stack_pointer(regs);
		fs = force_uaccess_begin();
		rem = __output_copy_user(handle, (void *) sp, dump_size);
		force_uaccess_end(fs);
		dyn_size = dump_size - rem;

		perf_output_skip(handle, rem);

		/* Dynamic size. */
		perf_output_put(handle, dyn_size);
	}
}