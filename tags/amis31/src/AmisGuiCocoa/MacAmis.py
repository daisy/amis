"""TinyTinyEdit -- A minimal Document-based Cocoa application."""
import sys
print 'PATH', sys.path
import urllib
import amiscore
import os
import sys

from PyObjCTools import NibClassBuilder, AppHelper


NibClassBuilder.extractClasses("AmisGuiDocument")

# class defined in AmisGuiDocument.nib
class AmisGuiDocument(NibClassBuilder.AutoBaseClass):
    # the actual base class is NSDocument
    # The following outlets are added to the class:
    # textView

    path = None
    
    def init(self):
        self.dtb = amiscore.Dtb()
        self.bmkdir = os.path.join(os.getenv("HOME"), "bmk")
        self.curphrase = None

    def windowNibName(self):
        return "AmisGuiDocument"

    def readFromFile_ofType_(self, path, tp):
        print 'readFromFile', path, tp
        if self.textView is None:
            # we're not yet fully loaded
            self.path = path
        else:
            # "revert"
            self.readFromUTF8(path)
        return True
        
    def readFromData_ofType_error(self, data, tp):
        print 'readFromData', data, tp
        return False

    def windowControllerDidLoadNib_(self, controller):
        if self.path:
            self.readFromUTF8(self.path)

    def readFromUTF8(self, path):
        if not os.path.exists(self.bmkdir):
            os.mkdir(self.bmkdir)
        self.dtb.open(path, self.bmkdir)
        self.curphrase = self.dtb.startReading(False)
        self.update()
        
    def update(self):
        text_node = self.curphrase.getText()
        text_path = self.getPathFromNode(text_node)
        audio_node = self.curphrase.getAudio(0)
        audio_path = elf.getPathFromNode(audio_node)
        print 'TEXT', text_path
        print 'AUDIO', audio_path
        
    def getPathFromNode(self, node):
        if not node: return None
        src = node.getSrc()
        fileset = self.dtb.getFileSet()
        smil_file = fileset.getSmilFile()
        smil_dir = os.path.dirname(smil_file)
        return os.path.join(smil_dir, src)
        
if __name__ == "__main__":
    AppHelper.runEventLoop()
