void usage(char *progname)
{
	fprintf(stderr,
		_("Usage: %s [ options ] [ <interval> [ <count> ] ] [ <datafile> | -[0-9]+ ]\n"),
		progname);

	fprintf(stderr, _("Options are:\n"
			  "[ -C ] [ -c | -d | -g | -j | -l | -p | -r | -x ] [ -H ] [ -h ] [ -T | -t | -U ] [ -V ]\n"
			  "[ -O <opts> [,...] ] [ -P { <cpu> [,...] | ALL } ]\n"
			  "[ --dev=<dev_list> ] [ --fs=<fs_list> ] [ --iface=<iface_list> ]\n"
			  "[ -s [ <hh:mm[:ss]> ] ] [ -e [ <hh:mm[:ss]> ] ]\n"
			  "[ -- <sar_options> ]\n"));
	exit(1);
}