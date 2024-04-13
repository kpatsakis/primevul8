random_dump_stats()
{
    fprintf(stderr,
	    "random usage: poolsize=%d mixed=%lu polls=%lu/%lu added=%lu/%lu\n"
	    "              outmix=%lu getlvl1=%lu/%lu getlvl2=%lu/%lu\n",
	POOLSIZE, rndstats.mixrnd, rndstats.slowpolls, rndstats.fastpolls,
		  rndstats.naddbytes, rndstats.addbytes,
	rndstats.mixkey, rndstats.ngetbytes1, rndstats.getbytes1,
		    rndstats.ngetbytes2, rndstats.getbytes2 );
}