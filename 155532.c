pdf_new_run_processor(fz_context *ctx, fz_device *dev, const fz_matrix *ctm, const char *usage, pdf_gstate *gstate, int nested, fz_default_colorspaces *default_cs)
{
	pdf_run_processor *proc = pdf_new_processor(ctx, sizeof *proc);
	{
		proc->super.usage = usage;

		proc->super.drop_processor = pdf_drop_run_processor;

		/* general graphics state */
		proc->super.op_w = pdf_run_w;
		proc->super.op_j = pdf_run_j;
		proc->super.op_J = pdf_run_J;
		proc->super.op_M = pdf_run_M;
		proc->super.op_d = pdf_run_d;
		proc->super.op_ri = pdf_run_ri;
		proc->super.op_i = pdf_run_i;
		proc->super.op_gs_begin = pdf_run_gs_begin;
		proc->super.op_gs_end = pdf_run_gs_end;

		/* transparency graphics state */
		proc->super.op_gs_BM = pdf_run_gs_BM;
		proc->super.op_gs_CA = pdf_run_gs_CA;
		proc->super.op_gs_ca = pdf_run_gs_ca;
		proc->super.op_gs_SMask = pdf_run_gs_SMask;

		/* special graphics state */
		proc->super.op_q = pdf_run_q;
		proc->super.op_Q = pdf_run_Q;
		proc->super.op_cm = pdf_run_cm;

		/* path construction */
		proc->super.op_m = pdf_run_m;
		proc->super.op_l = pdf_run_l;
		proc->super.op_c = pdf_run_c;
		proc->super.op_v = pdf_run_v;
		proc->super.op_y = pdf_run_y;
		proc->super.op_h = pdf_run_h;
		proc->super.op_re = pdf_run_re;

		/* path painting */
		proc->super.op_S = pdf_run_S;
		proc->super.op_s = pdf_run_s;
		proc->super.op_F = pdf_run_F;
		proc->super.op_f = pdf_run_f;
		proc->super.op_fstar = pdf_run_fstar;
		proc->super.op_B = pdf_run_B;
		proc->super.op_Bstar = pdf_run_Bstar;
		proc->super.op_b = pdf_run_b;
		proc->super.op_bstar = pdf_run_bstar;
		proc->super.op_n = pdf_run_n;

		/* clipping paths */
		proc->super.op_W = pdf_run_W;
		proc->super.op_Wstar = pdf_run_Wstar;

		/* text objects */
		proc->super.op_BT = pdf_run_BT;
		proc->super.op_ET = pdf_run_ET;

		/* text state */
		proc->super.op_Tc = pdf_run_Tc;
		proc->super.op_Tw = pdf_run_Tw;
		proc->super.op_Tz = pdf_run_Tz;
		proc->super.op_TL = pdf_run_TL;
		proc->super.op_Tf = pdf_run_Tf;
		proc->super.op_Tr = pdf_run_Tr;
		proc->super.op_Ts = pdf_run_Ts;

		/* text positioning */
		proc->super.op_Td = pdf_run_Td;
		proc->super.op_TD = pdf_run_TD;
		proc->super.op_Tm = pdf_run_Tm;
		proc->super.op_Tstar = pdf_run_Tstar;

		/* text showing */
		proc->super.op_TJ = pdf_run_TJ;
		proc->super.op_Tj = pdf_run_Tj;
		proc->super.op_squote = pdf_run_squote;
		proc->super.op_dquote = pdf_run_dquote;

		/* type 3 fonts */
		proc->super.op_d0 = pdf_run_d0;
		proc->super.op_d1 = pdf_run_d1;

		/* color */
		proc->super.op_CS = pdf_run_CS;
		proc->super.op_cs = pdf_run_cs;
		proc->super.op_SC_color = pdf_run_SC_color;
		proc->super.op_sc_color = pdf_run_sc_color;
		proc->super.op_SC_pattern = pdf_run_SC_pattern;
		proc->super.op_sc_pattern = pdf_run_sc_pattern;
		proc->super.op_SC_shade = pdf_run_SC_shade;
		proc->super.op_sc_shade = pdf_run_sc_shade;

		proc->super.op_G = pdf_run_G;
		proc->super.op_g = pdf_run_g;
		proc->super.op_RG = pdf_run_RG;
		proc->super.op_rg = pdf_run_rg;
		proc->super.op_K = pdf_run_K;
		proc->super.op_k = pdf_run_k;

		/* shadings, images, xobjects */
		proc->super.op_sh = pdf_run_sh;
		if (dev->fill_image || dev->fill_image_mask || dev->clip_image_mask)
		{
			proc->super.op_BI = pdf_run_BI;
			proc->super.op_Do_image = pdf_run_Do_image;
		}
		proc->super.op_Do_form = pdf_run_Do_form;

		/* marked content */
		proc->super.op_MP = pdf_run_MP;
		proc->super.op_DP = pdf_run_DP;
		proc->super.op_BMC = pdf_run_BMC;
		proc->super.op_BDC = pdf_run_BDC;
		proc->super.op_EMC = pdf_run_EMC;

		/* compatibility */
		proc->super.op_BX = pdf_run_BX;
		proc->super.op_EX = pdf_run_EX;

		/* extgstate */
		proc->super.op_gs_OP = pdf_run_gs_OP;
		proc->super.op_gs_op = pdf_run_gs_op;
		proc->super.op_gs_OPM = pdf_run_gs_OPM;
		proc->super.op_gs_UseBlackPtComp = pdf_run_gs_UseBlackPtComp;

		proc->super.op_END = pdf_run_END;
	}

	proc->dev = dev;

	proc->default_cs = fz_keep_default_colorspaces(ctx, default_cs);

	proc->nested_depth = nested;

	proc->path = NULL;
	proc->clip = 0;
	proc->clip_even_odd = 0;

	proc->tos.text = NULL;
	proc->tos.tlm = fz_identity;
	proc->tos.tm = fz_identity;
	proc->tos.text_mode = 0;

	fz_try(ctx)
	{
		proc->path = fz_new_path(ctx);

		proc->gcap = 64;
		proc->gstate = fz_malloc_array(ctx, proc->gcap, sizeof(pdf_gstate));

		pdf_init_gstate(ctx, &proc->gstate[0], ctm);
		if (gstate)
		{
			pdf_copy_gstate(ctx, &proc->gstate[0], gstate);
			proc->gstate[0].clip_depth = 0;
			proc->gstate[0].ctm = *ctm;
		}
		proc->gtop = 0;
		proc->gbot = 0;
		proc->gparent = 0;
	}
	fz_catch(ctx)
	{
		fz_drop_path(ctx, proc->path);
		fz_free(ctx, proc);
		fz_rethrow(ctx);
	}

	/* We need to save an extra level to allow for level 0 to be the parent gstate level. */
	pdf_gsave(ctx, proc);

	return (pdf_processor*)proc;
}