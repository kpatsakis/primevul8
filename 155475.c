pdf_run_xobject(fz_context *ctx, pdf_run_processor *proc, pdf_xobject *xobj, pdf_obj *page_resources, const fz_matrix *transform, int is_smask)
{
	pdf_run_processor *pr = (pdf_run_processor *)proc;
	pdf_gstate *gstate = NULL;
	int oldtop = 0;
	int oldbot = -1;
	fz_matrix local_transform = *transform;
	softmask_save softmask = { NULL };
	int gparent_save;
	fz_matrix gparent_save_ctm;
	int cleanup_state = 0;
	char errmess[256] = "";
	pdf_obj *resources;
	fz_rect xobj_bbox;
	fz_matrix xobj_matrix;
	int transparency = 0;
	pdf_document *doc;
	fz_colorspace *cs = NULL;
	fz_default_colorspaces *saved_def_cs = NULL;

	/* Avoid infinite recursion */
	if (xobj == NULL || pdf_mark_obj(ctx, xobj->obj))
		return;

	fz_var(cleanup_state);
	fz_var(gstate);
	fz_var(oldtop);
	fz_var(oldbot);
	fz_var(cs);
	fz_var(saved_def_cs);

	gparent_save = pr->gparent;
	pr->gparent = pr->gtop;
	oldtop = pr->gtop;

	fz_try(ctx)
	{
		pdf_gsave(ctx, pr);

		gstate = pr->gstate + pr->gtop;

		pdf_xobject_bbox(ctx, xobj, &xobj_bbox);
		pdf_xobject_matrix(ctx, xobj, &xobj_matrix);
		transparency = pdf_xobject_transparency(ctx, xobj);

		/* apply xobject's transform matrix */
		fz_concat(&local_transform, &xobj_matrix, &local_transform);
		fz_concat(&gstate->ctm, &local_transform, &gstate->ctm);

		/* The gparent is updated with the modified ctm */
		gparent_save_ctm = pr->gstate[pr->gparent].ctm;
		pr->gstate[pr->gparent].ctm = gstate->ctm;

		/* apply soft mask, create transparency group and reset state */
		if (transparency)
		{
			fz_rect bbox;
			int isolated = pdf_xobject_isolated(ctx, xobj);

			bbox = xobj_bbox;
			fz_transform_rect(&bbox, &gstate->ctm);

			/* Remember that we tried to call begin_softmask. Even
			 * if it throws an error, we must call end_softmask. */
			cleanup_state = 1;
			gstate = begin_softmask(ctx, pr, &softmask);

			/* Remember that we tried to call fz_begin_group. Even
			 * if it throws an error, we must call fz_end_group. */
			cleanup_state = 2;
			if (isolated)
				cs = pdf_xobject_colorspace(ctx, xobj);
			fz_begin_group(ctx, pr->dev, &bbox,
					cs,
					(is_smask ? 1 : isolated),
					pdf_xobject_knockout(ctx, xobj),
					gstate->blendmode, gstate->fill.alpha);

			gstate->blendmode = 0;
			gstate->stroke.alpha = 1;
			gstate->fill.alpha = 1;
		}

		/* Remember that we tried to save for the clippath. Even if it
		 * throws an error, we must pop it. */
		cleanup_state = 3;
		pdf_gsave(ctx, pr); /* Save here so the clippath doesn't persist */

		/* clip to the bounds */
		fz_moveto(ctx, pr->path, xobj_bbox.x0, xobj_bbox.y0);
		fz_lineto(ctx, pr->path, xobj_bbox.x1, xobj_bbox.y0);
		fz_lineto(ctx, pr->path, xobj_bbox.x1, xobj_bbox.y1);
		fz_lineto(ctx, pr->path, xobj_bbox.x0, xobj_bbox.y1);
		fz_closepath(ctx, pr->path);
		pr->clip = 1;
		pdf_show_path(ctx, pr, 0, 0, 0, 0);

		/* run contents */

		resources = pdf_xobject_resources(ctx, xobj);
		if (!resources)
			resources = page_resources;

		saved_def_cs = pr->default_cs;
		pr->default_cs = NULL;
		pr->default_cs = pdf_update_default_colorspaces(ctx, saved_def_cs, resources);

		if (pr->default_cs != saved_def_cs)
			fz_set_default_colorspaces(ctx, pr->dev, pr->default_cs);

		doc = pdf_get_bound_document(ctx, xobj->obj);

		oldbot = pr->gbot;
		pr->gbot = pr->gtop;

		pdf_process_contents(ctx, (pdf_processor*)pr, doc, resources, xobj->obj, NULL);
	}
	fz_always(ctx)
	{
		fz_drop_colorspace(ctx, cs);

		if (saved_def_cs)
		{
			fz_drop_default_colorspaces(ctx, pr->default_cs);
			pr->default_cs = saved_def_cs;
			fz_try(ctx)
			{
				fz_set_default_colorspaces(ctx, pr->dev, pr->default_cs);
			}
			fz_catch(ctx)
			{
				/* Postpone the problem */
				strcpy(errmess, fz_caught_message(ctx));
			}
		}

		/* Undo any gstate mismatches due to the pdf_process_contents call */
		if (oldbot != -1)
		{
			while (pr->gtop > pr->gbot)
			{
				pdf_grestore(ctx, pr);
			}
			pr->gbot = oldbot;
		}

		if (cleanup_state >= 3)
			pdf_grestore(ctx, pr); /* Remove the state we pushed for the clippath */

		/* wrap up transparency stacks */
		if (transparency)
		{
			if (cleanup_state >= 2)
			{
				fz_try(ctx)
				{
					fz_end_group(ctx, pr->dev);
				}
				fz_catch(ctx)
				{
					/* Postpone the problem */
					if (errmess[0])
						fz_warn(ctx, "%s", errmess);
					strcpy(errmess, fz_caught_message(ctx));
				}
			}
			if (cleanup_state >= 1)
			{
				fz_try(ctx)
				{
					end_softmask(ctx, pr, &softmask);
				}
				fz_catch(ctx)
				{
					/* Postpone the problem */
					if (errmess[0])
						fz_warn(ctx, "%s", errmess);
					strcpy(errmess, fz_caught_message(ctx));
				}
			}
		}

		pr->gstate[pr->gparent].ctm = gparent_save_ctm;
		pr->gparent = gparent_save;

		while (oldtop < pr->gtop)
			pdf_grestore(ctx, pr);

		pdf_unmark_obj(ctx, xobj->obj);
	}
	fz_catch(ctx)
	{
		fz_rethrow(ctx);
	}

	/* Rethrow postponed errors */
	if (errmess[0])
		fz_throw(ctx, FZ_ERROR_GENERIC, "%s", errmess);
}