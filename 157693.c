int jbd2_journal_restart(handle_t *handle, int nblocks)
{
	return jbd2__journal_restart(handle, nblocks, 0, GFP_NOFS);
}