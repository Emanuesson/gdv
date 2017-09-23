#!/usr/bin/env python

import sys

import gi
gi.require_version('Gtk', '3.0')
gi.require_version('Gdv', '1.0')

from gi.repository import GObject, GLib, Gio, Gtk, Gdv

class MyWindow(Gtk.Window):

	def __init__(self):
		  Gtk.Window.__init__(self, title="Hello World")

		  self.verbox = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=6)
		  self.add(self.verbox)

		  self.layerframe = Gtk.Frame()
		  self.layer = Gdv.OnedLayer()
		  self.layer.set_property ("height-request", 50)
		  self.layer.set_property ("width-request", 20)
		  self.layer.connect("button_press_event", self.on_layer_clicked)
		  self.layerframe.add(self.layer)
		  self.verbox.add(self.layerframe)

		  self.indicator1 = Gdv.Indicator()
		  self.axis = self.layer.get_property ("axis")
		  self.axis.connect("button_press_event", self.on_axis_clicked)
		  Gtk.Container.add (self.axis, self.indicator1)

		  self.box = Gtk.Box(spacing=6)
		  self.verbox.add(self.box)

		  self.button1 = Gtk.Button(label="Hello")
		  self.button1.connect("clicked", self.on_button1_clicked)
		  self.box.pack_start(self.button1, True, True, 0)

		  self.button2 = Gtk.Button(label="Goodbye")
		  self.button2.connect("clicked", self.on_button2_clicked)
		  self.box.pack_start(self.button2, True, True, 0)

	def on_window_clicked(self, widget, data):
		  print("Window clicked")

	def on_layer_clicked(self, widget, data):
		  print("Layer clicked")

	def on_axis_clicked(self, widget, data):
		  print("Axis clicked")

	def on_button1_clicked(self, widget):
		  print("Hello")

	def on_button2_clicked(self, widget):
		  print("Goodbye")

win = MyWindow()

win.connect("button_press_event", win.on_window_clicked)

win.connect("delete-event", Gtk.main_quit)
win.show_all()
Gtk.main()




