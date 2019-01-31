/* Generated by GOB (v2.0.20)   (do not edit directly) */

/* End world hunger, donate to the World Food Programme, http://www.wfp.org */

#define GOB_VERSION_MAJOR 2
#define GOB_VERSION_MINOR 0
#define GOB_VERSION_PATCHLEVEL 20

#define selfp (gdve_column_delimiter_get_instance_private(self))

#include <string.h> /* memset() */

#include "sourceview/gdve-column-delimiter.h"
#include "gdve-central.h"

#ifdef G_LIKELY
#define ___GOB_LIKELY(expr) G_LIKELY(expr)
#define ___GOB_UNLIKELY(expr) G_UNLIKELY(expr)
#else /* ! G_LIKELY */
#define ___GOB_LIKELY(expr) (expr)
#define ___GOB_UNLIKELY(expr) (expr)
#endif /* G_LIKELY */

struct _GdveColumnDelimiterClass {
	GtkWidgetClass __parent__;
};


typedef struct _GdveColumnDelimiterPrivate GdveColumnDelimiterPrivate;
struct _GdveColumnDelimiterPrivate {
	char * symbol;
};

/* here are local prototypes */
static void ___object_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void ___object_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
static void gdve_column_delimiter_class_init (GdveColumnDelimiterClass * c) G_GNUC_UNUSED;
static void gdve_column_delimiter_init (GdveColumnDelimiter * self) G_GNUC_UNUSED;

enum {
	PROP_0,
	PROP_SYMBOL
};

/* pointer to the class of our parent */
static GtkWidgetClass *parent_class = NULL;

/* Short form macros */
#define self_get_symbol gdve_column_delimiter_get_symbol
#define self_set_symbol gdve_column_delimiter_set_symbol
#define self_new gdve_column_delimiter_new
#define self_copy gdve_column_delimiter_copy
GType
gdve_column_delimiter_get_type (void)
{
	static GType type = 0;

	if ___GOB_UNLIKELY(type == 0) {
		static const GTypeInfo info = {
			sizeof (GdveColumnDelimiterClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) gdve_column_delimiter_class_init,
			(GClassFinalizeFunc) NULL,
			NULL /* class_data */,
			sizeof (GdveColumnDelimiter),
			0 /* n_preallocs */,
			(GInstanceInitFunc) gdve_column_delimiter_init,
			NULL
		};

		type = g_type_register_static (GTK_TYPE_WIDGET, "GdveColumnDelimiter", &info, (GTypeFlags)0);
	}

	return type;
}

/* a macro for creating a new object of our type */
#define GET_NEW ((GdveColumnDelimiter *)g_object_new(gdve_column_delimiter_get_type(), NULL))

/* a function for creating a new object of our type */
#include <stdarg.h>
static GdveColumnDelimiter * GET_NEW_VARG (const char *first, ...) G_GNUC_UNUSED;
static GdveColumnDelimiter *
GET_NEW_VARG (const char *first, ...)
{
	GdveColumnDelimiter *ret;
	va_list ap;
	va_start (ap, first);
	ret = (GdveColumnDelimiter *)g_object_new_valist (gdve_column_delimiter_get_type (), first, ap);
	va_end (ap);
	return ret;
}


static void
___finalize(GObject *obj_self)
{
#define __GOB_FUNCTION__ "Gdve:Column:Delimiter::finalize"
	GdveColumnDelimiter *self G_GNUC_UNUSED = GDVE_COLUMN_DELIMITER (obj_self);
  GdveColumnDelimiterPrivate *priv G_GNUC_UNUSED = G_TYPE_INSTANCE_GET_PRIVATE(self,GDVE_TYPE_COLUMN_DELIMITER,GdveColumnDelimiterPrivate);
	if(priv->symbol) { g_free ((gpointer) priv->symbol); priv->symbol = NULL; }
	if(G_OBJECT_CLASS(parent_class)->finalize) \
		(* G_OBJECT_CLASS(parent_class)->finalize)(obj_self);
}
#undef __GOB_FUNCTION__

static void
gdve_column_delimiter_class_init (GdveColumnDelimiterClass * c G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "Gdve:Column:Delimiter::class_init"
	GObjectClass *g_object_class G_GNUC_UNUSED = (GObjectClass*) c;

	//g_type_class_add_private(c,sizeof(GdveColumnDelimiterPrivate));

	parent_class = g_type_class_ref (GTK_TYPE_WIDGET);

	g_object_class->finalize = ___finalize;
	g_object_class->get_property = ___object_get_property;
	g_object_class->set_property = ___object_set_property;
    {
	GParamSpec   *param_spec;

	param_spec = g_param_spec_string
		("symbol" /* name */,
		 _("Symbol") /* nick */,
		 _("The symbol to separate columns from each other") /* blurb */,
		 " " /* default_value */,
		 (GParamFlags)(G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (g_object_class,
		PROP_SYMBOL,
		param_spec);
    }
}
#undef __GOB_FUNCTION__
static void
gdve_column_delimiter_init (GdveColumnDelimiter * self G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "Gdve:Column:Delimiter::init"
  GdveColumnDelimiterPrivate *priv G_GNUC_UNUSED = G_TYPE_INSTANCE_GET_PRIVATE(self,GDVE_TYPE_COLUMN_DELIMITER,GdveColumnDelimiterPrivate);

	priv->symbol = g_strdup(" ");
}
#undef __GOB_FUNCTION__

static void
draw_column_delimiter_at_pos (cairo_t      *cr,
                              GdkRectangle  rect)
{
	gint x, y;
	gdouble w, h;

	x = rect.x;
	y = rect.y + rect.height / 3;

	w = 2 * rect.width;
	h = rect.height;

	cairo_save (cr);

	if (gtk_widget_get_default_direction () == GTK_TEXT_DIR_LTR)
	{
		cairo_move_to (cr, x + w * 7 / 8, y);
		cairo_rel_line_to (cr, 0, h * 1 / 3);
		cairo_rel_line_to (cr, -w * 6 / 8, 0);
		cairo_rel_line_to (cr, +h * 1 / 4, -h * 1 / 4);
		cairo_rel_move_to (cr, -h * 1 / 4, +h * 1 / 4);
		cairo_rel_line_to (cr, +h * 1 / 4, +h * 1 / 4);
	}
	else
	{
		cairo_move_to (cr, x + w * 1 / 8, y);
		cairo_rel_line_to (cr, 0, h * 1 / 3);
		cairo_rel_line_to (cr, w * 6 / 8, 0);
		cairo_rel_line_to (cr, -h * 1 / 4, -h * 1 / 4);
		cairo_rel_move_to (cr, +h * 1 / 4, +h * 1 / 4);
		cairo_rel_line_to (cr, -h * 1 / 4, -h * 1 / 4);
	}

	cairo_stroke (cr);
	cairo_restore (cr);
}

static void
draw_delimiter_at_iter (GtkTextView *text_view,
                        GtkTextIter *iter,
                        cairo_t     *cr)
{
//	gunichar ch;
	GdkRectangle rect;

	gtk_text_view_get_iter_location (text_view, iter, &rect);

	/* If the space is at a line-wrap position, or if the character is a
	 * newline, we get 0 width so we fallback to the height.
	 */
	if (rect.width == 0)
	{
		rect.width = rect.height;
	}

//	ch = gtk_text_iter_get_char (iter);
  draw_column_delimiter_at_pos (cr, rect);

}

static void
___object_set_property (GObject *object,
	guint property_id,
	const GValue *VAL G_GNUC_UNUSED,
	GParamSpec *pspec G_GNUC_UNUSED)
#define __GOB_FUNCTION__ "Gdve:Column:Delimiter::set_property"
{
	GdveColumnDelimiter *self G_GNUC_UNUSED;
  GdveColumnDelimiterPrivate *priv G_GNUC_UNUSED;

	self = GDVE_COLUMN_DELIMITER (object);
  priv = G_TYPE_INSTANCE_GET_PRIVATE(self,GDVE_TYPE_COLUMN_DELIMITER,GdveColumnDelimiterPrivate);

	switch (property_id) {
	case PROP_SYMBOL:
		{
{ char *old = priv->symbol; priv->symbol = g_value_dup_string (VAL); g_free (old); }
		}
		break;
	default:
/* Apparently in g++ this is needed, glib is b0rk */
#ifndef __PRETTY_FUNCTION__
#  undef G_STRLOC
#  define G_STRLOC	__FILE__ ":" G_STRINGIFY (__LINE__)
#endif
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}
#undef __GOB_FUNCTION__

static void
___object_get_property (GObject *object,
	guint property_id,
	GValue *VAL G_GNUC_UNUSED,
	GParamSpec *pspec G_GNUC_UNUSED)
#define __GOB_FUNCTION__ "Gdve:Column:Delimiter::get_property"
{
	GdveColumnDelimiter *self G_GNUC_UNUSED;
  GdveColumnDelimiterPrivate *priv G_GNUC_UNUSED;

	self = GDVE_COLUMN_DELIMITER (object);
  priv = G_TYPE_INSTANCE_GET_PRIVATE(self,GDVE_TYPE_COLUMN_DELIMITER,GdveColumnDelimiterPrivate);

	switch (property_id) {
	case PROP_SYMBOL:
		{
g_value_set_string (VAL, priv->symbol);
		}
		break;
	default:
/* Apparently in g++ this is needed, glib is b0rk */
#ifndef __PRETTY_FUNCTION__
#  undef G_STRLOC
#  define G_STRLOC	__FILE__ ":" G_STRINGIFY (__LINE__)
#endif
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}
#undef __GOB_FUNCTION__


gchar *
gdve_column_delimiter_get_symbol (GdveColumnDelimiter * self)
{
#define __GOB_FUNCTION__ "Gdve:Column:Delimiter::get_symbol"
{
		gchar* val; g_object_get (G_OBJECT (self), "symbol", &val, NULL); return val;
}}
#undef __GOB_FUNCTION__

void
gdve_column_delimiter_set_symbol (GdveColumnDelimiter * self, gchar * val)
{
#define __GOB_FUNCTION__ "Gdve:Column:Delimiter::set_symbol"
{
		g_object_set (G_OBJECT (self), "symbol", val, NULL);
}}
#undef __GOB_FUNCTION__


/**
 * gdve_column_delimiter_new:
 *
 * Makes a new #GtkButtonCount widget
 *
 * Returns: a new widget
 **/
GdveColumnDelimiter *
gdve_column_delimiter_new (void)
{
#define __GOB_FUNCTION__ "Gdve:Column:Delimiter::new"
{

                /* It's ok to use a normal cast here, as we are sure that we
                 * have gotten the right type */
                return (GdveColumnDelimiter *)GET_NEW;
        }}
#undef __GOB_FUNCTION__

/**
 * gdve_column_delimiter_copy:
 *
 * Makes a new #GtkButtonCount widget
 *
 * Returns: a new widget
 **/
GdveColumnDelimiter *
gdve_column_delimiter_copy (GdveColumnDelimiter * self)
{
#define __GOB_FUNCTION__ "Gdve:Column:Delimiter::copy"
{

                /* It's ok to use a normal cast here, as we are sure that we
                 * have gotten the right type */
                return (GdveColumnDelimiter *)GET_NEW;
        }}
#undef __GOB_FUNCTION__
