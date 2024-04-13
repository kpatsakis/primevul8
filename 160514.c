void LibRaw::parseRicohMakernotes(int base, unsigned tag, unsigned type,
                                  unsigned len, unsigned dng_writer)
{
  char buffer[17];
  if (tag == 0x0005)
  {
    int c;
    int count = 0;
    fread(buffer, 16, 1, ifp);
    buffer[16] = 0;
    FORC(16)
    {
      if ((isspace(buffer[c])) || (buffer[c] == 0x2D) || (isalnum(buffer[c])))
        count++;
      else
        break;
    }
    if (count == 16)
    {
      if (strncmp(model, "GXR", 3))
      {
        sprintf(imgdata.shootinginfo.BodySerial, "%8s", buffer + 8);
      }
      buffer[8] = 0;
      sprintf(imgdata.shootinginfo.InternalBodySerial, "%8s", buffer);
    }
    else
    {
      sprintf(imgdata.shootinginfo.BodySerial, "%02x%02x%02x%02x", buffer[4],
              buffer[5], buffer[6], buffer[7]);
      sprintf(imgdata.shootinginfo.InternalBodySerial, "%02x%02x%02x%02x",
              buffer[8], buffer[9], buffer[10], buffer[11]);
    }
  }
  else if ((tag == 0x1001) && (type == 3))
  {
    ilm.CameraMount = LIBRAW_MOUNT_FixedLens;
    ilm.LensMount = LIBRAW_MOUNT_FixedLens;
    ilm.CameraFormat = LIBRAW_FORMAT_APSC;
    ilm.LensID = -1;
    ilm.FocalType = LIBRAW_FT_PRIME_LENS;
    imgdata.shootinginfo.ExposureProgram = get2();
  }
  else if (tag == 0x1002)
  {
    imgdata.shootinginfo.DriveMode = get2();
  }
  else if (tag == 0x1006)
  {
    imgdata.shootinginfo.FocusMode = get2();
  }
  else if ((tag == 0x100b) && (type == 10))
  {
    imgdata.makernotes.common.FlashEC = getreal(type);
  }
  else if ((tag == 0x1017) && (get2() == 2))
  {
    strcpy(ilm.Attachment, "Wide-Angle Adapter");
  }
  else if (tag == 0x1500)
  {
    ilm.CurFocal = getreal(type);
  }
  else if ((tag == 0x2001) && !strncmp(model, "GXR", 3))
  {
    short ntags, cur_tag;
    fseek(ifp, 20, SEEK_CUR);
    ntags = get2();
    cur_tag = get2();
    while (cur_tag != 0x002c)
    {
      fseek(ifp, 10, SEEK_CUR);
      cur_tag = get2();
    }
    fseek(ifp, 6, SEEK_CUR);
    fseek(ifp, get4(), SEEK_SET);
    for (int i=0; i<4; i++) {
      stread(buffer, 16, ifp);
      if ((buffer[0] == 'S') && (buffer[1] == 'I') && (buffer[2] == 'D'))
			  memcpy(imgdata.shootinginfo.BodySerial, buffer+4, 12);
      else if ((buffer[0] == 'R') && (buffer[1] == 'L'))
			  ilm.LensID = buffer[2] - '0';
      else if ((buffer[0] == 'L') && (buffer[1] == 'I') && (buffer[2] == 'D'))
			  memcpy(imgdata.lens.LensSerial, buffer+4, 12);
    }
  }
}