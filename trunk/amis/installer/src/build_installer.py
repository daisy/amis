# Builds AMIS setup and AMIS langpack setup executables for the language
# incorporates translation if available

import argparse
from lxml import etree
import shutil
import yaml
import os
from nsi_from_template import NsiFromTemplate

AMIS_NS = "http://daisy.org/ns/amis"
NSMAP = {"a": AMIS_NS} # arbitrary prefix; see http://lxml.de/FAQ.html#how-can-i-specify-a-default-namespace-for-xpath-expressions

class InstallScript:
    def __init__(self, config, lang):
        self.config = config
        self.lang = lang
        self.data_dir = self.config["Global"]["data_dir"]
        self.settings_source_dir = self.config["Global"]["settings_source"]
        self.setup_nsi_filepath = ""
        self.setup_langpack_nsi_filepath = ""

    def create_install_script(self):
        self.create_dirs()
        self.copy_files()
        self.generate_nsi()

    def create_dirs(self):
        # make sure the working dir is empty
        settings_dir = os.path.join(self.data_dir, "settings")
        custom_styles_dir = os.path.join(settings_dir, "customStyles")
        nsi_dir = os.path.join(self.data_dir, "nsi")

        if (os.path.isdir(self.data_dir)):
            shutil.rmtree(self.data_dir)
        os.mkdir(self.data_dir)
        os.mkdir(settings_dir)
        os.mkdir(custom_styles_dir)
        os.mkdir(nsi_dir)

        # make an output dir for the lang
        outdir = os.path.join(self.config["Global"]["data_dir"], "nsi/{0}".format(self.lang["langid"]))
        os.mkdir(outdir)

    @staticmethod
    def copy_file(source, dest):
        print "*** Copying {0} ==> {1}".format(source, dest)
        shutil.copyfile(source, dest)

    def copy_files(self):
        history_source = os.path.join(self.settings_source_dir, "amisHistory.xml.default")
        history_dest = os.path.join(self.data_dir, "amisHistory.xml")
        InstallScript.copy_file(history_source, history_dest)

        contrast_translation = self.lang["contrast_translation"]
        for x in (1, 2, 3):
            contrast_source = os.path.join(self.settings_source_dir, "css/customStyles/contrast{0}.css".format(x))
            contrast_dest = os.path.join(self.data_dir, "settings/customStyles/{0}{1}.css".format(contrast_translation, x))
            InstallScript.copy_file(contrast_source, contrast_dest)

        prefs = ["amisPrefs.xml", "amisPrefsCompatibilityMode.xml", "amisPrefsCompatibilityModeWithDX.xml",
        "amisPrefsCompatibilityModeWithTTS.xml", "amisPrefsDebug.xml"]

        for p in prefs:
            prefs_source = os.path.join(self.settings_source_dir, "{0}.default".format(p))
            prefs_dest = os.path.join(self.data_dir, "settings/{0}".format(p))
            InstallScript.copy_file(prefs_source, prefs_dest)
            prefsdoc = AmisPrefsDocument(prefs_dest)
            prefsdoc.set_property("ui-lang-id", self.lang["langid"])
            prefsdoc.store(prefs_dest)

        # copy resource files
        files_to_copy = ["filebrowse.ini", "getversions.nsh"]
        curr_path = os.path.dirname(os.path.abspath(__file__))
        for f in files_to_copy:
            file_source = os.path.join(curr_path, "./resources/" + f)
            file_dest = os.path.join(self.config["Global"]["data_dir"], "nsi/" + f)
            InstallScript.copy_file(file_source, file_dest)

        # copy lgpl translation
        lgpl_source = os.path.join(self.config["Global"]["translations"], "{0}/{1}".format(self.lang["langid"], "lgpl.txt"))
        lgpl_dest = os.path.join(self.config["Global"]["data_dir"], "nsi/{0}/lgpl.txt".format(self.lang["langid"]))
        InstallScript.copy_file(lgpl_source, lgpl_dest)

    def generate_nsi(self):
        nsi = NsiFromTemplate(self.config, self.lang)
        self.setup_nsi_filepath, self.setup_langpack_nsi_filepath = nsi.create()

class AmisPrefsDocument:
    def __init__(self, path):
        self.xmldoc = etree.parse(path)

    # set value in <entry id="name" value="value"/>
    def set_property(self, name, value):
        xpath_expr = "//a:entry[@id='{0}']".format(name)
        res = self.xmldoc.xpath(xpath_expr, namespaces = NSMAP)
        if len(res) == 0:
            return
        res[0].attrib['value'] = value

    def store(self, path):
        """utility function to write XML to disk"""
        xmlstr = etree.tostring(self.xmldoc, pretty_print = True, encoding="utf-8")
        f = open(path, "w")
        f.write(xmlstr)
        f.close()

# create a batch file listing all the NSIS scripts that were created
# this way, I can run this in OSX to generate the NSIS files, and then actually generate executables in Windows
class BatchHelper:
    def __init__(self, config):
        self.config = config
        self.batchfile = "REM Create AMIS setup and langpack setup executables"
        self.batchfile += "\n"
        self.filepath = os.path.join(self.config["Global"]["data_dir"], "nsi/create-setup-exes.bat")

    def add_nsi(self, install_script):
        rel_setup_nsi_filepath = os.path.relpath(install_script.setup_nsi_filepath,
            os.path.dirname(self.filepath))
        rel_setup_nsi_langpack_filepath = os.path.relpath(install_script.setup_langpack_nsi_filepath,
            os.path.dirname(self.filepath))
        self.batchfile += "\nECHO {0}".format(install_script.lang["langid"])
        self.batchfile += "\n{0}".format(self.make_cmd_string(rel_setup_nsi_filepath))
        self.batchfile += "\n{0}".format(self.make_cmd_string(rel_setup_nsi_langpack_filepath))
        self.batchfile += "\n\n"

    def save(self):
        f = open(self.filepath, "w")
        f.write(self.batchfile)
        f.close()
        return self.filepath

    def make_cmd_string(self, nsi_path):
        cmd_string = "\"{0}\" \"{1}\"".format(self.config["Global"]["makensis"], nsi_path)
        return cmd_string

# resolve selected paths in the config
def resolve_paths(config):
    curr_path = os.path.dirname(os.path.abspath(__file__))
    config["Global"]["settings_source"] = os.path.normpath(os.path.join(curr_path, config["Global"]["settings_source"]))
    config["Global"]["data_dir"] = os.path.normpath(os.path.join(curr_path, config["Global"]["data_dir"]))
    config["Global"]["translations"] = os.path.normpath(os.path.join(curr_path, config["Global"]["translations"]))
    config["Installer"]["bin_dir"] = os.path.normpath(os.path.join(curr_path, config["Installer"]["bin_dir"]))
    config["Installer"]["logo_dir"] = os.path.normpath(os.path.join(curr_path, config["Installer"]["logo_dir"]))
    config["Installer"]["jaws_scripts"] = os.path.normpath(os.path.join(curr_path, config["Installer"]["jaws_scripts"]))

    return config

def main():
    print "Make sure you compiled the cheetah templates first!!"
    # load configuration
    config_file = os.path.join(os.path.dirname(__file__), 'config.yaml')
    f = open(config_file)
    config = yaml.load(f.read())
    f.close()

    config = resolve_paths(config)

    parser = argparse.ArgumentParser(description="Build AMIS setup files")
    parser.add_argument('--langid', action='store', help='Specify a langid; otherwise, builds all.')
    parser.add_argument('--nsis', dest="nsis", action="store", default=True, help="If True (default): runs makensis to actually create the executables.")
    args = parser.parse_args()

    batch = BatchHelper(config)

    if args.langid:
        config["Global"]["build_lang"] = args.langid

    # create and run all the installer scripts
    if config["Global"]["build_all"] == True:
        print "Building all\n"
        for lang in config["Langs"]:
            script = InstallScript(config, lang)
            script.create_install_script()
            batch.add_nsi(script)
    else:
        print "Building {0}\n".format(config["Global"]["build_lang"])
        found_lang = False
        for lang in config["Langs"]:
            if lang["langid"] == config["Global"]["build_lang"]:
                found_lang = True
                script = InstallScript(config, lang)
                script.create_install_script()
                batch.add_nsi(script)
        if found_lang == False:
            print "Language {0} not found.".format(config["Global"]["build_lang"])

    batch_filepath = batch.save()
    if args.nsis == True:
        print "Running {0} to invoke makensis".format(batch_filepath)
        os.popen(batch_filepath)
    else:
        print "Run {0} to invoke makensis".format(batch_filepath)

if __name__ == '__main__':
    main()


