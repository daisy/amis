# Usage: build-installer.py moduleDesc.xml helpfile-path installer-translation word-for-contrast
import argparse
import etree
import shutil
import yaml

AMIS_NS = "http://daisy.org/ns/amis"
NSMAP = {None: AMIS_NS}

SETTINGS_DEST_DIR = "bin/settings/clean_settings_for_installer"


class NsiBuilder():
    def __init__(self):
        self.lang_id = ""
        self.langname = ""
        self.helpfile_name = ""
        self.settings_dest_dir = ""
    
    # create the NSI script that then gets run to create the installer EXE
    def create_nsi_script(self):
    
    

class AmisSettings():
    def __init__(self, config):
        self.settings_source_dir = ""
        self.settings_dest_dir = ""
        self.jaws_scripts_installer = config['']
        self.contrast_translation = config['contrast-translation']
        self.amis_bin_dir = config['amis-bin-dir']
        self.lang_id = ""
    
    def copy_file(self, source, dest):
        print "Copying {0} ==> {1}".format(source, dest)
        shutil.copyfile(source, dest)
    
    def copy_settings_file(self, rel_source_path, rel_dest_path):
        self.copy_file(os.path.join(self.settings_source_dir, rel_source_path), \
            os.path.join(self.settings_dest_dir, rel_dest_path))
    
    # main function
    def set_up_settings(self):
        # delete and recreate the settings dest dir
        if (os.path.isdir(self.settings_dest_dir)):
            shutil.rmtree(self.settings_dest_dir)
        os.mkdir(self.settings_dest_dir)
        os.mkdir(os.path.join(self.settings_dest_dir), "customStyles")
        
        # copy amisHistory.xml.default to amisHistory.xml
        self.copy_settings_file("amisHistory.xml.default", "amisHistory.xml")
    
        self.copy_file(self.jaws_scripts_installer, os.path.join(self.amis_bin_dir, "amis3_jfw_script.exe"))
        
        contrast1 = "customStyles/{0}1.css".format(self.contrast_translation)
        contrast2 = "customStyles/{0}2.css".format(self.contrast_translation)
        contrast2 = "customStyles/{0}3.css".format(self.contrast_translation)
        self.copy_settings_file("css/customStyles/contrast1.css", contrast1)
        self.copy_settings_file("css/customStyles/contrast2.css", contrast2)
        self.copy_settings_file("css/customStyles/contrast3.css", contrast3)
        
        prefs = ["amisPrefs.xml", "amisPrefsCompatibilityMode.xml", "amisPrefsCompatibilityModeWithDX.xml", 
        "amisPrefsCompatibilityModeWithTTS.xml", "amisPrefsDebug.xml"]
        
        for p in prefs:
            self.copy_settings_file("{0}.default".format(p), p)
            prefsdoc = AmisPrefsDocument(os.path.join(self.settings_dest_dir, p))
            prefsdoc.set_property("ui-lang", self.lang_id)
            prefsdoc.store(os.path.join(self.settings_dest_dir, p))
            
        
class AmisPrefsDocument(object):
    
    def __init__(self, path):
        self._xmldoc = etree.parse(path)
        
    @property
    def xmldoc(self):
        return self._xmldoc
    
    # set value in <entry id="name" value="value"/>
    def set_property(self, name, value):
        res = self._xmldoc.xpath("//entry[@id='{0}']".format(name), namespaces = ns.NSMAP)
        if len(res) == 0:
            return
        res[0].attrib['value'] = value;
    
    def store(self, path):
        """utility function to write XML to disk"""
        xmlstr = etree.tostring(self._xmldoc, pretty_print = True, encoding="utf-8")
        f = open(filename, "w")
        f.write(xmlstr)
        f.close()
        
    
    
def main():
    # load configuration
    config_file = os.path.join(os.path.dirname(__file__), 'config.yaml')
    f = open(config_file)
    config = yaml.load(f.read())
    f.close()
    

def RunInstallerScript():
    "C:\Program Files\NSIS-Unicode\makensis" /DCUSTOM_LANG_ID=%1 /DCUSTOM_LANG_NAME=%2 /DCUSTOM_HELP=%3 ..\%1\setup-amis3.nsi  

if __name__ == '__main__':
    main()


