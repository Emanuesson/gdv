/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) <2007> Wim Taymans <wim.taymans@collabora.co.uk>
 * Copyright (C) <2007> Edward Hervey <edward.hervey@collabora.co.uk>
 * Copyright (C) <2007> Jan Schmidt <thaytan@noraisin.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/**
 * SECTION:element-alpha
 * @title: alpha
 *
 * The alpha element adds an alpha channel to a video stream. The values
 * of the alpha channel can be either be set to a constant or can be
 * dynamically calculated via chroma keying, e.g. blue can be set as
 * the transparent color.
 *
 * Sample pipeline:
 * |[
 * gst-launch-1.0 videotestsrc pattern=snow ! mixer.sink_0 \
 *   videotestsrc pattern=smpte75 ! alpha method=green ! mixer.sink_1 \
 *   videomixer name=mixer sink_0::zorder=0 sink_1::zorder=1 ! \
 *   videoconvert ! autovideosink
 * ]| This pipeline adds a alpha channel to the SMPTE color bars
 * with green as the transparent color and overlays the output on
 * top of a snow video stream.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gdvaccel3d.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI  3.14159265358979323846
#endif

/* signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

#define DEFAULT_METHOD ALPHA_METHOD_SET
#define DEFAULT_ALPHA 1.0
#define DEFAULT_TARGET_R 0
#define DEFAULT_TARGET_G 255
#define DEFAULT_TARGET_B 0
#define DEFAULT_ANGLE 20.0
#define DEFAULT_NOISE_LEVEL 2.0
#define DEFAULT_BLACK_SENSITIVITY 100
#define DEFAULT_WHITE_SENSITIVITY 100
#define DEFAULT_PREFER_PASSTHROUGH FALSE

enum
{
  PROP_0,
  PROP_METHOD,
  PROP_ALPHA,
  PROP_TARGET_R,
  PROP_TARGET_G,
  PROP_TARGET_B,
  PROP_ANGLE,
  PROP_NOISE_LEVEL,
  PROP_BLACK_SENSITIVITY,
  PROP_WHITE_SENSITIVITY,
  PROP_PREFER_PASSTHROUGH
};

static GstStaticPadTemplate gst_alpha_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("{ AYUV, "
            "ARGB, BGRA, ABGR, RGBA, Y444, xRGB, BGRx, xBGR, "
            "RGBx, RGB, BGR, Y42B, YUY2, YVYU, UYVY, I420, YV12, Y41B } "))
    );

static GstStaticPadTemplate gst_alpha_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("{ AYUV, "
            "ARGB, BGRA, ABGR, RGBA, Y444, xRGB, BGRx, xBGR, "
            "RGBx, RGB, BGR, Y42B, YUY2, YVYU, UYVY, I420, YV12, " "Y41B } "))
    );

static GstStaticCaps gst_alpha_alpha_caps =
GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("{ AYUV, ARGB, BGRA, ABGR, RGBA }"));

/* FIXME: why do we need our own lock for this? */
#define GST_ALPHA_LOCK(alpha) G_STMT_START { \
  GST_LOG_OBJECT (alpha, "Locking alpha from thread %p", g_thread_self ()); \
  g_mutex_lock (&alpha->lock); \
  GST_LOG_OBJECT (alpha, "Locked alpha from thread %p", g_thread_self ()); \
} G_STMT_END

#define GST_ALPHA_UNLOCK(alpha) G_STMT_START { \
  GST_LOG_OBJECT (alpha, "Unlocking alpha from thread %p", g_thread_self ()); \
  g_mutex_unlock (&alpha->lock); \
} G_STMT_END

static GstCaps *gst_alpha_transform_caps (GstBaseTransform * btrans,
    GstPadDirection direction, GstCaps * caps, GstCaps * filter);
static void gst_alpha_before_transform (GstBaseTransform * btrans,
    GstBuffer * buf);

static gboolean gst_alpha_set_info (GstVideoFilter * filter,
    GstCaps * incaps, GstVideoInfo * in_info, GstCaps * outcaps,
    GstVideoInfo * out_info);
static GstFlowReturn gst_alpha_transform_frame (GstVideoFilter * filter,
    GstVideoFrame * in_frame, GstVideoFrame * out_frame);

static void gst_alpha_init_params_full (GstAlpha * alpha,
    const GstVideoFormatInfo * in_info, const GstVideoFormatInfo * out_info);
static void gst_alpha_init_params (GstAlpha * alpha);
static void gst_alpha_set_process_function (GstAlpha * alpha);
static gboolean gst_alpha_set_process_function_full (GstAlpha * alpha,
    GstVideoInfo * in_info, GstVideoInfo * out_info);

static void gst_alpha_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_alpha_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);
static void gst_alpha_finalize (GObject * object);

#define gst_alpha_parent_class parent_class
G_DEFINE_TYPE (GstAlpha, gst_alpha, GST_TYPE_VIDEO_FILTER);

#define GST_TYPE_ALPHA_METHOD (gst_alpha_method_get_type())
static GType
gst_alpha_method_get_type (void)
{
  static GType alpha_method_type = 0;
  static const GEnumValue alpha_method[] = {
    {ALPHA_METHOD_SET, "Set/adjust alpha channel", "set"},
    {ALPHA_METHOD_GREEN, "Chroma Key on pure green", "green"},
    {ALPHA_METHOD_BLUE, "Chroma Key on pure blue", "blue"},
    {ALPHA_METHOD_CUSTOM, "Chroma Key on custom RGB values", "custom"},
    {0, NULL, NULL},
  };

  if (!alpha_method_type) {
    alpha_method_type = g_enum_register_static ("GstAlphaMethod", alpha_method);
  }
  return alpha_method_type;
}

static void
gst_alpha_class_init (GstAlphaClass * klass)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;
  GstElementClass *gstelement_class = (GstElementClass *) klass;
  GstBaseTransformClass *btrans_class = (GstBaseTransformClass *) klass;
  GstVideoFilterClass *vfilter_class = (GstVideoFilterClass *) klass;

  GST_DEBUG_CATEGORY_INIT (gst_alpha_debug, "alpha", 0,
      "alpha - Element for adding alpha channel to streams");

  gobject_class->set_property = gst_alpha_set_property;
  gobject_class->get_property = gst_alpha_get_property;
  gobject_class->finalize = gst_alpha_finalize;

  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_METHOD,
      g_param_spec_enum ("method", "Method",
          "How the alpha channels should be created", GST_TYPE_ALPHA_METHOD,
          DEFAULT_METHOD, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_ALPHA,
      g_param_spec_double ("alpha", "Alpha", "The value for the alpha channel",
          0.0, 1.0, DEFAULT_ALPHA,
          G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_TARGET_R,
      g_param_spec_uint ("target-r", "Target Red",
          "The red color value for custom RGB chroma keying", 0, 255,
          DEFAULT_TARGET_R,
          G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_TARGET_G,
      g_param_spec_uint ("target-g", "Target Green",
          "The green color value for custom RGB chroma keying", 0, 255,
          DEFAULT_TARGET_G,
          G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_TARGET_B,
      g_param_spec_uint ("target-b", "Target Blue",
          "The blue color value for custom RGB chroma keying", 0, 255,
          DEFAULT_TARGET_B,
          G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_ANGLE,
      g_param_spec_float ("angle", "Angle", "Size of the colorcube to change",
          0.0, 90.0, DEFAULT_ANGLE,
          G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (G_OBJECT_CLASS (klass), PROP_NOISE_LEVEL,
      g_param_spec_float ("noise-level", "Noise Level", "Size of noise radius",
          0.0, 64.0, DEFAULT_NOISE_LEVEL,
          G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (G_OBJECT_CLASS (klass),
      PROP_BLACK_SENSITIVITY, g_param_spec_uint ("black-sensitivity",
          "Black Sensitivity", "Sensitivity to dark colors", 0, 128,
          DEFAULT_BLACK_SENSITIVITY,
          G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (G_OBJECT_CLASS (klass),
      PROP_WHITE_SENSITIVITY, g_param_spec_uint ("white-sensitivity",
          "White Sensitivity", "Sensitivity to bright colors", 0, 128,
          DEFAULT_WHITE_SENSITIVITY,
          G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (G_OBJECT_CLASS (klass),
      PROP_PREFER_PASSTHROUGH, g_param_spec_boolean ("prefer-passthrough",
          "Prefer Passthrough",
          "Don't do any processing for alpha=1.0 if possible",
          DEFAULT_PREFER_PASSTHROUGH,
          G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE | G_PARAM_STATIC_STRINGS));

  gst_element_class_set_static_metadata (gstelement_class, "Alpha filter",
      "Filter/Effect/Video",
      "Adds an alpha channel to video - uniform or via chroma-keying",
      "Wim Taymans <wim.taymans@gmail.com>\n"
      "Edward Hervey <edward.hervey@collabora.co.uk>\n"
      "Jan Schmidt <thaytan@noraisin.net>");

  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_alpha_sink_template);
  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_alpha_src_template);

  btrans_class->before_transform =
      GST_DEBUG_FUNCPTR (gst_alpha_before_transform);
  btrans_class->transform_caps = GST_DEBUG_FUNCPTR (gst_alpha_transform_caps);

  vfilter_class->set_info = GST_DEBUG_FUNCPTR (gst_alpha_set_info);
  vfilter_class->transform_frame =
      GST_DEBUG_FUNCPTR (gst_alpha_transform_frame);
}

static void
gst_alpha_init (GstAlpha * alpha)
{
  alpha->alpha = DEFAULT_ALPHA;
  alpha->method = DEFAULT_METHOD;
  alpha->target_r = DEFAULT_TARGET_R;
  alpha->target_g = DEFAULT_TARGET_G;
  alpha->target_b = DEFAULT_TARGET_B;
  alpha->angle = DEFAULT_ANGLE;
  alpha->noise_level = DEFAULT_NOISE_LEVEL;
  alpha->black_sensitivity = DEFAULT_BLACK_SENSITIVITY;
  alpha->white_sensitivity = DEFAULT_WHITE_SENSITIVITY;

  g_mutex_init (&alpha->lock);
}

static void
gst_alpha_finalize (GObject * object)
{
  GstAlpha *alpha = GST_ALPHA (object);

  g_mutex_clear (&alpha->lock);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gst_alpha_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstAlpha *alpha = GST_ALPHA (object);
  gboolean reconfigure = FALSE;

  GST_ALPHA_LOCK (alpha);
  switch (prop_id) {
    case PROP_METHOD:{
      gint method = g_value_get_enum (value);

      reconfigure = (method != alpha->method) && (method == ALPHA_METHOD_SET
          || alpha->method == ALPHA_METHOD_SET) && (alpha->alpha == 1.0)
          && (alpha->prefer_passthrough);
      alpha->method = method;

      gst_alpha_set_process_function (alpha);
      gst_alpha_init_params (alpha);
      break;
    }
    case PROP_ALPHA:{
      gdouble a = g_value_get_double (value);

      reconfigure = (a != alpha->alpha) && (a == 1.0 || alpha->alpha == 1.0)
          && (alpha->method == ALPHA_METHOD_SET) && (alpha->prefer_passthrough);
      alpha->alpha = a;
      break;
    }
    case PROP_TARGET_R:
      alpha->target_r = g_value_get_uint (value);
      gst_alpha_init_params (alpha);
      break;
    case PROP_TARGET_G:
      alpha->target_g = g_value_get_uint (value);
      gst_alpha_init_params (alpha);
      break;
    case PROP_TARGET_B:
      alpha->target_b = g_value_get_uint (value);
      gst_alpha_init_params (alpha);
      break;
    case PROP_ANGLE:
      alpha->angle = g_value_get_float (value);
      gst_alpha_init_params (alpha);
      break;
    case PROP_NOISE_LEVEL:
      alpha->noise_level = g_value_get_float (value);
      gst_alpha_init_params (alpha);
      break;
    case PROP_BLACK_SENSITIVITY:
      alpha->black_sensitivity = g_value_get_uint (value);
      break;
    case PROP_WHITE_SENSITIVITY:
      alpha->white_sensitivity = g_value_get_uint (value);
      break;
    case PROP_PREFER_PASSTHROUGH:{
      gboolean prefer_passthrough = g_value_get_boolean (value);

      reconfigure = ((! !prefer_passthrough) != (! !alpha->prefer_passthrough))
          && (alpha->method == ALPHA_METHOD_SET) && (alpha->alpha == 1.0);
      alpha->prefer_passthrough = prefer_passthrough;
      break;
    }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }

  if (reconfigure)
    gst_base_transform_reconfigure_src (GST_BASE_TRANSFORM_CAST (alpha));

  GST_ALPHA_UNLOCK (alpha);
}

static void
gst_alpha_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec * pspec)
{
  GstAlpha *alpha = GST_ALPHA (object);

  switch (prop_id) {
    case PROP_METHOD:
      g_value_set_enum (value, alpha->method);
      break;
    case PROP_ALPHA:
      g_value_set_double (value, alpha->alpha);
      break;
    case PROP_TARGET_R:
      g_value_set_uint (value, alpha->target_r);
      break;
    case PROP_TARGET_G:
      g_value_set_uint (value, alpha->target_g);
      break;
    case PROP_TARGET_B:
      g_value_set_uint (value, alpha->target_b);
      break;
    case PROP_ANGLE:
      g_value_set_float (value, alpha->angle);
      break;
    case PROP_NOISE_LEVEL:
      g_value_set_float (value, alpha->noise_level);
      break;
    case PROP_BLACK_SENSITIVITY:
      g_value_set_uint (value, alpha->black_sensitivity);
      break;
    case PROP_WHITE_SENSITIVITY:
      g_value_set_uint (value, alpha->white_sensitivity);
      break;
    case PROP_PREFER_PASSTHROUGH:
      g_value_set_boolean (value, alpha->prefer_passthrough);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstCaps *
gst_alpha_transform_caps (GstBaseTransform * btrans,
    GstPadDirection direction, GstCaps * caps, GstCaps * filter)
{
  GstAlpha *alpha = GST_ALPHA (btrans);
  GstCaps *ret, *tmp, *tmp2;
  GstStructure *structure;
  gint i;

  tmp = gst_caps_new_empty ();

  GST_ALPHA_LOCK (alpha);
  for (i = 0; i < gst_caps_get_size (caps); i++) {
    structure = gst_structure_copy (gst_caps_get_structure (caps, i));

    gst_structure_remove_field (structure, "format");
    gst_structure_remove_field (structure, "colorimetry");
    gst_structure_remove_field (structure, "chroma-site");

    gst_caps_append_structure (tmp, structure);
  }

  if (direction == GST_PAD_SINK) {
    tmp2 = gst_static_caps_get (&gst_alpha_alpha_caps);
    ret = gst_caps_intersect (tmp, tmp2);
    gst_caps_unref (tmp);
    gst_caps_unref (tmp2);
    tmp = ret;
    ret = NULL;

    if (alpha->prefer_passthrough && alpha->method == ALPHA_METHOD_SET
        && alpha->alpha == 1.0) {
      ret = gst_caps_copy (caps);
      gst_caps_append (ret, tmp);
      tmp = NULL;
    } else {
      ret = tmp;
      tmp = NULL;
    }
  } else {
    ret = tmp;
    tmp = NULL;
  }

  GST_DEBUG_OBJECT (alpha,
      "Transformed %" GST_PTR_FORMAT " -> %" GST_PTR_FORMAT, caps, ret);

  if (filter) {
    GstCaps *intersection;

    GST_DEBUG_OBJECT (alpha, "Using filter caps %" GST_PTR_FORMAT, filter);
    intersection =
        gst_caps_intersect_full (filter, ret, GST_CAPS_INTERSECT_FIRST);
    gst_caps_unref (ret);
    ret = intersection;
    GST_DEBUG_OBJECT (alpha, "Intersection %" GST_PTR_FORMAT, ret);
  }


  GST_ALPHA_UNLOCK (alpha);

  return ret;
}

static gboolean
gst_alpha_set_info (GstVideoFilter * filter,
    GstCaps * incaps, GstVideoInfo * in_info, GstCaps * outcaps,
    GstVideoInfo * out_info)
{
  GstAlpha *alpha = GST_ALPHA (filter);
  gboolean passthrough;

  GST_ALPHA_LOCK (alpha);

  alpha->in_sdtv = in_info->colorimetry.matrix == GST_VIDEO_COLOR_MATRIX_BT601;
  alpha->out_sdtv =
      out_info->colorimetry.matrix == GST_VIDEO_COLOR_MATRIX_BT601;

  passthrough = alpha->prefer_passthrough &&
      GST_VIDEO_INFO_FORMAT (in_info) == GST_VIDEO_INFO_FORMAT (out_info)
      && alpha->in_sdtv == alpha->out_sdtv && alpha->method == ALPHA_METHOD_SET
      && alpha->alpha == 1.0;

  GST_DEBUG_OBJECT (alpha,
      "Setting caps %" GST_PTR_FORMAT " -> %" GST_PTR_FORMAT
      " (passthrough: %d)", incaps, outcaps, passthrough);
  gst_base_transform_set_passthrough (GST_BASE_TRANSFORM_CAST (filter),
      passthrough);

  if (!gst_alpha_set_process_function_full (alpha, in_info, out_info)
      && !passthrough)
    goto no_process;

  gst_alpha_init_params_full (alpha, in_info->finfo, out_info->finfo);

  GST_ALPHA_UNLOCK (alpha);

  return TRUE;

  /* ERRORS */
no_process:
  {
    GST_WARNING_OBJECT (alpha,
        "No processing function for this caps and no passthrough mode");
    GST_ALPHA_UNLOCK (alpha);
    return FALSE;
  }
}

/* based on http://www.cs.utah.edu/~michael/chroma/
 */
static inline gint
chroma_keying_yuv (gint a, gint * y, gint * u,
    gint * v, gint cr, gint cb, gint smin, gint smax, guint8 accept_angle_tg,
    guint8 accept_angle_ctg, guint8 one_over_kc, guint8 kfgy_scale, gint8 kg,
    guint noise_level2)
{
  gint tmp, tmp1;
  gint x1, y1;
  gint x, z;
  gint b_alpha;

  /* too dark or too bright, keep alpha */
  if (*y < smin || *y > smax)
    return a;

  /* Convert foreground to XZ coords where X direction is defined by
     the key color */
  tmp = ((*u) * cb + (*v) * cr) >> 7;
  x = CLAMP (tmp, -128, 127);
  tmp = ((*v) * cb - (*u) * cr) >> 7;
  z = CLAMP (tmp, -128, 127);

  /* WARNING: accept angle should never be set greater than "somewhat less
     than 90 degrees" to avoid dealing with negative/infinite tg. In reality,
     80 degrees should be enough if foreground is reasonable. If this seems
     to be a problem, go to alternative ways of checking point position
     (scalar product or line equations). This angle should not be too small
     either to avoid infinite ctg (used to suppress foreground without use of
     division) */

  tmp = (x * accept_angle_tg) >> 4;
  tmp = MIN (tmp, 127);

  if (abs (z) > tmp) {
    /* keep foreground Kfg = 0 */
    return a;
  }
  /* Compute Kfg (implicitly) and Kbg, suppress foreground in XZ coord
     according to Kfg */
  tmp = (z * accept_angle_ctg) >> 4;
  tmp = CLAMP (tmp, -128, 127);
  x1 = abs (tmp);
  y1 = z;

  tmp1 = x - x1;
  tmp1 = MAX (tmp1, 0);
  b_alpha = (tmp1 * one_over_kc) / 2;
  b_alpha = 255 - CLAMP (b_alpha, 0, 255);
  b_alpha = (a * b_alpha) >> 8;

  tmp = (tmp1 * kfgy_scale) >> 4;
  tmp1 = MIN (tmp, 255);

  *y = (*y < tmp1) ? 0 : *y - tmp1;

  /* Convert suppressed foreground back to CbCr */
  tmp = (x1 * cb - y1 * cr) >> 7;
  *u = CLAMP (tmp, -128, 127);

  tmp = (x1 * cr + y1 * cb) >> 7;
  *v = CLAMP (tmp, -128, 127);

  /* Deal with noise. For now, a circle around the key color with
     radius of noise_level treated as exact key color. Introduces
     sharp transitions.
   */
  tmp = z * z + (x - kg) * (x - kg);
  tmp = MIN (tmp, 0xffff);

  if (tmp < noise_level2)
    b_alpha = 0;

  return b_alpha;
}

#define APPLY_MATRIX(m,o,v1,v2,v3) ((m[o*4] * v1 + m[o*4+1] * v2 + m[o*4+2] * v3 + m[o*4+3]) >> 8)

static void
gst_alpha_set_argb_ayuv (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  gint s_alpha = CLAMP ((gint) (alpha->alpha * 256), 0, 256);
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint i, j;
  gint matrix[12];
  gint y, u, v;
  gint o[4];

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  o[0] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 3);
  o[1] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 0);
  o[2] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 1);
  o[3] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 2);

  memcpy (matrix,
      alpha->out_sdtv ? cog_rgb_to_ycbcr_matrix_8bit_sdtv :
      cog_rgb_to_ycbcr_matrix_8bit_hdtv, 12 * sizeof (gint));

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      dest[0] = (src[o[0]] * s_alpha) >> 8;

      y = APPLY_MATRIX (matrix, 0, src[o[1]], src[o[2]], src[o[3]]);
      u = APPLY_MATRIX (matrix, 1, src[o[1]], src[o[2]], src[o[3]]);
      v = APPLY_MATRIX (matrix, 2, src[o[1]], src[o[2]], src[o[3]]);

      dest[1] = y;
      dest[2] = u;
      dest[3] = v;

      dest += 4;
      src += 4;
    }
  }
}

static void
gst_alpha_chroma_key_argb_ayuv (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint i, j;
  gint a, y, u, v;
  gint r, g, b;
  gint smin, smax;
  gint pa = CLAMP ((gint) (alpha->alpha * 256), 0, 256);
  gint8 cb = alpha->cb, cr = alpha->cr;
  gint8 kg = alpha->kg;
  guint8 accept_angle_tg = alpha->accept_angle_tg;
  guint8 accept_angle_ctg = alpha->accept_angle_ctg;
  guint8 one_over_kc = alpha->one_over_kc;
  guint8 kfgy_scale = alpha->kfgy_scale;
  guint noise_level2 = alpha->noise_level2;
  gint matrix[12];
  gint o[4];

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  o[0] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 3);
  o[1] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 0);
  o[2] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 1);
  o[3] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 2);

  smin = 128 - alpha->black_sensitivity;
  smax = 128 + alpha->white_sensitivity;

  memcpy (matrix,
      alpha->out_sdtv ? cog_rgb_to_ycbcr_matrix_8bit_sdtv :
      cog_rgb_to_ycbcr_matrix_8bit_hdtv, 12 * sizeof (gint));

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      a = (src[o[0]] * pa) >> 8;
      r = src[o[1]];
      g = src[o[2]];
      b = src[o[3]];

      y = APPLY_MATRIX (matrix, 0, r, g, b);
      u = APPLY_MATRIX (matrix, 1, r, g, b) - 128;
      v = APPLY_MATRIX (matrix, 2, r, g, b) - 128;

      a = chroma_keying_yuv (a, &y, &u, &v, cr, cb,
          smin, smax, accept_angle_tg, accept_angle_ctg,
          one_over_kc, kfgy_scale, kg, noise_level2);

      u += 128;
      v += 128;

      dest[0] = a;
      dest[1] = y;
      dest[2] = u;
      dest[3] = v;

      src += 4;
      dest += 4;
    }
  }
}

static void
gst_alpha_set_argb_argb (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint s_alpha = CLAMP ((gint) (alpha->alpha * 256), 0, 256);
  gint i, j;
  gint p[4], o[4];

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  p[0] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 3);
  p[1] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 0);
  p[2] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 1);
  p[3] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 2);

  o[0] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 3);
  o[1] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 0);
  o[2] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 1);
  o[3] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 2);

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      dest[p[0]] = (src[o[0]] * s_alpha) >> 8;

      dest[p[1]] = src[o[1]];
      dest[p[2]] = src[o[2]];
      dest[p[3]] = src[o[3]];

      dest += 4;
      src += 4;
    }
  }
}

static void
gst_alpha_chroma_key_argb_argb (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint i, j;
  gint a, y, u, v;
  gint r, g, b;
  gint smin, smax;
  gint pa = CLAMP ((gint) (alpha->alpha * 256), 0, 256);
  gint8 cb = alpha->cb, cr = alpha->cr;
  gint8 kg = alpha->kg;
  guint8 accept_angle_tg = alpha->accept_angle_tg;
  guint8 accept_angle_ctg = alpha->accept_angle_ctg;
  guint8 one_over_kc = alpha->one_over_kc;
  guint8 kfgy_scale = alpha->kfgy_scale;
  guint noise_level2 = alpha->noise_level2;
  gint matrix[12], matrix2[12];
  gint p[4], o[4];

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  p[0] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 3);
  p[1] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 0);
  p[2] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 1);
  p[3] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 2);

  o[0] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 3);
  o[1] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 0);
  o[2] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 1);
  o[3] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 2);

  smin = 128 - alpha->black_sensitivity;
  smax = 128 + alpha->white_sensitivity;

  memcpy (matrix, cog_rgb_to_ycbcr_matrix_8bit_sdtv, 12 * sizeof (gint));
  memcpy (matrix2, cog_ycbcr_to_rgb_matrix_8bit_sdtv, 12 * sizeof (gint));

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      a = (src[o[0]] * pa) >> 8;
      r = src[o[1]];
      g = src[o[2]];
      b = src[o[3]];

      y = APPLY_MATRIX (matrix, 0, r, g, b);
      u = APPLY_MATRIX (matrix, 1, r, g, b) - 128;
      v = APPLY_MATRIX (matrix, 2, r, g, b) - 128;

      a = chroma_keying_yuv (a, &y, &u, &v, cr, cb,
          smin, smax, accept_angle_tg, accept_angle_ctg,
          one_over_kc, kfgy_scale, kg, noise_level2);

      u += 128;
      v += 128;

      r = APPLY_MATRIX (matrix2, 0, y, u, v);
      g = APPLY_MATRIX (matrix2, 1, y, u, v);
      b = APPLY_MATRIX (matrix2, 2, y, u, v);

      dest[p[0]] = a;
      dest[p[1]] = CLAMP (r, 0, 255);
      dest[p[2]] = CLAMP (g, 0, 255);
      dest[p[3]] = CLAMP (b, 0, 255);

      src += 4;
      dest += 4;
    }
  }
}

static void
gst_alpha_set_ayuv_argb (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint s_alpha = CLAMP ((gint) (alpha->alpha * 256), 0, 256);
  gint y, x;
  gint matrix[12];
  gint r, g, b;
  gint p[4];

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  p[0] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 3);
  p[1] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 0);
  p[2] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 1);
  p[3] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 2);

  memcpy (matrix,
      alpha->in_sdtv ? cog_ycbcr_to_rgb_matrix_8bit_sdtv :
      cog_ycbcr_to_rgb_matrix_8bit_hdtv, 12 * sizeof (gint));

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      dest[p[0]] = (src[0] * s_alpha) >> 8;

      r = APPLY_MATRIX (matrix, 0, src[1], src[2], src[3]);
      g = APPLY_MATRIX (matrix, 1, src[1], src[2], src[3]);
      b = APPLY_MATRIX (matrix, 2, src[1], src[2], src[3]);

      dest[p[1]] = CLAMP (r, 0, 255);
      dest[p[2]] = CLAMP (g, 0, 255);
      dest[p[3]] = CLAMP (b, 0, 255);

      dest += 4;
      src += 4;
    }
  }
}

static void
gst_alpha_chroma_key_ayuv_argb (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint i, j;
  gint a, y, u, v;
  gint r, g, b;
  gint smin, smax;
  gint pa = CLAMP ((gint) (alpha->alpha * 256), 0, 256);
  gint8 cb = alpha->cb, cr = alpha->cr;
  gint8 kg = alpha->kg;
  guint8 accept_angle_tg = alpha->accept_angle_tg;
  guint8 accept_angle_ctg = alpha->accept_angle_ctg;
  guint8 one_over_kc = alpha->one_over_kc;
  guint8 kfgy_scale = alpha->kfgy_scale;
  guint noise_level2 = alpha->noise_level2;
  gint matrix[12];
  gint p[4];

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  p[0] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 3);
  p[1] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 0);
  p[2] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 1);
  p[3] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 2);

  smin = 128 - alpha->black_sensitivity;
  smax = 128 + alpha->white_sensitivity;

  memcpy (matrix,
      alpha->in_sdtv ? cog_ycbcr_to_rgb_matrix_8bit_sdtv :
      cog_ycbcr_to_rgb_matrix_8bit_hdtv, 12 * sizeof (gint));

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      a = (src[0] * pa) >> 8;
      y = src[1];
      u = src[2] - 128;
      v = src[3] - 128;

      a = chroma_keying_yuv (a, &y, &u, &v, cr, cb,
          smin, smax, accept_angle_tg, accept_angle_ctg,
          one_over_kc, kfgy_scale, kg, noise_level2);

      u += 128;
      v += 128;

      r = APPLY_MATRIX (matrix, 0, y, u, v);
      g = APPLY_MATRIX (matrix, 1, y, u, v);
      b = APPLY_MATRIX (matrix, 2, y, u, v);

      dest[p[0]] = a;
      dest[p[1]] = CLAMP (r, 0, 255);
      dest[p[2]] = CLAMP (g, 0, 255);
      dest[p[3]] = CLAMP (b, 0, 255);

      src += 4;
      dest += 4;
    }
  }
}

static void
gst_alpha_set_ayuv_ayuv (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint s_alpha = CLAMP ((gint) (alpha->alpha * 256), 0, 256);
  gint y, x;

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  if (alpha->in_sdtv == alpha->out_sdtv) {
    for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
        dest[0] = (src[0] * s_alpha) >> 8;
        dest[1] = src[1];
        dest[2] = src[2];
        dest[3] = src[3];

        dest += 4;
        src += 4;
      }
    }
  } else {
    gint matrix[12];

    memcpy (matrix,
        alpha->out_sdtv ? cog_ycbcr_hdtv_to_ycbcr_sdtv_matrix_8bit :
        cog_ycbcr_sdtv_to_ycbcr_hdtv_matrix_8bit, 12 * sizeof (gint));

    for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
        dest[0] = (src[0] * s_alpha) >> 8;
        dest[1] = APPLY_MATRIX (matrix, 0, src[1], src[2], src[3]);
        dest[2] = APPLY_MATRIX (matrix, 1, src[1], src[2], src[3]);
        dest[3] = APPLY_MATRIX (matrix, 2, src[1], src[2], src[3]);

        dest += 4;
        src += 4;
      }
    }
  }
}

static void
gst_alpha_chroma_key_ayuv_ayuv (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint i, j;
  gint a, y, u, v;
  gint smin, smax;
  gint pa = CLAMP ((gint) (alpha->alpha * 256), 0, 256);
  gint8 cb = alpha->cb, cr = alpha->cr;
  gint8 kg = alpha->kg;
  guint8 accept_angle_tg = alpha->accept_angle_tg;
  guint8 accept_angle_ctg = alpha->accept_angle_ctg;
  guint8 one_over_kc = alpha->one_over_kc;
  guint8 kfgy_scale = alpha->kfgy_scale;
  guint noise_level2 = alpha->noise_level2;

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  smin = 128 - alpha->black_sensitivity;
  smax = 128 + alpha->white_sensitivity;

  if (alpha->in_sdtv == alpha->out_sdtv) {
    for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {
        a = (src[0] * pa) >> 8;
        y = src[1];
        u = src[2] - 128;
        v = src[3] - 128;

        a = chroma_keying_yuv (a, &y, &u, &v, cr, cb,
            smin, smax, accept_angle_tg, accept_angle_ctg,
            one_over_kc, kfgy_scale, kg, noise_level2);

        u += 128;
        v += 128;

        dest[0] = a;
        dest[1] = y;
        dest[2] = u;
        dest[3] = v;

        src += 4;
        dest += 4;
      }
    }
  } else {
    gint matrix[12];

    memcpy (matrix,
        alpha->out_sdtv ? cog_ycbcr_hdtv_to_ycbcr_sdtv_matrix_8bit :
        cog_ycbcr_sdtv_to_ycbcr_hdtv_matrix_8bit, 12 * sizeof (gint));

    for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {
        a = (src[0] * pa) >> 8;
        y = APPLY_MATRIX (matrix, 0, src[1], src[2], src[3]);
        u = APPLY_MATRIX (matrix, 1, src[1], src[2], src[3]) - 128;
        v = APPLY_MATRIX (matrix, 2, src[1], src[2], src[3]) - 128;

        a = chroma_keying_yuv (a, &y, &u, &v, cr, cb,
            smin, smax, accept_angle_tg, accept_angle_ctg,
            one_over_kc, kfgy_scale, kg, noise_level2);

        u += 128;
        v += 128;

        dest[0] = a;
        dest[1] = y;
        dest[2] = u;
        dest[3] = v;

        src += 4;
        dest += 4;
      }
    }
  }
}

static void
gst_alpha_set_rgb_ayuv (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint s_alpha = CLAMP ((gint) (alpha->alpha * 255), 0, 255);
  gint i, j;
  gint matrix[12];
  gint y, u, v;
  gint o[3];
  gint bpp;

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  bpp = GST_VIDEO_FRAME_COMP_PSTRIDE (in_frame, 0);
  o[0] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 0);
  o[1] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 1);
  o[2] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 2);

  memcpy (matrix,
      alpha->out_sdtv ? cog_rgb_to_ycbcr_matrix_8bit_sdtv :
      cog_rgb_to_ycbcr_matrix_8bit_hdtv, 12 * sizeof (gint));

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      dest[0] = s_alpha;

      y = APPLY_MATRIX (matrix, 0, src[o[0]], src[o[1]], src[o[2]]);
      u = APPLY_MATRIX (matrix, 1, src[o[0]], src[o[1]], src[o[2]]);
      v = APPLY_MATRIX (matrix, 2, src[o[0]], src[o[1]], src[o[2]]);

      dest[1] = y;
      dest[2] = u;
      dest[3] = v;

      dest += 4;
      src += bpp;
    }
  }
}

static void
gst_alpha_chroma_key_rgb_ayuv (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint i, j;
  gint a, y, u, v;
  gint r, g, b;
  gint smin, smax;
  gint pa = CLAMP ((gint) (alpha->alpha * 255), 0, 255);
  gint8 cb = alpha->cb, cr = alpha->cr;
  gint8 kg = alpha->kg;
  guint8 accept_angle_tg = alpha->accept_angle_tg;
  guint8 accept_angle_ctg = alpha->accept_angle_ctg;
  guint8 one_over_kc = alpha->one_over_kc;
  guint8 kfgy_scale = alpha->kfgy_scale;
  guint noise_level2 = alpha->noise_level2;
  gint matrix[12];
  gint o[3];
  gint bpp;

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  bpp = GST_VIDEO_FRAME_COMP_PSTRIDE (in_frame, 0);

  o[0] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 0);
  o[1] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 1);
  o[2] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 2);

  smin = 128 - alpha->black_sensitivity;
  smax = 128 + alpha->white_sensitivity;

  memcpy (matrix,
      alpha->out_sdtv ? cog_rgb_to_ycbcr_matrix_8bit_sdtv :
      cog_rgb_to_ycbcr_matrix_8bit_hdtv, 12 * sizeof (gint));

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      a = pa;
      r = src[o[0]];
      g = src[o[1]];
      b = src[o[2]];

      y = APPLY_MATRIX (matrix, 0, r, g, b);
      u = APPLY_MATRIX (matrix, 1, r, g, b) - 128;
      v = APPLY_MATRIX (matrix, 2, r, g, b) - 128;

      a = chroma_keying_yuv (a, &y, &u, &v, cr, cb,
          smin, smax, accept_angle_tg, accept_angle_ctg,
          one_over_kc, kfgy_scale, kg, noise_level2);

      u += 128;
      v += 128;

      dest[0] = a;
      dest[1] = y;
      dest[2] = u;
      dest[3] = v;

      src += bpp;
      dest += 4;
    }
  }
}

static void
gst_alpha_set_rgb_argb (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint s_alpha = CLAMP ((gint) (alpha->alpha * 255), 0, 255);
  gint i, j;
  gint p[4], o[3];
  gint bpp;

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  bpp = GST_VIDEO_FRAME_COMP_PSTRIDE (in_frame, 0);

  o[0] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 0);
  o[1] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 1);
  o[2] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 2);

  p[0] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 3);
  p[1] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 0);
  p[2] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 1);
  p[3] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 2);

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      dest[p[0]] = s_alpha;

      dest[p[1]] = src[o[0]];
      dest[p[2]] = src[o[1]];
      dest[p[3]] = src[o[2]];

      dest += 4;
      src += bpp;
    }
  }
}

static void
gst_alpha_chroma_key_rgb_argb (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint i, j;
  gint a, y, u, v;
  gint r, g, b;
  gint smin, smax;
  gint pa = CLAMP ((gint) (alpha->alpha * 255), 0, 255);
  gint8 cb = alpha->cb, cr = alpha->cr;
  gint8 kg = alpha->kg;
  guint8 accept_angle_tg = alpha->accept_angle_tg;
  guint8 accept_angle_ctg = alpha->accept_angle_ctg;
  guint8 one_over_kc = alpha->one_over_kc;
  guint8 kfgy_scale = alpha->kfgy_scale;
  guint noise_level2 = alpha->noise_level2;
  gint matrix[12], matrix2[12];
  gint p[4], o[3];
  gint bpp;

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  bpp = GST_VIDEO_FRAME_COMP_PSTRIDE (in_frame, 0);

  o[0] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 0);
  o[1] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 1);
  o[2] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 2);

  p[0] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 3);
  p[1] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 0);
  p[2] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 1);
  p[3] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 2);

  smin = 128 - alpha->black_sensitivity;
  smax = 128 + alpha->white_sensitivity;

  memcpy (matrix, cog_rgb_to_ycbcr_matrix_8bit_sdtv, 12 * sizeof (gint));
  memcpy (matrix2, cog_ycbcr_to_rgb_matrix_8bit_sdtv, 12 * sizeof (gint));

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      a = pa;
      r = src[o[0]];
      g = src[o[1]];
      b = src[o[2]];

      y = APPLY_MATRIX (matrix, 0, r, g, b);
      u = APPLY_MATRIX (matrix, 1, r, g, b) - 128;
      v = APPLY_MATRIX (matrix, 2, r, g, b) - 128;

      a = chroma_keying_yuv (a, &y, &u, &v, cr, cb,
          smin, smax, accept_angle_tg, accept_angle_ctg,
          one_over_kc, kfgy_scale, kg, noise_level2);

      u += 128;
      v += 128;

      r = APPLY_MATRIX (matrix2, 0, y, u, v);
      g = APPLY_MATRIX (matrix2, 1, y, u, v);
      b = APPLY_MATRIX (matrix2, 2, y, u, v);

      dest[p[0]] = a;
      dest[p[1]] = CLAMP (r, 0, 255);
      dest[p[2]] = CLAMP (g, 0, 255);
      dest[p[3]] = CLAMP (b, 0, 255);

      src += bpp;
      dest += 4;
    }
  }
}

static void
gst_alpha_set_planar_yuv_ayuv (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  guint8 *dest;
  gint width, height;
  gint b_alpha = CLAMP ((gint) (alpha->alpha * 255), 0, 255);
  const guint8 *srcY, *srcY_tmp;
  const guint8 *srcU, *srcU_tmp;
  const guint8 *srcV, *srcV_tmp;
  gint i, j;
  gint y_stride, uv_stride;
  gint v_subs, h_subs;

  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  y_stride = GST_VIDEO_FRAME_COMP_STRIDE (in_frame, 0);
  uv_stride = GST_VIDEO_FRAME_COMP_STRIDE (in_frame, 1);

  srcY_tmp = srcY = GST_VIDEO_FRAME_COMP_DATA (in_frame, 0);
  srcU_tmp = srcU = GST_VIDEO_FRAME_COMP_DATA (in_frame, 1);
  srcV_tmp = srcV = GST_VIDEO_FRAME_COMP_DATA (in_frame, 2);

  switch (GST_VIDEO_FRAME_FORMAT (in_frame)) {
    case GST_VIDEO_FORMAT_I420:
    case GST_VIDEO_FORMAT_YV12:
      v_subs = h_subs = 2;
      break;
    case GST_VIDEO_FORMAT_Y444:
      v_subs = h_subs = 1;
      break;
    case GST_VIDEO_FORMAT_Y42B:
      v_subs = 1;
      h_subs = 2;
      break;
    case GST_VIDEO_FORMAT_Y41B:
      v_subs = 1;
      h_subs = 4;
      break;
    default:
      g_assert_not_reached ();
      return;
  }

  if (alpha->in_sdtv == alpha->out_sdtv) {
    for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {
        dest[0] = b_alpha;
        dest[1] = srcY[0];
        dest[2] = srcU[0];
        dest[3] = srcV[0];

        dest += 4;
        srcY++;
        if ((j + 1) % h_subs == 0) {
          srcU++;
          srcV++;
        }
      }

      srcY_tmp = srcY = srcY_tmp + y_stride;
      if ((i + 1) % v_subs == 0) {
        srcU_tmp = srcU = srcU_tmp + uv_stride;
        srcV_tmp = srcV = srcV_tmp + uv_stride;
      } else {
        srcU = srcU_tmp;
        srcV = srcV_tmp;
      }
    }
  } else {
    gint matrix[12];
    gint a, y, u, v;

    memcpy (matrix,
        alpha->out_sdtv ? cog_ycbcr_hdtv_to_ycbcr_sdtv_matrix_8bit :
        cog_ycbcr_sdtv_to_ycbcr_hdtv_matrix_8bit, 12 * sizeof (gint));

    for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {
        a = b_alpha;
        y = srcY[0];
        u = srcU[0];
        v = srcV[0];

        dest[0] = a;
        dest[1] = APPLY_MATRIX (matrix, 0, y, u, v);
        dest[2] = APPLY_MATRIX (matrix, 1, y, u, v);
        dest[3] = APPLY_MATRIX (matrix, 2, y, u, v);

        dest += 4;
        srcY++;
        if ((j + 1) % h_subs == 0) {
          srcU++;
          srcV++;
        }
      }

      srcY_tmp = srcY = srcY_tmp + y_stride;
      if ((i + 1) % v_subs == 0) {
        srcU_tmp = srcU = srcU_tmp + uv_stride;
        srcV_tmp = srcV = srcV_tmp + uv_stride;
      } else {
        srcU = srcU_tmp;
        srcV = srcV_tmp;
      }
    }
  }
}

static void
gst_alpha_chroma_key_planar_yuv_ayuv (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  guint8 *dest;
  gint width, height;
  gint b_alpha = CLAMP ((gint) (alpha->alpha * 255), 0, 255);
  const guint8 *srcY, *srcY_tmp;
  const guint8 *srcU, *srcU_tmp;
  const guint8 *srcV, *srcV_tmp;
  gint i, j;
  gint a, y, u, v;
  gint y_stride, uv_stride;
  gint v_subs, h_subs;
  gint smin = 128 - alpha->black_sensitivity;
  gint smax = 128 + alpha->white_sensitivity;
  gint8 cb = alpha->cb, cr = alpha->cr;
  gint8 kg = alpha->kg;
  guint8 accept_angle_tg = alpha->accept_angle_tg;
  guint8 accept_angle_ctg = alpha->accept_angle_ctg;
  guint8 one_over_kc = alpha->one_over_kc;
  guint8 kfgy_scale = alpha->kfgy_scale;
  guint noise_level2 = alpha->noise_level2;

  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  y_stride = GST_VIDEO_FRAME_COMP_STRIDE (in_frame, 0);
  uv_stride = GST_VIDEO_FRAME_COMP_STRIDE (in_frame, 1);

  srcY_tmp = srcY = GST_VIDEO_FRAME_COMP_DATA (in_frame, 0);
  srcU_tmp = srcU = GST_VIDEO_FRAME_COMP_DATA (in_frame, 1);
  srcV_tmp = srcV = GST_VIDEO_FRAME_COMP_DATA (in_frame, 2);

  switch (GST_VIDEO_FRAME_FORMAT (in_frame)) {
    case GST_VIDEO_FORMAT_I420:
    case GST_VIDEO_FORMAT_YV12:
      v_subs = h_subs = 2;
      break;
    case GST_VIDEO_FORMAT_Y444:
      v_subs = h_subs = 1;
      break;
    case GST_VIDEO_FORMAT_Y42B:
      v_subs = 1;
      h_subs = 2;
      break;
    case GST_VIDEO_FORMAT_Y41B:
      v_subs = 1;
      h_subs = 4;
      break;
    default:
      g_assert_not_reached ();
      return;
  }

  if (alpha->in_sdtv == alpha->out_sdtv) {
    for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {
        a = b_alpha;
        y = srcY[0];
        u = srcU[0] - 128;
        v = srcV[0] - 128;

        a = chroma_keying_yuv (a, &y, &u, &v, cr, cb, smin,
            smax, accept_angle_tg, accept_angle_ctg,
            one_over_kc, kfgy_scale, kg, noise_level2);

        u += 128;
        v += 128;

        dest[0] = a;
        dest[1] = y;
        dest[2] = u;
        dest[3] = v;

        dest += 4;
        srcY++;
        if ((j + 1) % h_subs == 0) {
          srcU++;
          srcV++;
        }
      }

      srcY_tmp = srcY = srcY_tmp + y_stride;
      if ((i + 1) % v_subs == 0) {
        srcU_tmp = srcU = srcU_tmp + uv_stride;
        srcV_tmp = srcV = srcV_tmp + uv_stride;
      } else {
        srcU = srcU_tmp;
        srcV = srcV_tmp;
      }
    }
  } else {
    gint matrix[12];

    memcpy (matrix,
        alpha->out_sdtv ? cog_ycbcr_hdtv_to_ycbcr_sdtv_matrix_8bit :
        cog_ycbcr_sdtv_to_ycbcr_hdtv_matrix_8bit, 12 * sizeof (gint));

    for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {
        a = b_alpha;
        y = APPLY_MATRIX (matrix, 0, srcY[0], srcU[0], srcV[0]);
        u = APPLY_MATRIX (matrix, 1, srcY[0], srcU[0], srcV[0]) - 128;
        v = APPLY_MATRIX (matrix, 2, srcY[0], srcU[0], srcV[0]) - 128;

        a = chroma_keying_yuv (a, &y, &u, &v, cr, cb, smin,
            smax, accept_angle_tg, accept_angle_ctg,
            one_over_kc, kfgy_scale, kg, noise_level2);

        dest[0] = a;
        dest[1] = y;
        dest[2] = u + 128;
        dest[3] = v + 128;

        dest += 4;
        srcY++;
        if ((j + 1) % h_subs == 0) {
          srcU++;
          srcV++;
        }
      }

      srcY_tmp = srcY = srcY_tmp + y_stride;
      if ((i + 1) % v_subs == 0) {
        srcU_tmp = srcU = srcU_tmp + uv_stride;
        srcV_tmp = srcV = srcV_tmp + uv_stride;
      } else {
        srcU = srcU_tmp;
        srcV = srcV_tmp;
      }
    }
  }
}

static void
gst_alpha_set_planar_yuv_argb (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  guint8 *dest;
  gint width, height;
  gint b_alpha = CLAMP ((gint) (alpha->alpha * 255), 0, 255);
  const guint8 *srcY, *srcY_tmp;
  const guint8 *srcU, *srcU_tmp;
  const guint8 *srcV, *srcV_tmp;
  gint i, j;
  gint y_stride, uv_stride;
  gint v_subs, h_subs;
  gint matrix[12];
  gint a, y, u, v;
  gint r, g, b;
  gint p[4];

  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  p[0] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 3);
  p[1] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 0);
  p[2] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 1);
  p[3] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 2);

  y_stride = GST_VIDEO_FRAME_COMP_STRIDE (in_frame, 0);
  uv_stride = GST_VIDEO_FRAME_COMP_STRIDE (in_frame, 1);

  srcY_tmp = srcY = GST_VIDEO_FRAME_COMP_DATA (in_frame, 0);
  srcU_tmp = srcU = GST_VIDEO_FRAME_COMP_DATA (in_frame, 1);
  srcV_tmp = srcV = GST_VIDEO_FRAME_COMP_DATA (in_frame, 2);

  switch (GST_VIDEO_FRAME_FORMAT (in_frame)) {
    case GST_VIDEO_FORMAT_I420:
    case GST_VIDEO_FORMAT_YV12:
      v_subs = h_subs = 2;
      break;
    case GST_VIDEO_FORMAT_Y444:
      v_subs = h_subs = 1;
      break;
    case GST_VIDEO_FORMAT_Y42B:
      v_subs = 1;
      h_subs = 2;
      break;
    case GST_VIDEO_FORMAT_Y41B:
      v_subs = 1;
      h_subs = 4;
      break;
    default:
      g_assert_not_reached ();
      return;
  }

  memcpy (matrix,
      alpha->in_sdtv ? cog_ycbcr_to_rgb_matrix_8bit_sdtv :
      cog_ycbcr_to_rgb_matrix_8bit_hdtv, 12 * sizeof (gint));

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      a = b_alpha;
      y = srcY[0];
      u = srcU[0];
      v = srcV[0];

      dest[p[0]] = a;
      r = APPLY_MATRIX (matrix, 0, y, u, v);
      g = APPLY_MATRIX (matrix, 1, y, u, v);
      b = APPLY_MATRIX (matrix, 2, y, u, v);
      dest[p[1]] = CLAMP (r, 0, 255);
      dest[p[2]] = CLAMP (g, 0, 255);
      dest[p[3]] = CLAMP (b, 0, 255);

      dest += 4;
      srcY++;
      if ((j + 1) % h_subs == 0) {
        srcU++;
        srcV++;
      }
    }

    srcY_tmp = srcY = srcY_tmp + y_stride;
    if ((i + 1) % v_subs == 0) {
      srcU_tmp = srcU = srcU_tmp + uv_stride;
      srcV_tmp = srcV = srcV_tmp + uv_stride;
    } else {
      srcU = srcU_tmp;
      srcV = srcV_tmp;
    }
  }
}

static void
gst_alpha_chroma_key_planar_yuv_argb (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  guint8 *dest;
  gint width, height;
  gint b_alpha = CLAMP ((gint) (alpha->alpha * 255), 0, 255);
  const guint8 *srcY, *srcY_tmp;
  const guint8 *srcU, *srcU_tmp;
  const guint8 *srcV, *srcV_tmp;
  gint i, j;
  gint a, y, u, v;
  gint r, g, b;
  gint y_stride, uv_stride;
  gint v_subs, h_subs;
  gint smin = 128 - alpha->black_sensitivity;
  gint smax = 128 + alpha->white_sensitivity;
  gint8 cb = alpha->cb, cr = alpha->cr;
  gint8 kg = alpha->kg;
  guint8 accept_angle_tg = alpha->accept_angle_tg;
  guint8 accept_angle_ctg = alpha->accept_angle_ctg;
  guint8 one_over_kc = alpha->one_over_kc;
  guint8 kfgy_scale = alpha->kfgy_scale;
  guint noise_level2 = alpha->noise_level2;
  gint matrix[12];
  gint p[4];

  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  p[0] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 3);
  p[1] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 0);
  p[2] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 1);
  p[3] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 2);

  y_stride = GST_VIDEO_FRAME_COMP_STRIDE (in_frame, 0);
  uv_stride = GST_VIDEO_FRAME_COMP_STRIDE (in_frame, 1);

  srcY_tmp = srcY = GST_VIDEO_FRAME_COMP_DATA (in_frame, 0);
  srcU_tmp = srcU = GST_VIDEO_FRAME_COMP_DATA (in_frame, 1);
  srcV_tmp = srcV = GST_VIDEO_FRAME_COMP_DATA (in_frame, 2);

  switch (GST_VIDEO_FRAME_FORMAT (in_frame)) {
    case GST_VIDEO_FORMAT_I420:
    case GST_VIDEO_FORMAT_YV12:
      v_subs = h_subs = 2;
      break;
    case GST_VIDEO_FORMAT_Y444:
      v_subs = h_subs = 1;
      break;
    case GST_VIDEO_FORMAT_Y42B:
      v_subs = 1;
      h_subs = 2;
      break;
    case GST_VIDEO_FORMAT_Y41B:
      v_subs = 1;
      h_subs = 4;
      break;
    default:
      g_assert_not_reached ();
      return;
  }

  memcpy (matrix,
      alpha->in_sdtv ? cog_ycbcr_to_rgb_matrix_8bit_sdtv :
      cog_ycbcr_to_rgb_matrix_8bit_hdtv, 12 * sizeof (gint));

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      a = b_alpha;
      y = srcY[0];
      u = srcU[0] - 128;
      v = srcV[0] - 128;

      a = chroma_keying_yuv (a, &y, &u, &v, cr, cb, smin,
          smax, accept_angle_tg, accept_angle_ctg,
          one_over_kc, kfgy_scale, kg, noise_level2);

      u += 128;
      v += 128;

      dest[p[0]] = a;
      r = APPLY_MATRIX (matrix, 0, y, u, v);
      g = APPLY_MATRIX (matrix, 1, y, u, v);
      b = APPLY_MATRIX (matrix, 2, y, u, v);
      dest[p[1]] = CLAMP (r, 0, 255);
      dest[p[2]] = CLAMP (g, 0, 255);
      dest[p[3]] = CLAMP (b, 0, 255);

      dest += 4;
      srcY++;
      if ((j + 1) % h_subs == 0) {
        srcU++;
        srcV++;
      }
    }

    srcY_tmp = srcY = srcY_tmp + y_stride;
    if ((i + 1) % v_subs == 0) {
      srcU_tmp = srcU = srcU_tmp + uv_stride;
      srcV_tmp = srcV = srcV_tmp + uv_stride;
    } else {
      srcU = srcU_tmp;
      srcV = srcV_tmp;
    }
  }
}

static void
gst_alpha_set_packed_422_ayuv (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint s_alpha = CLAMP ((gint) (alpha->alpha * 255), 0, 255);
  gint i, j;
  gint y, u, v;
  gint p[4];                    /* Y U Y V */
  gint src_stride;
  const guint8 *src_tmp;

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  src_stride = GST_VIDEO_FRAME_COMP_STRIDE (in_frame, 0);

  p[0] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 0);
  p[2] = p[0] + 2;
  p[1] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 1);
  p[3] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 2);

  if (alpha->in_sdtv != alpha->out_sdtv) {
    gint matrix[12];

    memcpy (matrix,
        alpha->in_sdtv ? cog_ycbcr_sdtv_to_ycbcr_hdtv_matrix_8bit :
        cog_ycbcr_hdtv_to_ycbcr_sdtv_matrix_8bit, 12 * sizeof (gint));

    for (i = 0; i < height; i++) {
      src_tmp = src;

      for (j = 0; j < width - 1; j += 2) {
        dest[0] = s_alpha;
        dest[4] = s_alpha;

        y = APPLY_MATRIX (matrix, 0, src[p[0]], src[p[1]], src[p[3]]);
        u = APPLY_MATRIX (matrix, 1, src[p[0]], src[p[1]], src[p[3]]);
        v = APPLY_MATRIX (matrix, 2, src[p[0]], src[p[1]], src[p[3]]);

        dest[1] = y;
        dest[2] = u;
        dest[3] = v;

        y = APPLY_MATRIX (matrix, 0, src[p[2]], src[p[1]], src[p[3]]);
        u = APPLY_MATRIX (matrix, 1, src[p[2]], src[p[1]], src[p[3]]);
        v = APPLY_MATRIX (matrix, 2, src[p[2]], src[p[1]], src[p[3]]);

        dest[5] = y;
        dest[6] = u;
        dest[7] = v;

        dest += 8;
        src += 4;
      }

      if (j == width - 1) {
        dest[0] = s_alpha;

        y = APPLY_MATRIX (matrix, 0, src[p[0]], src[p[1]], src[p[3]]);
        u = APPLY_MATRIX (matrix, 1, src[p[0]], src[p[1]], src[p[3]]);
        v = APPLY_MATRIX (matrix, 2, src[p[0]], src[p[1]], src[p[3]]);

        dest[1] = y;
        dest[2] = u;
        dest[3] = v;

        dest += 4;
      }

      src = src_tmp + src_stride;
    }
  } else {
    for (i = 0; i < height; i++) {
      src_tmp = src;

      for (j = 0; j < width - 1; j += 2) {
        dest[0] = s_alpha;
        dest[4] = s_alpha;

        y = src[p[0]];
        u = src[p[1]];
        v = src[p[3]];

        dest[1] = y;
        dest[2] = u;
        dest[3] = v;

        y = src[p[2]];

        dest[5] = y;
        dest[6] = u;
        dest[7] = v;

        dest += 8;
        src += 4;
      }

      if (j == width - 1) {
        dest[0] = s_alpha;

        y = src[p[0]];
        u = src[p[1]];
        v = src[p[3]];

        dest[1] = y;
        dest[2] = u;
        dest[3] = v;

        dest += 4;
      }

      src = src_tmp + src_stride;
    }
  }
}

static void
gst_alpha_chroma_key_packed_422_ayuv (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint i, j;
  gint a, y, u, v;
  gint smin, smax;
  gint pa = CLAMP ((gint) (alpha->alpha * 255), 0, 255);
  gint8 cb = alpha->cb, cr = alpha->cr;
  gint8 kg = alpha->kg;
  guint8 accept_angle_tg = alpha->accept_angle_tg;
  guint8 accept_angle_ctg = alpha->accept_angle_ctg;
  guint8 one_over_kc = alpha->one_over_kc;
  guint8 kfgy_scale = alpha->kfgy_scale;
  guint noise_level2 = alpha->noise_level2;
  gint p[4];                    /* Y U Y V */
  gint src_stride;
  const guint8 *src_tmp;

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  src_stride = GST_VIDEO_FRAME_COMP_STRIDE (in_frame, 0);

  p[0] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 0);
  p[2] = p[0] + 2;
  p[1] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 1);
  p[3] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 2);

  smin = 128 - alpha->black_sensitivity;
  smax = 128 + alpha->white_sensitivity;

  if (alpha->in_sdtv != alpha->out_sdtv) {
    gint matrix[12];

    memcpy (matrix,
        alpha->in_sdtv ? cog_ycbcr_sdtv_to_ycbcr_hdtv_matrix_8bit :
        cog_ycbcr_hdtv_to_ycbcr_sdtv_matrix_8bit, 12 * sizeof (gint));

    for (i = 0; i < height; i++) {
      src_tmp = src;

      for (j = 0; j < width - 1; j += 2) {
        y = APPLY_MATRIX (matrix, 0, src[p[0]], src[p[1]], src[p[3]]);
        u = APPLY_MATRIX (matrix, 1, src[p[0]], src[p[1]], src[p[3]]) - 128;
        v = APPLY_MATRIX (matrix, 2, src[p[0]], src[p[1]], src[p[3]]) - 128;

        a = chroma_keying_yuv (pa, &y, &u, &v, cr, cb,
            smin, smax, accept_angle_tg, accept_angle_ctg,
            one_over_kc, kfgy_scale, kg, noise_level2);

        dest[0] = a;
        dest[1] = y;
        dest[2] = u + 128;
        dest[3] = v + 128;

        y = APPLY_MATRIX (matrix, 0, src[p[2]], src[p[1]], src[p[3]]);
        u = APPLY_MATRIX (matrix, 1, src[p[2]], src[p[1]], src[p[3]]) - 128;
        v = APPLY_MATRIX (matrix, 2, src[p[2]], src[p[1]], src[p[3]]) - 128;

        a = chroma_keying_yuv (pa, &y, &u, &v, cr, cb,
            smin, smax, accept_angle_tg, accept_angle_ctg,
            one_over_kc, kfgy_scale, kg, noise_level2);

        dest[4] = a;
        dest[5] = y;
        dest[6] = u + 128;
        dest[7] = v + 128;

        dest += 8;
        src += 4;
      }

      if (j == width - 1) {
        y = APPLY_MATRIX (matrix, 0, src[p[0]], src[p[1]], src[p[3]]);
        u = APPLY_MATRIX (matrix, 1, src[p[0]], src[p[1]], src[p[3]]) - 128;
        v = APPLY_MATRIX (matrix, 2, src[p[0]], src[p[1]], src[p[3]]) - 128;

        a = chroma_keying_yuv (pa, &y, &u, &v, cr, cb,
            smin, smax, accept_angle_tg, accept_angle_ctg,
            one_over_kc, kfgy_scale, kg, noise_level2);

        dest[0] = a;
        dest[1] = y;
        dest[2] = u + 128;
        dest[3] = v + 128;

        dest += 4;
      }

      src = src_tmp + src_stride;
    }
  } else {
    for (i = 0; i < height; i++) {
      src_tmp = src;

      for (j = 0; j < width - 1; j += 2) {
        y = src[p[0]];
        u = src[p[1]] - 128;
        v = src[p[3]] - 128;

        a = chroma_keying_yuv (pa, &y, &u, &v, cr, cb,
            smin, smax, accept_angle_tg, accept_angle_ctg,
            one_over_kc, kfgy_scale, kg, noise_level2);

        dest[0] = a;
        dest[1] = y;
        dest[2] = u + 128;
        dest[3] = v + 128;

        y = src[p[2]];
        u = src[p[1]] - 128;
        v = src[p[3]] - 128;

        a = chroma_keying_yuv (pa, &y, &u, &v, cr, cb,
            smin, smax, accept_angle_tg, accept_angle_ctg,
            one_over_kc, kfgy_scale, kg, noise_level2);

        dest[4] = a;
        dest[5] = y;
        dest[6] = u + 128;
        dest[7] = v + 128;

        dest += 8;
        src += 4;
      }

      if (j == width - 1) {
        y = src[p[0]];
        u = src[p[1]] - 128;
        v = src[p[3]] - 128;

        a = chroma_keying_yuv (pa, &y, &u, &v, cr, cb,
            smin, smax, accept_angle_tg, accept_angle_ctg,
            one_over_kc, kfgy_scale, kg, noise_level2);

        dest[0] = a;
        dest[1] = y;
        dest[2] = u + 128;
        dest[3] = v + 128;

        dest += 4;
      }

      src = src_tmp + src_stride;
    }
  }
}

static void
gst_alpha_set_packed_422_argb (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint s_alpha = CLAMP ((gint) (alpha->alpha * 255), 0, 255);
  gint i, j;
  gint p[4], o[4];
  gint src_stride;
  const guint8 *src_tmp;
  gint matrix[12];
  gint r, g, b;

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  src_stride = GST_VIDEO_FRAME_COMP_STRIDE (in_frame, 0);

  o[0] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 0);
  o[2] = o[0] + 2;
  o[1] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 1);
  o[3] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 2);

  p[0] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 3);
  p[1] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 0);
  p[2] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 1);
  p[3] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 2);

  memcpy (matrix,
      alpha->in_sdtv ? cog_ycbcr_to_rgb_matrix_8bit_sdtv :
      cog_ycbcr_to_rgb_matrix_8bit_hdtv, 12 * sizeof (gint));

  for (i = 0; i < height; i++) {
    src_tmp = src;

    for (j = 0; j < width - 1; j += 2) {
      r = APPLY_MATRIX (matrix, 0, src[o[0]], src[o[1]], src[o[3]]);
      g = APPLY_MATRIX (matrix, 1, src[o[0]], src[o[1]], src[o[3]]);
      b = APPLY_MATRIX (matrix, 2, src[o[0]], src[o[1]], src[o[3]]);

      dest[p[0]] = s_alpha;
      dest[p[1]] = CLAMP (r, 0, 255);
      dest[p[2]] = CLAMP (g, 0, 255);
      dest[p[3]] = CLAMP (b, 0, 255);

      r = APPLY_MATRIX (matrix, 0, src[o[2]], src[o[1]], src[o[3]]);
      g = APPLY_MATRIX (matrix, 1, src[o[2]], src[o[1]], src[o[3]]);
      b = APPLY_MATRIX (matrix, 2, src[o[2]], src[o[1]], src[o[3]]);

      dest[4 + p[0]] = s_alpha;
      dest[4 + p[1]] = CLAMP (r, 0, 255);
      dest[4 + p[2]] = CLAMP (g, 0, 255);
      dest[4 + p[3]] = CLAMP (b, 0, 255);

      dest += 8;
      src += 4;
    }

    if (j == width - 1) {
      r = APPLY_MATRIX (matrix, 0, src[o[0]], src[o[1]], src[o[3]]);
      g = APPLY_MATRIX (matrix, 1, src[o[0]], src[o[1]], src[o[3]]);
      b = APPLY_MATRIX (matrix, 2, src[o[0]], src[o[1]], src[o[3]]);

      dest[p[0]] = s_alpha;
      dest[p[1]] = CLAMP (r, 0, 255);
      dest[p[2]] = CLAMP (g, 0, 255);
      dest[p[3]] = CLAMP (b, 0, 255);

      dest += 4;
    }

    src = src_tmp + src_stride;
  }
}

static void
gst_alpha_chroma_key_packed_422_argb (const GstVideoFrame * in_frame,
    GstVideoFrame * out_frame, GstAlpha * alpha)
{
  const guint8 *src;
  guint8 *dest;
  gint width, height;
  gint i, j;
  gint a, y, u, v;
  gint r, g, b;
  gint smin, smax;
  gint pa = CLAMP ((gint) (alpha->alpha * 255), 0, 255);
  gint8 cb = alpha->cb, cr = alpha->cr;
  gint8 kg = alpha->kg;
  guint8 accept_angle_tg = alpha->accept_angle_tg;
  guint8 accept_angle_ctg = alpha->accept_angle_ctg;
  guint8 one_over_kc = alpha->one_over_kc;
  guint8 kfgy_scale = alpha->kfgy_scale;
  guint noise_level2 = alpha->noise_level2;
  gint p[4], o[4];
  gint src_stride;
  const guint8 *src_tmp;
  gint matrix[12];

  src = GST_VIDEO_FRAME_PLANE_DATA (in_frame, 0);
  dest = GST_VIDEO_FRAME_PLANE_DATA (out_frame, 0);

  width = GST_VIDEO_FRAME_WIDTH (in_frame);
  height = GST_VIDEO_FRAME_HEIGHT (in_frame);

  src_stride = GST_VIDEO_FRAME_COMP_STRIDE (in_frame, 0);

  o[0] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 0);
  o[2] = o[0] + 2;
  o[1] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 1);
  o[3] = GST_VIDEO_FRAME_COMP_POFFSET (in_frame, 2);

  p[0] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 3);
  p[1] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 0);
  p[2] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 1);
  p[3] = GST_VIDEO_FRAME_COMP_POFFSET (out_frame, 2);

  memcpy (matrix,
      alpha->in_sdtv ? cog_ycbcr_to_rgb_matrix_8bit_sdtv :
      cog_ycbcr_to_rgb_matrix_8bit_hdtv, 12 * sizeof (gint));

  smin = 128 - alpha->black_sensitivity;
  smax = 128 + alpha->white_sensitivity;

  for (i = 0; i < height; i++) {
    src_tmp = src;

    for (j = 0; j < width - 1; j += 2) {
      y = src[o[0]];
      u = src[o[1]] - 128;
      v = src[o[3]] - 128;

      a = chroma_keying_yuv (pa, &y, &u, &v, cr, cb,
          smin, smax, accept_angle_tg, accept_angle_ctg,
          one_over_kc, kfgy_scale, kg, noise_level2);
      u += 128;
      v += 128;

      r = APPLY_MATRIX (matrix, 0, y, u, v);
      g = APPLY_MATRIX (matrix, 1, y, u, v);
      b = APPLY_MATRIX (matrix, 2, y, u, v);

      dest[p[0]] = a;
      dest[p[1]] = CLAMP (r, 0, 255);
      dest[p[2]] = CLAMP (g, 0, 255);
      dest[p[3]] = CLAMP (b, 0, 255);

      y = src[o[2]];
      u = src[o[1]] - 128;
      v = src[o[3]] - 128;

      a = chroma_keying_yuv (pa, &y, &u, &v, cr, cb,
          smin, smax, accept_angle_tg, accept_angle_ctg,
          one_over_kc, kfgy_scale, kg, noise_level2);
      u += 128;
      v += 128;

      r = APPLY_MATRIX (matrix, 0, y, u, v);
      g = APPLY_MATRIX (matrix, 1, y, u, v);
      b = APPLY_MATRIX (matrix, 2, y, u, v);

      dest[4 + p[0]] = a;
      dest[4 + p[1]] = CLAMP (r, 0, 255);
      dest[4 + p[2]] = CLAMP (g, 0, 255);
      dest[4 + p[3]] = CLAMP (b, 0, 255);

      dest += 8;
      src += 4;
    }

    if (j == width - 1) {
      y = src[o[0]];
      u = src[o[1]] - 128;
      v = src[o[3]] - 128;

      a = chroma_keying_yuv (pa, &y, &u, &v, cr, cb,
          smin, smax, accept_angle_tg, accept_angle_ctg,
          one_over_kc, kfgy_scale, kg, noise_level2);
      u += 128;
      v += 128;

      r = APPLY_MATRIX (matrix, 0, y, u, v);
      g = APPLY_MATRIX (matrix, 1, y, u, v);
      b = APPLY_MATRIX (matrix, 2, y, u, v);

      dest[p[0]] = a;
      dest[p[1]] = CLAMP (r, 0, 255);
      dest[p[2]] = CLAMP (g, 0, 255);
      dest[p[3]] = CLAMP (b, 0, 255);

      dest += 4;
    }

    src = src_tmp + src_stride;
  }
}

/* Protected with the alpha lock */
static void
gst_alpha_init_params_full (GstAlpha * alpha,
    const GstVideoFormatInfo * in_info, const GstVideoFormatInfo * out_info)
{
  gfloat kgl;
  gfloat tmp;
  gfloat tmp1, tmp2;
  gfloat y;
  guint target_r = alpha->target_r;
  guint target_g = alpha->target_g;
  guint target_b = alpha->target_b;
  const gint *matrix;

  switch (alpha->method) {
    case ALPHA_METHOD_GREEN:
      target_r = 0;
      target_g = 255;
      target_b = 0;
      break;
    case ALPHA_METHOD_BLUE:
      target_r = 0;
      target_g = 0;
      target_b = 255;
      break;
    default:
      break;
  }

  /* RGB->RGB: convert to SDTV YUV, chroma keying, convert back
   * YUV->RGB: chroma keying, convert to RGB
   * RGB->YUV: convert to YUV, chroma keying
   * YUV->YUV: convert matrix, chroma keying
   */
  if (GST_VIDEO_FORMAT_INFO_IS_RGB (in_info)
      && GST_VIDEO_FORMAT_INFO_IS_RGB (out_info))
    matrix = cog_rgb_to_ycbcr_matrix_8bit_sdtv;
  else if (GST_VIDEO_FORMAT_INFO_IS_YUV (in_info)
      && GST_VIDEO_FORMAT_INFO_IS_RGB (out_info))
    matrix =
        (alpha->in_sdtv) ? cog_rgb_to_ycbcr_matrix_8bit_sdtv :
        cog_rgb_to_ycbcr_matrix_8bit_hdtv;
  else if (GST_VIDEO_FORMAT_INFO_IS_RGB (in_info)
      && GST_VIDEO_FORMAT_INFO_IS_YUV (out_info))
    matrix =
        (alpha->out_sdtv) ? cog_rgb_to_ycbcr_matrix_8bit_sdtv :
        cog_rgb_to_ycbcr_matrix_8bit_hdtv;
  else                          /* yuv -> yuv */
    matrix =
        (alpha->out_sdtv) ? cog_rgb_to_ycbcr_matrix_8bit_sdtv :
        cog_rgb_to_ycbcr_matrix_8bit_hdtv;

  y = (matrix[0] * ((gint) target_r) +
      matrix[1] * ((gint) target_g) +
      matrix[2] * ((gint) target_b) + matrix[3]) >> 8;
  /* Cb,Cr without offset here because the chroma keying
   * works with them being in range [-128,127]
   */
  tmp1 =
      (matrix[4] * ((gint) target_r) +
      matrix[5] * ((gint) target_g) + matrix[6] * ((gint) target_b)) >> 8;
  tmp2 =
      (matrix[8] * ((gint) target_r) +
      matrix[9] * ((gint) target_g) + matrix[10] * ((gint) target_b)) >> 8;

  kgl = sqrt (tmp1 * tmp1 + tmp2 * tmp2);
  alpha->cb = 127 * (tmp1 / kgl);
  alpha->cr = 127 * (tmp2 / kgl);

  tmp = 15 * tan (M_PI * alpha->angle / 180);
  tmp = MIN (tmp, 255);
  alpha->accept_angle_tg = tmp;
  tmp = 15 / tan (M_PI * alpha->angle / 180);
  tmp = MIN (tmp, 255);
  alpha->accept_angle_ctg = tmp;
  tmp = 1 / (kgl);
  alpha->one_over_kc = 255 * 2 * tmp - 255;
  tmp = 15 * y / kgl;
  tmp = MIN (tmp, 255);
  alpha->kfgy_scale = tmp;
  alpha->kg = MIN (kgl, 127);

  alpha->noise_level2 = alpha->noise_level * alpha->noise_level;
}

static void
gst_alpha_init_params (GstAlpha * alpha)
{
  const GstVideoFormatInfo *finfo_in, *finfo_out;

  finfo_in = GST_VIDEO_FILTER (alpha)->in_info.finfo;
  finfo_out = GST_VIDEO_FILTER (alpha)->out_info.finfo;

  if (finfo_in != NULL && finfo_out != NULL) {
    gst_alpha_init_params_full (alpha, finfo_in, finfo_out);
  } else {
    GST_DEBUG_OBJECT (alpha, "video formats not set yet");
  }
}

/* Protected with the alpha lock */
static gboolean
gst_alpha_set_process_function_full (GstAlpha * alpha, GstVideoInfo * in_info,
    GstVideoInfo * out_info)
{
  alpha->process = NULL;

  switch (alpha->method) {
    case ALPHA_METHOD_SET:
      switch (GST_VIDEO_INFO_FORMAT (out_info)) {
        case GST_VIDEO_FORMAT_AYUV:
          switch (GST_VIDEO_INFO_FORMAT (in_info)) {
            case GST_VIDEO_FORMAT_AYUV:
              alpha->process = gst_alpha_set_ayuv_ayuv;
              break;
            case GST_VIDEO_FORMAT_Y444:
            case GST_VIDEO_FORMAT_Y42B:
            case GST_VIDEO_FORMAT_I420:
            case GST_VIDEO_FORMAT_YV12:
            case GST_VIDEO_FORMAT_Y41B:
              alpha->process = gst_alpha_set_planar_yuv_ayuv;
              break;
            case GST_VIDEO_FORMAT_YUY2:
            case GST_VIDEO_FORMAT_YVYU:
            case GST_VIDEO_FORMAT_UYVY:
              alpha->process = gst_alpha_set_packed_422_ayuv;
              break;
            case GST_VIDEO_FORMAT_ARGB:
            case GST_VIDEO_FORMAT_ABGR:
            case GST_VIDEO_FORMAT_RGBA:
            case GST_VIDEO_FORMAT_BGRA:
              alpha->process = gst_alpha_set_argb_ayuv;
              break;
            case GST_VIDEO_FORMAT_xRGB:
            case GST_VIDEO_FORMAT_xBGR:
            case GST_VIDEO_FORMAT_RGBx:
            case GST_VIDEO_FORMAT_BGRx:
            case GST_VIDEO_FORMAT_RGB:
            case GST_VIDEO_FORMAT_BGR:
              alpha->process = gst_alpha_set_rgb_ayuv;
              break;
            default:
              break;
          }
          break;
        case GST_VIDEO_FORMAT_ARGB:
        case GST_VIDEO_FORMAT_ABGR:
        case GST_VIDEO_FORMAT_RGBA:
        case GST_VIDEO_FORMAT_BGRA:
          switch (GST_VIDEO_INFO_FORMAT (in_info)) {
            case GST_VIDEO_FORMAT_AYUV:
              alpha->process = gst_alpha_set_ayuv_argb;
              break;
            case GST_VIDEO_FORMAT_Y444:
            case GST_VIDEO_FORMAT_Y42B:
            case GST_VIDEO_FORMAT_I420:
            case GST_VIDEO_FORMAT_YV12:
            case GST_VIDEO_FORMAT_Y41B:
              alpha->process = gst_alpha_set_planar_yuv_argb;
              break;
            case GST_VIDEO_FORMAT_YUY2:
            case GST_VIDEO_FORMAT_YVYU:
            case GST_VIDEO_FORMAT_UYVY:
              alpha->process = gst_alpha_set_packed_422_argb;
              break;
            case GST_VIDEO_FORMAT_ARGB:
            case GST_VIDEO_FORMAT_ABGR:
            case GST_VIDEO_FORMAT_RGBA:
            case GST_VIDEO_FORMAT_BGRA:
              alpha->process = gst_alpha_set_argb_argb;
              break;
            case GST_VIDEO_FORMAT_xRGB:
            case GST_VIDEO_FORMAT_xBGR:
            case GST_VIDEO_FORMAT_RGBx:
            case GST_VIDEO_FORMAT_BGRx:
            case GST_VIDEO_FORMAT_RGB:
            case GST_VIDEO_FORMAT_BGR:
              alpha->process = gst_alpha_set_rgb_argb;
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
      break;
    case ALPHA_METHOD_GREEN:
    case ALPHA_METHOD_BLUE:
    case ALPHA_METHOD_CUSTOM:
      switch (GST_VIDEO_INFO_FORMAT (out_info)) {
        case GST_VIDEO_FORMAT_AYUV:
          switch (GST_VIDEO_INFO_FORMAT (in_info)) {
            case GST_VIDEO_FORMAT_AYUV:
              alpha->process = gst_alpha_chroma_key_ayuv_ayuv;
              break;
            case GST_VIDEO_FORMAT_Y444:
            case GST_VIDEO_FORMAT_Y42B:
            case GST_VIDEO_FORMAT_I420:
            case GST_VIDEO_FORMAT_YV12:
            case GST_VIDEO_FORMAT_Y41B:
              alpha->process = gst_alpha_chroma_key_planar_yuv_ayuv;
              break;
            case GST_VIDEO_FORMAT_YUY2:
            case GST_VIDEO_FORMAT_YVYU:
            case GST_VIDEO_FORMAT_UYVY:
              alpha->process = gst_alpha_chroma_key_packed_422_ayuv;
              break;
            case GST_VIDEO_FORMAT_ARGB:
            case GST_VIDEO_FORMAT_ABGR:
            case GST_VIDEO_FORMAT_RGBA:
            case GST_VIDEO_FORMAT_BGRA:
              alpha->process = gst_alpha_chroma_key_argb_ayuv;
              break;
            case GST_VIDEO_FORMAT_xRGB:
            case GST_VIDEO_FORMAT_xBGR:
            case GST_VIDEO_FORMAT_RGBx:
            case GST_VIDEO_FORMAT_BGRx:
            case GST_VIDEO_FORMAT_RGB:
            case GST_VIDEO_FORMAT_BGR:
              alpha->process = gst_alpha_chroma_key_rgb_ayuv;
              break;
            default:
              break;
          }
          break;
        case GST_VIDEO_FORMAT_ARGB:
        case GST_VIDEO_FORMAT_ABGR:
        case GST_VIDEO_FORMAT_RGBA:
        case GST_VIDEO_FORMAT_BGRA:
          switch (GST_VIDEO_INFO_FORMAT (in_info)) {
            case GST_VIDEO_FORMAT_AYUV:
              alpha->process = gst_alpha_chroma_key_ayuv_argb;
              break;
            case GST_VIDEO_FORMAT_Y444:
            case GST_VIDEO_FORMAT_Y42B:
            case GST_VIDEO_FORMAT_I420:
            case GST_VIDEO_FORMAT_YV12:
            case GST_VIDEO_FORMAT_Y41B:
              alpha->process = gst_alpha_chroma_key_planar_yuv_argb;
              break;
            case GST_VIDEO_FORMAT_YUY2:
            case GST_VIDEO_FORMAT_YVYU:
            case GST_VIDEO_FORMAT_UYVY:
              alpha->process = gst_alpha_chroma_key_packed_422_argb;
              break;
            case GST_VIDEO_FORMAT_ARGB:
            case GST_VIDEO_FORMAT_ABGR:
            case GST_VIDEO_FORMAT_RGBA:
            case GST_VIDEO_FORMAT_BGRA:
              alpha->process = gst_alpha_chroma_key_argb_argb;
              break;
            case GST_VIDEO_FORMAT_xRGB:
            case GST_VIDEO_FORMAT_xBGR:
            case GST_VIDEO_FORMAT_RGBx:
            case GST_VIDEO_FORMAT_BGRx:
            case GST_VIDEO_FORMAT_RGB:
            case GST_VIDEO_FORMAT_BGR:
              alpha->process = gst_alpha_chroma_key_rgb_argb;
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  return alpha->process != NULL;
}

static void
gst_alpha_set_process_function (GstAlpha * alpha)
{
  GstVideoInfo *info_in, *info_out;

  info_in = &GST_VIDEO_FILTER (alpha)->in_info;
  info_out = &GST_VIDEO_FILTER (alpha)->out_info;

  if (info_in->finfo != NULL && info_out->finfo != NULL) {
    gst_alpha_set_process_function_full (alpha, info_in, info_out);
  } else {
    GST_DEBUG_OBJECT (alpha, "video formats not set yet");
  }
}

static void
gst_alpha_before_transform (GstBaseTransform * btrans, GstBuffer * buf)
{
  GstAlpha *alpha = GST_ALPHA (btrans);
  GstClockTime timestamp;

  timestamp = gst_segment_to_stream_time (&btrans->segment, GST_FORMAT_TIME,
      GST_BUFFER_TIMESTAMP (buf));
  GST_LOG ("Got stream time of %" GST_TIME_FORMAT, GST_TIME_ARGS (timestamp));
  if (GST_CLOCK_TIME_IS_VALID (timestamp))
    gst_object_sync_values (GST_OBJECT (alpha), timestamp);
}

static GstFlowReturn
gst_alpha_transform_frame (GstVideoFilter * filter, GstVideoFrame * in_frame,
    GstVideoFrame * out_frame)
{
  GstAlpha *alpha = GST_ALPHA (filter);

  GST_ALPHA_LOCK (alpha);

  if (G_UNLIKELY (!alpha->process))
    goto not_negotiated;

  alpha->process (in_frame, out_frame, alpha);

  GST_ALPHA_UNLOCK (alpha);

  return GST_FLOW_OK;

  /* ERRORS */
not_negotiated:
  {
    GST_ERROR_OBJECT (alpha, "Not negotiated yet");
    GST_ALPHA_UNLOCK (alpha);
    return GST_FLOW_NOT_NEGOTIATED;
  }
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  return gst_element_register (plugin, "accel3d", GST_RANK_NONE, GST_TYPE_ALPHA);
}

GST_PLUGIN_DEFINE (MAJOR_VERSION, MINOR_VERSION, accel3d,
    "3d accellerometer data source",
    plugin_init, PACKAGE_VERSION, PACKAGE_LICENSE, PACKAGE_NAME, PACKAGE_ORIGIN)

