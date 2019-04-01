/*
 * gdv-enums.h
 * This file is part of gdv
 *
 * Copyright (C) 2013-2014 - Emanuel Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GDV_ENUMS_H__
#define __GDV_ENUMS_H__

/**
 * GdvOnedLayerType:
 * @GDV_NORMAL_STRAIGHT: A normal straight one-dimensional layer
 * @GDV_DRUM_SCALE:      A graph-style were the scale range is updated continously to show the data
 *                       always in the scale-center
 * @GDV_BAR_GRAPH:       A graph-style were the data is indicated by a marker on a normal scale
 * @GDV_BOX_GRAPH:       A graph-style were the data is indicated by a box on a normal scale
 *
 * Represents the style to handle data-indicators and axis-range in a 1d-plot.
 */
typedef enum
{
  GDV_NORMAL_STRAIGHT,
  GDV_DRUM_SCALE,
  GDV_BAR_GRAPH,
  GDV_BOX_GRAPH
} GdvOnedLayerType;

/**
 * GdvTwodAxisType:
 * @GDV_X1_AXIS: The bottom x-axis
 * @GDV_X2_AXIS: The top-side x-axis
 * @GDV_Y1_AXIS: The left y-axis
 * @GDV_Y2_AXIS: The right y-axis
 *
 * Differs between the different axes, present in an normal two-dimensional cartesian plot.
 */
typedef enum
{
  GDV_X1_AXIS,
  GDV_X2_AXIS,
  GDV_Y1_AXIS,
  GDV_Y2_AXIS
} GdvTwodAxisType;

/**
 * GdvDistributionType:
 * @GDV_UPPER_DIST: An upper focused distribution
 * @GDV_SYMMETRIC_DIST: An symmetric distribution
 * @GDV_LOWER_DIST: An lower focused distribution
 *
 * Necessary to clarify a distribution.
 */
typedef enum
{
  GDV_UPPER_DIST,
  GDV_SYMMETRIC_DIST,
  GDV_LOWER_DIST,
} GdvDistributionType;

#endif /* __GDV_ENUMS_H__ */
