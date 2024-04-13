  int raw_was_read()
  {
    return imgdata.rawdata.raw_image || imgdata.rawdata.color4_image ||
           imgdata.rawdata.color3_image || imgdata.rawdata.float_image ||
           imgdata.rawdata.float3_image || imgdata.rawdata.float4_image;
  }