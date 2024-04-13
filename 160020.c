void LibRaw::parse_phase_one(int base)
{
  unsigned entries, tag, type, len, data, save, i, c;
  float romm_cam[3][3];
  char *cp;

  memset(&ph1, 0, sizeof ph1);
  fseek(ifp, base, SEEK_SET);
  order = get4() & 0xffff;
  if (get4() >> 8 != 0x526177)
    return; /* "Raw" */
  unsigned offset = get4();
  if (offset == 0xbad0bad)
    return;
  fseek(ifp, offset + base, SEEK_SET);
  entries = get4();
  if (entries > 8192)
    return; // too much??
  get4();
  while (entries--)
  {
    tag = get4();
    type = get4();
    len = get4();
    data = get4();
    save = ftell(ifp);
    fseek(ifp, base + data, SEEK_SET);
    switch (tag)
    {

    case 0x0102:
      stmread(imgdata.shootinginfo.BodySerial, len, ifp);
      if ((imgdata.shootinginfo.BodySerial[0] == 0x4c) &&
          (imgdata.shootinginfo.BodySerial[1] == 0x49))
      {
        unique_id = (((imgdata.shootinginfo.BodySerial[0] & 0x3f) << 5) |
                     (imgdata.shootinginfo.BodySerial[2] & 0x3f)) -
                    0x41;
      }
      else
      {
        unique_id = (((imgdata.shootinginfo.BodySerial[0] & 0x3f) << 5) |
                     (imgdata.shootinginfo.BodySerial[1] & 0x3f)) -
                    0x41;
      }
      setPhaseOneFeatures(unique_id);
      break;
    case 0x0203:
      stmread(imgdata.makernotes.phaseone.Software, len, ifp);
    case 0x0204:
      stmread(imgdata.makernotes.phaseone.SystemType, len, ifp);
    case 0x0211:
      imgdata.makernotes.common.SensorTemperature2 = int_to_float(data);
      break;
    case 0x0401:
      if (type == 4)
        ilm.CurAp = libraw_powf64l(2.0f, (int_to_float(data) / 2.0f));
      else
        ilm.CurAp = libraw_powf64l(2.0f, (getreal(type) / 2.0f));
      break;
    case 0x0403:
      if (type == 4)
        ilm.CurFocal = int_to_float(data);
      else
        ilm.CurFocal = getreal(type);
      break;
    case 0x0410:
      stmread(ilm.body, len, ifp);
      if (((unsigned char)ilm.body[0]) == 0xff)
        ilm.body[0] = 0;
      break;
    case 0x0412:
      stmread(ilm.Lens, len, ifp);
      if (((unsigned char)ilm.Lens[0]) == 0xff)
        ilm.Lens[0] = 0;
      break;
    case 0x0414:
      if (type == 4)
      {
        ilm.MaxAp4CurFocal = libraw_powf64l(2.0f, (int_to_float(data) / 2.0f));
      }
      else
      {
        ilm.MaxAp4CurFocal = libraw_powf64l(2.0f, (getreal(type) / 2.0f));
      }
      break;
    case 0x0415:
      if (type == 4)
      {
        ilm.MinAp4CurFocal = libraw_powf64l(2.0f, (int_to_float(data) / 2.0f));
      }
      else
      {
        ilm.MinAp4CurFocal = libraw_powf64l(2.0f, (getreal(type) / 2.0f));
      }
      break;
    case 0x0416:
      if (type == 4)
      {
        ilm.MinFocal = int_to_float(data);
      }
      else
      {
        ilm.MinFocal = getreal(type);
      }
      if (ilm.MinFocal > 1000.0f)
      {
        ilm.MinFocal = 0.0f;
      }
      break;
    case 0x0417:
      if (type == 4)
      {
        ilm.MaxFocal = int_to_float(data);
      }
      else
      {
        ilm.MaxFocal = getreal(type);
      }
      break;

    case 0x0100:
      flip = "0653"[data & 3] - '0';
      break;
    case 0x0106:
      for (i = 0; i < 9; i++)
        imgdata.color.P1_color[0].romm_cam[i] = ((float *)romm_cam)[i] =
            getreal(11);
      romm_coeff(romm_cam);
      break;
    case 0x0107:
      FORC3 cam_mul[c] = getreal(11);
      break;
    case 0x0108:
      raw_width = data;
      break;
    case 0x0109:
      raw_height = data;
      break;
    case 0x010a:
      left_margin = data;
      break;
    case 0x010b:
      top_margin = data;
      break;
    case 0x010c:
      width = data;
      break;
    case 0x010d:
      height = data;
      break;
    case 0x010e:
      ph1.format = data;
      break;
    case 0x010f:
      data_offset = data + base;
      break;
    case 0x0110:
      meta_offset = data + base;
      meta_length = len;
      break;
    case 0x0112:
      ph1.key_off = save - 4;
      break;
    case 0x0210:
      ph1.tag_210 = int_to_float(data);
      imgdata.makernotes.common.SensorTemperature = ph1.tag_210;
      break;
    case 0x021a:
      ph1.tag_21a = data;
      break;
    case 0x021c:
      strip_offset = data + base;
      break;
    case 0x021d:
      ph1.t_black = data;
      break;
    case 0x0222:
      ph1.split_col = data;
      break;
    case 0x0223:
      ph1.black_col = data + base;
      break;
    case 0x0224:
      ph1.split_row = data;
      break;
    case 0x0225:
      ph1.black_row = data + base;
      break;
    case 0x0226:
      for (i = 0; i < 9; i++)
        imgdata.color.P1_color[1].romm_cam[i] = getreal(11);
      break;
    case 0x0301:
      model[63] = 0;
      imgdata.makernotes.phaseone.FirmwareString[255] = 0;
      fread(imgdata.makernotes.phaseone.FirmwareString, 1, 255, ifp);
      memcpy(model, imgdata.makernotes.phaseone.FirmwareString, 63);
      if ((cp = strstr(model, " camera")))
        *cp = 0;
      else if ((cp = strchr(model, ',')))
        *cp = 0;
      /* minus and the letter after it are not always present
        if present, last letter means:
          C : Contax 645AF
          H : Hasselblad H1 / H2
          M : Mamiya
          V : Hasselblad 555ELD / 553ELX / 503CW / 501CM; not included below
        because of adapter conflicts (Mamiya RZ body) if not present, Phase One
        645 AF, Mamiya 645AFD Series, or anything
       */
      strcpy(imgdata.makernotes.phaseone.SystemModel, model);
      if ((cp = strchr(model, '-')))
      {
        if (cp[1] == 'C')
        {
          strcpy(ilm.body, "Contax 645AF");
          ilm.CameraMount = LIBRAW_MOUNT_Contax645;
          ilm.CameraFormat = LIBRAW_FORMAT_645;
        }
        else if (cp[1] == 'M')
        {
          strcpy(ilm.body, "Mamiya 645");
          ilm.CameraMount = LIBRAW_MOUNT_Mamiya645;
          ilm.CameraFormat = LIBRAW_FORMAT_645;
        }
        else if (cp[1] == 'H')
        {
          strcpy(ilm.body, "Hasselblad H1/H2");
          ilm.CameraMount = LIBRAW_MOUNT_Hasselblad_H;
          ilm.CameraFormat = LIBRAW_FORMAT_645;
        }
        *cp = 0;
      }
    }
    fseek(ifp, save, SEEK_SET);
  }

  if (!ilm.body[0] && !imgdata.shootinginfo.BodySerial[0])
  {
    fseek(ifp, meta_offset, SEEK_SET);
    order = get2();
    fseek(ifp, 6, SEEK_CUR);
    fseek(ifp, meta_offset + get4(), SEEK_SET);
    entries = get4();
    get4();
    while (entries--)
    {
      tag = get4();
      len = get4();
      data = get4();
      save = ftell(ifp);
      fseek(ifp, meta_offset + data, SEEK_SET);
      if (tag == 0x0407)
      {
        stmread(imgdata.shootinginfo.BodySerial, len, ifp);
        if ((imgdata.shootinginfo.BodySerial[0] == 0x4c) &&
            (imgdata.shootinginfo.BodySerial[1] == 0x49))
        {
          unique_id = (((imgdata.shootinginfo.BodySerial[0] & 0x3f) << 5) |
                       (imgdata.shootinginfo.BodySerial[2] & 0x3f)) -
                      0x41;
        }
        else
        {
          unique_id = (((imgdata.shootinginfo.BodySerial[0] & 0x3f) << 5) |
                       (imgdata.shootinginfo.BodySerial[1] & 0x3f)) -
                      0x41;
        }
        setPhaseOneFeatures(unique_id);
      }
      fseek(ifp, save, SEEK_SET);
    }
  }
  load_raw = ph1.format < 3 ? &LibRaw::phase_one_load_raw
                            : &LibRaw::phase_one_load_raw_c;
  maximum = 0xffff;
  strcpy(make, "Phase One");
  if (model[0])
    return;
  switch (raw_height)
  {
  case 2060:
    strcpy(model, "LightPhase");
    break;
  case 2682:
    strcpy(model, "H 10");
    break;
  case 4128:
    strcpy(model, "H 20");
    break;
  case 5488:
    strcpy(model, "H 25");
    break;
  }
}