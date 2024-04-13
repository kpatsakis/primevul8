void LibRaw::process_Sony_0x9400(uchar *buf, ushort len, unsigned long long id)
{

  uchar s[4];
  int c;
  uchar bufx = buf[0];

  if (((bufx == 0x23) || (bufx == 0x24) || (bufx == 0x26) || (bufx == 0x28)) &&
      (len >= 0x1f))
  { // 0x9400 'c' version

    if ((id == 0x166ULL) || // ILCE-9
        (id == 0x16aULL) || // ILCE-7RM3
        (id == 0x16bULL) || // ILCE-7M3
        (id == 0x16dULL) || // DSC-RX10M4
        (id == 0x16eULL) || // DSC-RX100M6
        (id == 0x171ULL) || // DSC-RX100M5A
        (id == 0x173ULL) || // ILCE-6400
        (id == 0x174ULL) || // DSC-RX0M2
        (id == 0x176ULL) || // DSC-RX100M7
        (id == 0x177ULL) || // ILCE-7RM4
        (id == 0x17aULL) || // ILCE-6600
        (id == 0x17bULL)    // ILCE-6100
    )
    {
      imSony.ShotNumberSincePowerUp = SonySubstitution[buf[0x0a]];
    }
    else
    {
      FORC4 s[c] = SonySubstitution[buf[0x0a + c]];
      imSony.ShotNumberSincePowerUp = sget4(s);
    }

    imSony.Sony0x9400_version = 0xc;

    imSony.Sony0x9400_ReleaseMode2 = SonySubstitution[buf[0x09]];

    FORC4 s[c] = SonySubstitution[buf[0x12 + c]];
    imSony.Sony0x9400_SequenceImageNumber = sget4(s);

    imSony.Sony0x9400_SequenceLength1 = SonySubstitution[buf[0x16]]; // shots

    FORC4 s[c] = SonySubstitution[buf[0x1a + c]];
    imSony.Sony0x9400_SequenceFileNumber = sget4(s);

    imSony.Sony0x9400_SequenceLength2 = SonySubstitution[buf[0x1e]]; // files
  }

  else if ((bufx == 0x0c) && (len >= 0x1f))
  { // 0x9400 'b' version
    imSony.Sony0x9400_version = 0xb;

    FORC4 s[c] = SonySubstitution[buf[0x08 + c]];
    imSony.Sony0x9400_SequenceImageNumber = sget4(s);

    FORC4 s[c] = SonySubstitution[buf[0x0c + c]];
    imSony.Sony0x9400_SequenceFileNumber = sget4(s);

    imSony.Sony0x9400_ReleaseMode2 = SonySubstitution[buf[0x10]];

    imSony.Sony0x9400_SequenceLength1 = SonySubstitution[buf[0x1e]];
  }

  else if ((bufx == 0x0a) && (len >= 0x23))
  { // 0x9400 'a' version
    imSony.Sony0x9400_version = 0xa;

    FORC4 s[c] = SonySubstitution[buf[0x08 + c]];
    imSony.Sony0x9400_SequenceImageNumber = sget4(s);

    FORC4 s[c] = SonySubstitution[buf[0x0c + c]];
    imSony.Sony0x9400_SequenceFileNumber = sget4(s);

    imSony.Sony0x9400_ReleaseMode2 = SonySubstitution[buf[0x10]];

    imSony.Sony0x9400_SequenceLength1 = SonySubstitution[buf[0x22]];
  }

  else
    return;
}