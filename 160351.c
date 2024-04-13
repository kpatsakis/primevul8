void LibRaw::parse_mos(int offset)
{
  char data[40];
  int skip, from, i, c, neut[4], planes = 0, frot = 0;
  static const char *mod[] = {
      /* DM22, DM28, DM40, DM56 are somewhere here too */
      "",             //  0
      "DCB2",         //  1
      "Volare",       //  2
      "Cantare",      //  3
      "CMost",        //  4
      "Valeo 6",      //  5
      "Valeo 11",     //  6
      "Valeo 22",     //  7
      "Valeo 11p",    //  8
      "Valeo 17",     //  9
      "",             // 10
      "Aptus 17",     // 11
      "Aptus 22",     // 12
      "Aptus 75",     // 13
      "Aptus 65",     // 14
      "Aptus 54S",    // 15
      "Aptus 65S",    // 16
      "Aptus 75S",    // 17
      "AFi 5",        // 18
      "AFi 6",        // 19
      "AFi 7",        // 20
      "AFi-II 7",     // 21
      "Aptus-II 7",   // 22 (same CMs as Mamiya DM33)
      "",             // 23
      "Aptus-II 6",   // 24 (same CMs as Mamiya DM28)
      "AFi-II 10",    // 25
      "",             // 26
      "Aptus-II 10",  // 27 (same CMs as Mamiya DM56)
      "Aptus-II 5",   // 28 (same CMs as Mamiya DM22)
      "",             // 29
      "DM33",         // 30, make is Mamiya
      "",             // 31
      "",             // 32
      "Aptus-II 10R", // 33
      "Aptus-II 8",   // 34 (same CMs as Mamiya DM40)
      "",             // 35
      "Aptus-II 12",  // 36
      "",             // 37
      "AFi-II 12"     // 38
  };
  float romm_cam[3][3];

  fseek(ifp, offset, SEEK_SET);
  while (1)
  {
    if (get4() != 0x504b5453)
      break;
    get4();
    fread(data, 1, 40, ifp);
    skip = get4();
    from = ftell(ifp);

    if (!strcmp(data, "CameraObj_camera_type"))
    {
      stmread(ilm.body, skip, ifp);
      if (ilm.body[0])
      {
        if (!strncmp(ilm.body, "Mamiya R", 8))
        {
          ilm.CameraMount = LIBRAW_MOUNT_Mamiya67;
          ilm.CameraFormat = LIBRAW_FORMAT_67;
        }
        else if (!strncmp(ilm.body, "Hasselblad 5", 12))
        {
          ilm.CameraFormat = LIBRAW_FORMAT_66;
          ilm.CameraMount = LIBRAW_MOUNT_Hasselblad_V;
        }
        else if (!strncmp(ilm.body, "Hasselblad H", 12))
        {
          ilm.CameraMount = LIBRAW_MOUNT_Hasselblad_H;
          ilm.CameraFormat = LIBRAW_FORMAT_645;
        }
        else if (!strncmp(ilm.body, "Mamiya 6", 8) ||
                 !strncmp(ilm.body, "Phase One 6", 11))
        {
          ilm.CameraMount = LIBRAW_MOUNT_Mamiya645;
          ilm.CameraFormat = LIBRAW_FORMAT_645;
        }
        else if (!strncmp(ilm.body, "Large F", 7))
        {
          ilm.CameraMount = LIBRAW_MOUNT_LF;
          ilm.CameraFormat = LIBRAW_FORMAT_LF;
        }
        else if (!strncmp(model, "Leaf AFi", 8))
        {
          ilm.CameraMount = LIBRAW_MOUNT_Rollei_bayonet;
          ilm.CameraFormat = LIBRAW_FORMAT_66;
        }
      }
    }
    if (!strcmp(data, "back_serial_number"))
    {
      char buffer[sizeof(imgdata.shootinginfo.BodySerial)];
      char *words[4];
      int nwords;
      stmread(buffer, skip, ifp);
      nwords =
          getwords(buffer, words, 4, sizeof(imgdata.shootinginfo.BodySerial));
      strcpy(imgdata.shootinginfo.BodySerial, words[0]);
    }
    if (!strcmp(data, "CaptProf_serial_number"))
    {
      char buffer[sizeof(imgdata.shootinginfo.InternalBodySerial)];
      char *words[4];
      int nwords;
      stmread(buffer, skip, ifp);
      nwords = getwords(buffer, words, 4,
                        sizeof(imgdata.shootinginfo.InternalBodySerial));
      strcpy(imgdata.shootinginfo.InternalBodySerial, words[0]);
    }

    if (!strcmp(data, "JPEG_preview_data"))
    {
      thumb_offset = from;
      thumb_length = skip;
    }
    if (!strcmp(data, "icc_camera_profile"))
    {
      profile_offset = from;
      profile_length = skip;
    }
    if (!strcmp(data, "ShootObj_back_type"))
    {
      fscanf(ifp, "%d", &i);
      if ((unsigned)i < sizeof mod / sizeof(*mod))
      {
        strcpy(model, mod[i]);
        if (!strncmp(model, "AFi", 3))
        {
          ilm.CameraMount = LIBRAW_MOUNT_Rollei_bayonet;
          ilm.CameraFormat = LIBRAW_FORMAT_66;
        }
        ilm.CamID = i;
      }
    }
    if (!strcmp(data, "icc_camera_to_tone_matrix"))
    {
      for (i = 0; i < 9; i++)
        ((float *)romm_cam)[i] = int_to_float(get4());
      romm_coeff(romm_cam);
    }
    if (!strcmp(data, "CaptProf_color_matrix"))
    {
      for (i = 0; i < 9; i++)
        fscanf(ifp, "%f", (float *)romm_cam + i);
      romm_coeff(romm_cam);
    }
    if (!strcmp(data, "CaptProf_number_of_planes"))
      fscanf(ifp, "%d", &planes);
    if (!strcmp(data, "CaptProf_raw_data_rotation"))
      fscanf(ifp, "%d", &flip);
    if (!strcmp(data, "CaptProf_mosaic_pattern"))
      FORC4
      {
        fscanf(ifp, "%d", &i);
        if (i == 1)
          frot = c ^ (c >> 1);
      }
    if (!strcmp(data, "ImgProf_rotation_angle"))
    {
      fscanf(ifp, "%d", &i);
      flip = i - flip;
    }
    if (!strcmp(data, "NeutObj_neutrals") && !cam_mul[0])
    {
      FORC4 fscanf(ifp, "%d", neut + c);
      FORC3
      if (neut[c + 1])
        cam_mul[c] = (float)neut[0] / neut[c + 1];
    }
    if (!strcmp(data, "Rows_data"))
      load_flags = get4();
    parse_mos(from);
    fseek(ifp, skip + from, SEEK_SET);
  }
  if (planes)
    filters = (planes == 1) * 0x01010101U *
              (uchar) "\x94\x61\x16\x49"[(flip / 90 + frot) & 3];
}