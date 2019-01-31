/* Generated by GOB (v2.0.20)   (do not edit directly) */

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#ifndef __GDVE_COLUMN_DELIMITER_H__
#define __GDVE_COLUMN_DELIMITER_H__


/*
 * Type checking and casting macros
 */
#define GDVE_TYPE_COLUMN_DELIMITER	(gdve_column_delimiter_get_type())

G_DECLARE_DERIVABLE_TYPE (GdveColumnDelimiter, gdve_column_delimiter, GDVE, COLUMN_DELIMITER, GtkWidget)
GdveColumnDelimiter * 	gdve_column_delimiter_new	(void);
GdveColumnDelimiter * 	gdve_column_delimiter_copy	(GdveColumnDelimiter * self);

/*
 * Argument wrapping macros
 */
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define GDVE_COLUMN_DELIMITER_PROP_SYMBOL(arg)    	"symbol", __extension__ ({gchar *z = (arg); z;})
#define GDVE_COLUMN_DELIMITER_GET_PROP_SYMBOL(arg)	"symbol", __extension__ ({gchar **z = (arg); z;})
#else /* __GNUC__ && !__STRICT_ANSI__ */
#define GDVE_COLUMN_DELIMITER_PROP_SYMBOL(arg)    	"symbol",(gchar *)(arg)
#define GDVE_COLUMN_DELIMITER_GET_PROP_SYMBOL(arg)	"symbol",(gchar **)(arg)
#endif /* __GNUC__ && !__STRICT_ANSI__ */


#endif