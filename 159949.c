void LibRaw::setPentaxBodyFeatures(unsigned long long id)
{

  ilm.CamID = id;

  switch (id)
  {
  case 0x12994:
  case 0x12aa2:
  case 0x12b1a:
  case 0x12b60:
  case 0x12b62:
  case 0x12b7e:
  case 0x12b80:
  case 0x12b9c:
  case 0x12b9d:
  case 0x12ba2:
  case 0x12c1e:
  case 0x12c20:
  case 0x12cd2:
  case 0x12cd4:
  case 0x12cfa:
  case 0x12d72:
  case 0x12d73:
  case 0x12db8:
  case 0x12dfe:
  case 0x12e6c:
  case 0x12e76:
  case 0x12ef8:
  case 0x12f52:
  case 0x12f70:
  case 0x12f71:
  case 0x12fb6:
  case 0x12fc0:
  case 0x12fca:
  case 0x1301a:
  case 0x13024:
  case 0x1309c:
  case 0x13222:
  case 0x1322c:
    ilm.CameraMount = LIBRAW_MOUNT_Pentax_K;
    ilm.CameraFormat = LIBRAW_FORMAT_APSC;
    break;
  case 0x13092: // K-1
  case 0x13240: // K-1 Mark II
    ilm.CameraMount = LIBRAW_MOUNT_Pentax_K;
    ilm.CameraFormat = LIBRAW_FORMAT_FF;
    break;
  case 0x12e08: // 645D
  case 0x13010: // 645Z
    ilm.CameraMount = LIBRAW_MOUNT_Pentax_645;
    ilm.CameraFormat = LIBRAW_FORMAT_CROP645;
    break;
  case 0x12ee4: // Q
  case 0x12f66: // Q10
    ilm.CameraMount = LIBRAW_MOUNT_Pentax_Q;
    ilm.CameraFormat = LIBRAW_FORMAT_1div2p3INCH;
    break;
  case 0x12f7a: // Q7
  case 0x1302e: // Q-S1
    ilm.CameraMount = LIBRAW_MOUNT_Pentax_Q;
    ilm.CameraFormat = LIBRAW_FORMAT_1div1p7INCH;
    break;
  case 0x12f84: // MX-1
    ilm.LensMount = LIBRAW_MOUNT_FixedLens;
    ilm.CameraMount = LIBRAW_MOUNT_FixedLens;
    ilm.CameraFormat = LIBRAW_FORMAT_1div1p7INCH;
    ilm.FocalType = LIBRAW_FT_ZOOM_LENS;
    break;
  case 0x1320e: // GR III
    ilm.CameraMount = LIBRAW_MOUNT_FixedLens;
    ilm.LensMount = LIBRAW_MOUNT_FixedLens;
    ilm.CameraFormat = LIBRAW_FORMAT_APSC;
    ilm.LensFormat = LIBRAW_FORMAT_APSC;
    ilm.FocalType = LIBRAW_FT_PRIME_LENS;
    break;
  default:
    ilm.LensMount = LIBRAW_MOUNT_FixedLens;
    ilm.CameraMount = LIBRAW_MOUNT_FixedLens;
  }
  return;
}