void CLASS setSonyBodyFeatures (unsigned id) {

  imgdata.lens.makernotes.CamID = id;
  if (	// FF cameras
      (id == 257) ||		// a900
      (id == 269) ||		// a850
      (id == 340) ||		// ILCE-7M2
      (id == 318) ||		// ILCE-7S
      (id == 311) ||		// ILCE-7R
      (id == 347) ||		// ILCE-7RM2
      (id == 306) ||		// ILCE-7
      (id == 298) ||		// DSC-RX1
      (id == 299) ||		// NEX-VG900
      (id == 310) ||		// DSC-RX1R
      (id == 294)			// SLT-99, Hasselblad HV
      )
    {
      imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_FF;
    }
  else
    {
      if ((id != 002) &&  // DSC-R1
          (id != 297) &&  // DSC-RX100
          (id != 308) &&  // DSC-RX100M2
          (id != 309) &&  // DSC-RX10
          (id != 317) &&  // DSC-RX100M3
          (id != 341) &&  // DSC-RX100M4
          (id != 342)     // DSC-RX10M2
         )
      imgdata.lens.makernotes.CameraFormat = LIBRAW_FORMAT_APSC;
    }

  if (	// E-mount cameras
      // ILCE:
      (id == 302) ||
      (id == 306) ||
      (id == 311) ||
      (id == 312) ||
      (id == 313) ||
      (id == 318) ||
      (id == 339) ||
      (id == 340) ||
      (id == 346) ||
      (id == 347) ||
      // NEX:
      (id == 278) ||
      (id == 279) ||
      (id == 284) ||
      (id == 288) ||
      (id == 289) ||
      (id == 290) ||
      (id == 293) ||
      (id == 295) ||
      (id == 296) ||
      (id == 299) ||
      (id == 300) ||
      (id == 305) ||
      (id == 307)
      )
    {
      imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Sony_E;
    }

  else if (	// A-mount cameras
           // DSLR:
           (id == 256) ||
           (id == 257) ||
           (id == 258) ||
           (id == 259) ||
           (id == 260) ||
           (id == 261) ||
           (id == 262) ||
           (id == 263) ||
           (id == 264) ||
           (id == 265) ||
           (id == 266) ||
           (id == 269) ||
           (id == 270) ||
           (id == 273) ||
           (id == 274) ||
           (id == 275) ||
           (id == 282) ||
           (id == 283) ||
           // SLT:
           (id == 280) ||
           (id == 281) ||
           (id == 285) ||
           (id == 286) ||
           (id == 287) ||
           (id == 291) ||
           (id == 292) ||
           (id == 294) ||
           (id == 303) ||
           // ILCA:
           (id == 319)
           )
    {
      imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_Minolta_A;
    }

  else if (	// DSC
           (id == 002) ||  // DSC-R1
           (id == 297) ||  // DSC-RX100
           (id == 298) ||  // DSC-RX1
           (id == 308) ||  // DSC-RX100M2
           (id == 309) ||  // DSC-RX10
           (id == 310) ||  // DSC-RX1R
           (id == 317) ||  // DSC-RX100M3
           (id == 341) ||  // DSC-RX100M4
           (id == 342)     // DSC-RX10M2
           )
    {
      imgdata.lens.makernotes.CameraMount = LIBRAW_MOUNT_FixedLens;
      imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_FixedLens;
    }
  return;
}