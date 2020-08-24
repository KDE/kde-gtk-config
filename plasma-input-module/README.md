# how to setup

1) make sure you have another GTK IM module, as this relies on wrapping those
2) install this to the same prefix as the other GTK IM module
3) run `gtk-query-immodules-$GTK_VERSION > /prefix/gtk-3.0/3.0.0/immodules.cache`
4) set your GTK IM module to `plasma-` + the ID of your IM module
5) enjoy