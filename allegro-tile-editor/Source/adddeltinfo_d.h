
DIALOG ADDDEL_TINFO[] =
{
   /* (proc)  (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                 (dp2) (dp3) */
   { box, 0, 0, 300, 180, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL },
   { button, 165, 150, 125, 20, 0, 0, 'd', D_EXIT, 0, 0, "&Done", NULL, NULL },
   { button, 10, 90, 125, 20, 0, 0, 'a', D_EXIT, 0, 0, "&Add", NULL, NULL },
   { button, 10, 120, 125, 20, 0, 0, 'r', D_EXIT, 0, 0, "&Remove", NULL, NULL },
   { button, 10, 150, 125, 20, 0, 0, 'n', D_EXIT, 0, 0, "Re&name", NULL, NULL },
   { listbox, 10, 45, 125, 35, 0, 0, 0, D_EXIT, 0, 0, tinfo_planes_getter, NULL, NULL },
   { listbox, 165, 45, 125, 95, 0, 0, 0, D_EXIT, 0, 0, tinfo_info_getter, NULL, NULL },
   { label, 150, 5, 5, 10, 0, 0, 0, 0, 1, 1, "Manage TILEINFO", NULL, NULL },
   { label, 10, 30, 125, 10, 0, 0, 0, 0, 0, 1, "Select Plane", NULL, NULL },
   { label, 165, 30, 125, 10, 0, 0, 0, 0, 0, 1, "Select INFO", NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};