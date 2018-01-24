DIALOG MAP_STATS[] =
{
   /* (proc) (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                 (dp2) (dp3) */
   { box, 0, 0, 300, 180, 1, 0, 0, 0, 0, 0, NULL, NULL, NULL },
   { button, 105, 155, 80, 20, 0, 0, 'd', D_EXIT, 0, 0, "&Done", NULL, NULL },
   { label, 150, 5, 0, 0, 0, 0, 0, 0, 1, 1, "Map Statistics", NULL, NULL },
   { label, 15, 25, 130, 10, 0, 0, 0, 0, 0, 1, "Name", NULL, NULL },
   { label, 15, 55, 130, 10, 0, 0, 0, 0, 0, 1, "Width", NULL, NULL },
   { label, 15, 70, 130, 10, 0, 0, 0, 0, 0, 1, "Height", NULL, NULL },
   { label, 15, 40, 130, 10, 0, 0, 0, 0, 0, 1, "Planes", NULL, NULL },
   { label, 15, 85, 130, 10, 0, 0, 0, 0, 0, 1, "Unique BACK tiles", NULL, NULL },
   { label, 15, 100, 130, 10, 0, 0, 0, 0, 0, 1, "Unique FORE tiles", NULL, NULL },
   { label, 15, 115, 130, 10, 0, 0, 0, 0, 0, 1, "Number of ICONS", NULL, NULL },
   { label, 15, 135, 130, 10, 0, 0, 0, 0, 0, 1, "Size of MAP (Unpacked)", NULL, NULL },
   { label, 155, 25, 130, 10, 0, 0, 0, 0, 0, 1, map_name_string, NULL, NULL },
   { label, 155, 55, 130, 10, 0, 0, 0, 0, 0, 1, map_size_x, NULL, NULL },
   { label, 155, 70, 130, 10, 0, 0, 0, 0, 0, 1, map_size_y, NULL, NULL },
   { label, 155, 40, 130, 10, 0, 0, 0, 0, 0, 1, map_planes, NULL, NULL },
   { label, 155, 85, 130, 10, 0, 0, 0, 0, 0, 1, num_back, NULL, NULL },
   { label, 155, 100, 130, 10, 0, 0, 0, 0, 0, 1, num_fore, NULL, NULL },
   { label, 155, 115, 130, 10, 0, 0, 0, 0, 0, 1, num_icons, NULL, NULL },
   { label, 155, 135, 130, 10, 0, 0, 0, 0, 0, 1, map_size, NULL, NULL },
   { NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL }
};