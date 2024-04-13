query (void)
{
  static const GimpParamDef load_args[] =
  {
    { GIMP_PDB_INT32,  "run-mode",     "The run mode { RUN-INTERACTIVE (0), RUN-NONINTERACTIVE (1) }" },
    { GIMP_PDB_STRING, "filename",     "The name of the file to load" },
    { GIMP_PDB_STRING, "raw-filename", "The name entered"             }
  };
  static const GimpParamDef load_return_vals[] =
  {
    { GIMP_PDB_IMAGE, "image", "Output image" }
  };

  static const GimpParamDef save_args[] =
  {
    { GIMP_PDB_INT32,    "run-mode",     "The run mode { RUN-INTERACTIVE (0), RUN-NONINTERACTIVE (1) }" },
    { GIMP_PDB_IMAGE,    "image",        "Input image"                  },
    { GIMP_PDB_DRAWABLE, "drawable",     "Drawable to save"             },
    { GIMP_PDB_STRING,   "filename",     "The name of the file to save the image in" },
    { GIMP_PDB_STRING,   "raw-filename", "The name entered"             }
  };

  gimp_install_procedure (LOAD_PROC,
                          "Loads files in Zsoft PCX file format",
                          "FIXME: write help for pcx_load",
                          "Francisco Bustamante & Nick Lamb",
                          "Nick Lamb <njl195@zepler.org.uk>",
                          "January 1997",
                          N_("ZSoft PCX image"),
                          NULL,
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (load_args),
                          G_N_ELEMENTS (load_return_vals),
                          load_args, load_return_vals);

  gimp_register_file_handler_mime (LOAD_PROC, "image/x-pcx");
  gimp_register_magic_load_handler (LOAD_PROC,
                                    "pcx,pcc",
                                    "",
                                    "0&,byte,10,2&,byte,1,3&,byte,>0,3,byte,<9");

  gimp_install_procedure (SAVE_PROC,
                          "Saves files in ZSoft PCX file format",
                          "FIXME: write help for pcx_save",
                          "Francisco Bustamante & Nick Lamb",
                          "Nick Lamb <njl195@zepler.org.uk>",
                          "January 1997",
                          N_("ZSoft PCX image"),
                          "INDEXED, RGB, GRAY",
                          GIMP_PLUGIN,
                          G_N_ELEMENTS (save_args), 0,
                          save_args, NULL);

  gimp_register_file_handler_mime (SAVE_PROC, "image/x-pcx");
  gimp_register_save_handler (SAVE_PROC, "pcx,pcc", "");
}