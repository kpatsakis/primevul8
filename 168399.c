fz_new_icc_colorspace(fz_context *ctx, const char *name, int num, fz_buffer *buf)
{
#ifdef NO_ICC
	fz_throw(ctx, FZ_ERROR_GENERIC, "ICC Profiles not supported in NO_ICC build");
#else
	fz_colorspace *cs = NULL;
	fz_iccprofile *profile;
	int is_lab = 0;
	enum fz_colorspace_type type = FZ_COLORSPACE_NONE;
	int flags = FZ_COLORSPACE_IS_ICC;

	profile = fz_malloc_struct(ctx, fz_iccprofile);
	fz_try(ctx)
	{
		if (buf == NULL)
		{
			size_t size;
			const unsigned char *data;
			data = fz_lookup_icc(ctx, name, &size);
			profile->buffer = fz_new_buffer_from_shared_data(ctx, data, size);
			is_lab = (strcmp(name, FZ_ICC_PROFILE_LAB) == 0);
			profile->bgr = (strcmp(name, FZ_ICC_PROFILE_BGR) == 0);
			flags |= FZ_COLORSPACE_IS_DEVICE;
		}
		else
		{
			profile->buffer = fz_keep_buffer(ctx, buf);
		}

		fz_cmm_init_profile(ctx, profile);

		/* Check if correct type, if a particular type was expected */
		if (num != 0 && num != profile->num_devcomp)
		{
			fz_drop_buffer(ctx, profile->buffer);
			fz_cmm_fin_profile(ctx, profile);
			fz_free(ctx, profile);
			break;
		}

		fz_md5_icc(ctx, profile);

		switch (profile->num_devcomp)
		{
		default:
			type = FZ_COLORSPACE_SEPARATION;
			break;
		case 1:
			type = FZ_COLORSPACE_GRAY;
			break;
		case 3:
			if (is_lab)
				type = FZ_COLORSPACE_LAB;
			else if (profile->bgr)
				type = FZ_COLORSPACE_BGR;
			else
				type = FZ_COLORSPACE_RGB;
			break;
		case 4:
			type = FZ_COLORSPACE_CMYK;
			break;
		}

		cs = fz_new_colorspace(ctx, name, type, flags, profile->num_devcomp, NULL, NULL, NULL, is_lab ? clamp_lab_icc : clamp_default_icc, free_icc, profile, sizeof(profile));

		switch (profile->num_devcomp)
		{
		case 1:
			fz_colorspace_name_colorant(ctx, cs, 0, "Gray");
			break;
		case 3:
			if (is_lab)
			{
				fz_colorspace_name_colorant(ctx, cs, 0, "L*");
				fz_colorspace_name_colorant(ctx, cs, 1, "a*");
				fz_colorspace_name_colorant(ctx, cs, 2, "b*");
			} else {
				fz_colorspace_name_colorant(ctx, cs, profile->bgr ? 2 : 0, "Red");
				fz_colorspace_name_colorant(ctx, cs, 1, "Green");
				fz_colorspace_name_colorant(ctx, cs, profile->bgr ? 0 : 2, "Blue");
			}
			break;
		case 4:
			fz_colorspace_name_colorant(ctx, cs, 0, "Cyan");
			fz_colorspace_name_colorant(ctx, cs, 1, "Magenta");
			fz_colorspace_name_colorant(ctx, cs, 2, "Yellow");
			fz_colorspace_name_colorant(ctx, cs, 3, "Black");
			break;
		}
	}
	fz_catch(ctx)
	{
		fz_drop_buffer(ctx, profile->buffer);
		fz_cmm_fin_profile(ctx, profile);
		fz_free(ctx, profile);
	}
	return cs;
#endif
}