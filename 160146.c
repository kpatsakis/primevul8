void LibRaw::apply_profile(const char *input, const char *output)
{
  char *prof;
  cmsHPROFILE hInProfile = 0, hOutProfile = 0;
  cmsHTRANSFORM hTransform;
  FILE *fp;
  unsigned size;

  if (strcmp(input, "embed"))
    hInProfile = cmsOpenProfileFromFile(input, "r");
  else if (profile_length)
  {
    hInProfile = cmsOpenProfileFromMem(imgdata.color.profile, profile_length);
  }
  else
  {
    imgdata.process_warnings |= LIBRAW_WARN_NO_EMBEDDED_PROFILE;
  }
  if (!hInProfile)
  {
    imgdata.process_warnings |= LIBRAW_WARN_NO_INPUT_PROFILE;
    return;
  }
  if (!output)
    hOutProfile = cmsCreate_sRGBProfile();
  else if ((fp = fopen(output, "rb")))
  {
    fread(&size, 4, 1, fp);
    fseek(fp, 0, SEEK_SET);
    oprof = (unsigned *)malloc(size = ntohl(size));
    merror(oprof, "apply_profile()");
    fread(oprof, 1, size, fp);
    fclose(fp);
    if (!(hOutProfile = cmsOpenProfileFromMem(oprof, size)))
    {
      free(oprof);
      oprof = 0;
    }
  }
  if (!hOutProfile)
  {
    imgdata.process_warnings |= LIBRAW_WARN_BAD_OUTPUT_PROFILE;
    goto quit;
  }
  RUN_CALLBACK(LIBRAW_PROGRESS_APPLY_PROFILE, 0, 2);
  hTransform = cmsCreateTransform(hInProfile, TYPE_RGBA_16, hOutProfile,
                                  TYPE_RGBA_16, INTENT_PERCEPTUAL, 0);
  cmsDoTransform(hTransform, image, image, width * height);
  raw_color = 1; /* Don't use rgb_cam with a profile */
  cmsDeleteTransform(hTransform);
  cmsCloseProfile(hOutProfile);
quit:
  cmsCloseProfile(hInProfile);
  RUN_CALLBACK(LIBRAW_PROGRESS_APPLY_PROFILE, 1, 2);
}