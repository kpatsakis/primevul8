static void request_done(int uptodate)
{
	struct request *req = current_req;
	int block;
	char msg[sizeof("request done ") + sizeof(int) * 3];

	probing = 0;
	snprintf(msg, sizeof(msg), "request done %d", uptodate);
	reschedule_timeout(MAXTIMEOUT, msg);

	if (!req) {
		pr_info("floppy.c: no request in request_done\n");
		return;
	}

	if (uptodate) {
		/* maintain values for invalidation on geometry
		 * change */
		block = current_count_sectors + blk_rq_pos(req);
		INFBOUND(DRS->maxblock, block);
		if (block > _floppy->sect)
			DRS->maxtrack = 1;

		floppy_end_request(req, 0);
	} else {
		if (rq_data_dir(req) == WRITE) {
			/* record write error information */
			DRWE->write_errors++;
			if (DRWE->write_errors == 1) {
				DRWE->first_error_sector = blk_rq_pos(req);
				DRWE->first_error_generation = DRS->generation;
			}
			DRWE->last_error_sector = blk_rq_pos(req);
			DRWE->last_error_generation = DRS->generation;
		}
		floppy_end_request(req, BLK_STS_IOERR);
	}
}