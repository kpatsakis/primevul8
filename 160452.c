void print_timer(FILE* outfile, const starttime_t& started, int files)
{
	double elapsed = timer_elapsed(started);
	if (elapsed > 1.0)
		fprintf(outfile, "%d files processed in %5.3f sec, %5.3g files/sec\n", files, elapsed, files / elapsed);
	else if (elapsed > 0.001) // 1msec
	{
		double msec = elapsed * 1000.0;
		fprintf(outfile, "%d files processed in %5.3f msec, %5.3g files/sec\n", files, msec, files / elapsed);
	}
	else if (elapsed > 0.000001)
	{
		double usec = elapsed * 1000000.0;
		fprintf(outfile, "%d files processed in %5.3f usec, %5.3g files/sec\n", files, usec, files / elapsed);
	}
	else
		fprintf(outfile, "%d files processed, time too small to estimate\n", files);
}