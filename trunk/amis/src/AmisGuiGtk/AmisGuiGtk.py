#!/usr/bin/env/python
"""AMIS gui test

A simple gui for the first linux version of AMIS"""

import gtk
import gobject
import gtkmozembed
import os
import amiscore

class AmisGuiGtk(gtk.Window):
	
	def __init__(self, parent=None):
		gtk.Window.__init__(self)
		self.moz = gtkmozembed.MozEmbed()
		self.dtb = amiscore.Dtb()

		try:
			self.set_screen(parent.get_screen())
		except AttributeError:
			self.connect('destroy', lambda *w: gtk.main_quit())

		self.set_title("AMIS")

		vbox = gtk.VBox(False, 0)
		self.add(vbox)

		filemenulist = gtk.Menu()
		open = gtk.MenuItem("open")
		quit = gtk.MenuItem("quit")
		filemenulist.append(open)
		filemenulist.append(quit)
		file = gtk.MenuItem("file")
		file.set_submenu(filemenulist)
		menubar = gtk.MenuBar()
		menubar.append(file)

		vbox.pack_start(menubar, expand=False)

		hpaned = gtk.HPaned()
		model = gtk.TreeStore(gobject.TYPE_PYOBJECT, gobject.TYPE_STRING)
		treeitem= { "hello": "hi"}
		iter = model.insert_before(None, None)
		model.set_value(iter, 0, treeitem)

		treeview = gtk.TreeView(model)
		hpaned.add1(treeview)
		hpaned.add2(self.moz)

		vbox.pack_end(hpaned)
	
		self.show_all()
		open_book(self, None)

def play_audio(filepath, clipbegin, clipend):
	cmd = "mplayer %s >/dev/null 2>&1 1</dev/null"%filepath
	os.system(cmd)

def open_book(self, filepath):
	self.dtb.open("/media/winxp/dtb/Mountains_skip/ncc.html", "/home/med/bmk")
	media = self.dtb.startReading(False)
	text_node = media.getText()
	audio_node = media.getAudio(0)

	text_path = "file://" + getPathFromNode(self, text_node)
	audio_path = getPathFromNode(self, audio_node)
	
	showText(self, text_path)
	play_audio(audio_path, audio_node.getClipBegin(), None)

def getPathFromNode(self, node):
	if not node: return None
	src = node.getSrc()
	fileset = self.dtb.getFileSet()
	smil_file = fileset.getSmilFile()
	smil_dir = os.path.dirname(smil_file)
	return os.path.join(smil_dir, src)


def showText(self, url):
	self.moz.load_url(url)

	

def main():
	AmisGuiGtk()
	gtk.main()

if __name__ == '__main__':
	main()

