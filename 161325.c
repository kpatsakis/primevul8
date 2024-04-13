setup_xhost_auth (XHostAddress              *host_entries)
{
        host_entries[0].family    = FamilyServerInterpreted;
        host_entries[0].address   = "localuser\0root";
        host_entries[0].length    = sizeof ("localuser\0root");
        host_entries[1].family    = FamilyServerInterpreted;
        host_entries[1].address   = "localuser\0" GDM_USERNAME;
        host_entries[1].length    = sizeof ("localuser\0" GDM_USERNAME);
        host_entries[2].family    = FamilyServerInterpreted;
        host_entries[2].address   = "localuser\0gnome-initial-setup";
        host_entries[2].length    = sizeof ("localuser\0gnome-initial-setup");
}