#!/usr/bin/env python2	

'''
   This program illustrates how to embed gnuplot using a GTK socket.
   The threading will be cleaned up when time allows, therefore this
   program is just to be considered as a hack (a nice hack though).

   File:    plot.py
   Author:  Johan Astborg, 2014
   Repo:    https://github.com/joastbg/gtk-socket-gnuplot

'''

import sys
import datetime
import fcntl
import socket

from threading import Thread
from Queue import Queue
from time import sleep

import string
import pygtk
pygtk.require('2.0')

import gtk
#import wx
import os
import binascii
import subprocess
from subprocess import Popen, PIPE, STDOUT

q = Queue()

class plot(gtk.Window):
    
    def __init__(self):        
        gtk.Window.__init__(self)
        self.set_title("Embedded gnuplot")
        self.resize(800,600)

        vbox = gtk.VBox(False, 0)
        self.add(vbox)
        vbox.show()

        self.button = gtk.Button(label="Jack in gnuplot")
        self.handler_id = self.button.connect("clicked", self.on_button_clicked)
     
        self.label = gtk.Label('Hello World')
        self.socket = gtk.Socket()
        self.socket.show()
    
        self.textinput = gtk.Entry()
        self.textinput.show()

        vbox.pack_start(self.textinput, False, False, 5);
        vbox.pack_start(self.button, False, False, 5);
        vbox.pack_start(self.socket, True, True, 5);

        self.socket.connect("plug-added", self.plugged_event)
        print "Socket ID=", hex(int(self.socket.get_id()))

        #self.q = Queue()

    def plugged_event(self, widget):
        print ("A widget (most likley gnuplot) has just been jacked in!")

    def on_button_clicked2(self, widget):

        ''' Send command from text input to gnuplot '''

        print ("NEW COMMAND " + self.textinput.get_text())
        usercmd = self.textinput.set_text("")
        q.put(usercmd)
        print (q.empty())

    def on_button_clicked(self, widget):        

        ''' Change label of button (and functionality) '''

        self.button.set_label("Send command to gnuplot")
        self.button.disconnect(self.handler_id)
        self.button.connect("clicked", self.on_button_clicked2)

        ''' Start gnuplot '''

        print("Starting gnuplot...")

        id = hex(self.socket.get_id())

        cmd = "set terminal x11 window \"" + hex(int(self.socket.get_id())) + "\""
#        cmd = "set terminal x11 window \"0xc00007\""

        q.put(cmd)
        thread = Thread(target=self.thread_worker)
        thread.daemon = True
        thread.start()
        thread.join(timeout=1)

    def thread_worker(self):
        
        q.put("plot tan(x)")
        handler = ProcessHandler()

        # Never go any further...
    
class ProcessHandler():
    def __init__(self):
        self.process = subprocess.Popen(
            ['gnuplot', '-p'],
            shell=True,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE)

        thread = Thread(target=self.log_worker, args=[self.process.stdin])
        thread.daemon = True

        thread.start()

        self.process.wait()
        thread.join(timeout=1)

    def log_worker(self, stdin):
        while True:
            cmd = q.get()
            stdin.write(cmd + '\n')

    def non_block_read(self, output):

        fd = output.fileno()
        fl = fcntl.fcntl(fd, fcntl.F_GETFL)
        fcntl.fcntl(fd, fcntl.F_SETFL, fl | os.O_NONBLOCK)
        try:
            return output.read()
        except:
            return ''

if __name__ == '__main__':
    
    p = plot()
    p.connect("delete-event", gtk.main_quit)
    p.show_all()
    gtk.main()
