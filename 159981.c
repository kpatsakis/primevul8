void LibRaw::setCanonBodyFeatures(unsigned long long id)
{

  ilm.CamID = id;
  if ((id == 0x80000001ULL) || // 1D
      (id == 0x80000174ULL) || // 1D2
      (id == 0x80000232ULL) || // 1D2N
      (id == 0x80000169ULL) || // 1D3
      (id == 0x80000281ULL)    // 1D4
  )
  {
    ilm.CameraFormat = LIBRAW_FORMAT_APSH;
    ilm.CameraMount = LIBRAW_MOUNT_Canon_EF;
  }
  else if ((id == 0x80000167ULL) || // 1Ds
           (id == 0x80000188ULL) || // 1Ds2
           (id == 0x80000215ULL) || // 1Ds3
           (id == 0x80000269ULL) || // 1DX
           (id == 0x80000328ULL) || // 1DX2
           (id == 0x80000324ULL) || // 1DC
           (id == 0x80000213ULL) || // 5D
           (id == 0x80000218ULL) || // 5D2
           (id == 0x80000285ULL) || // 5D3
           (id == 0x80000349ULL) || // 5D4
           (id == 0x80000382ULL) || // 5DS
           (id == 0x80000401ULL) || // 5DS R
           (id == 0x80000302ULL) || // 6D
           (id == 0x80000406ULL)    // 6D Mark II

  )
  {
    ilm.CameraFormat = LIBRAW_FORMAT_FF;
    ilm.CameraMount = LIBRAW_MOUNT_Canon_EF;
  }
  else if ((id == 0x80000331ULL) || // M
           (id == 0x80000355ULL) || // M2
           (id == 0x03740000ULL) || // M3
           (id == 0x03840000ULL) || // M10
           (id == 0x03940000ULL) || // M5
           (id == 0x00000412ULL) || // M50
           (id == 0x04070000ULL) || // M6
           (id == 0x03980000ULL) || // M100
           (id == CanonID_EOS_M6_Mark_II)
  )
  {
    ilm.CameraFormat = LIBRAW_FORMAT_APSC;
    ilm.CameraMount = LIBRAW_MOUNT_Canon_EF_M;
  }
  else if ((id == 0x80000424ULL) || // EOS R
           (id == 0x80000433ULL)    // EOS RP
  )
  {
    ilm.CameraFormat = LIBRAW_FORMAT_FF;
    ilm.CameraMount = LIBRAW_MOUNT_Canon_RF;
  }
  else if ((id == 0x01140000ULL) || // D30
           (id == 0x01668000ULL) || // D60
           (id > 0x80000000ULL))
  {
    ilm.CameraFormat = LIBRAW_FORMAT_APSC;
    ilm.CameraMount = LIBRAW_MOUNT_Canon_EF;
  }
}