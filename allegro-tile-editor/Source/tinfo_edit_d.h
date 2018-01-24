DIALOG TINFO_EDIT[] =
{
   /* (proc)  (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)             (dp2) (dp3) */
   { box, 0, 0, 320, 200, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL },
   { button, 130, 175, 80, 20, 0, 0, 'd', D_EXIT, 0, 0, "&Done", NULL, NULL },
   { listbox, 10, 25, 115, 145, 0, 0, 0, 0, 0, 0, dummy_getter, NULL, NULL },
   { label, 10, 10, 115, 10, 0, 0, 0, 0, 0, 0, "Select TINFO:", NULL, NULL },
   { button, 135, 120, 175, 20, 0, 0, 0, 0, 0, 0, "button", NULL, NULL },
   { button, 135, 145, 175, 20, 0, 0, 0, 0, 0, 0, "button", NULL, NULL },
   { label, 135, 25, 35, 30, 0, 0, 0, 0, 0, 0, "label", NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};