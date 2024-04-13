void bnxt_re_dealloc_ucontext(struct ib_ucontext *ib_uctx)
{
	struct bnxt_re_ucontext *uctx = container_of(ib_uctx,
						   struct bnxt_re_ucontext,
						   ib_uctx);

	struct bnxt_re_dev *rdev = uctx->rdev;

	if (uctx->shpg)
		free_page((unsigned long)uctx->shpg);

	if (uctx->dpi.dbr) {
		/* Free DPI only if this is the first PD allocated by the
		 * application and mark the context dpi as NULL
		 */
		bnxt_qplib_dealloc_dpi(&rdev->qplib_res,
				       &rdev->qplib_res.dpi_tbl, &uctx->dpi);
		uctx->dpi.dbr = NULL;
	}
}