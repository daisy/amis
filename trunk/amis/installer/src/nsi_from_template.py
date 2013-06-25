"""Generate the setup and langpack setup NSI files
Use translated strings where available"""
from lxml import etree
import SetupAmis3NsiTemplate
import SetupAmis3LangpackNsiTemplate
import os

class NsiFromTemplate:
    def __init__(self, config, lang):
        self.config = config
        self.lang = lang

    def generate_terms_list(self, xmlfile):
        terms = {}
        tree = etree.parse(xmlfile)
        terms["include_jaws_scripts"] = tree.xpath('//options/@include_jaws_scripts')[0]
        terms["nsis_language"] = tree.xpath('//options/@nsis_language')[0]

        strings = tree.xpath("//string")
        for s in strings:
            terms[s.attrib['id']] = s.text

        # incorporate terms from the config file
        terms = dict(terms.items() + self.config["Installer"].items())

        # make certain paths relative to the location of the NSI output file
        outdir = os.path.join(self.config["Global"]["data_dir"], "nsi/{0}".format(self.lang["langid"]))
        terms["bin_dir"] = os.path.relpath(terms["bin_dir"], outdir)
        terms["logo_dir"] = os.path.relpath(terms["logo_dir"], outdir)
        terms["jaws_scripts"] = os.path.relpath(terms["jaws_scripts"], outdir)
        terms["build_dir"] = os.path.relpath(terms["build_dir"], outdir)
        print terms["build_dir"]
        # template convenience
        terms["jaws_scripts_filename"] = os.path.basename(terms["jaws_scripts"])
        rel_data_dir = os.path.relpath(self.config["Global"]["data_dir"], outdir)
        terms["data_dir"] = rel_data_dir


        # add lang terms
        terms = dict(terms.items() + self.lang.items())
        return terms

    # combine the translated strings with the template
    def export_nsi(self, xmlfile):
        terms = self.generate_terms_list(xmlfile)
        main_template = SetupAmis3NsiTemplate.SetupAmis3NsiTemplate(searchList = terms)
        langpack_template = SetupAmis3LangpackNsiTemplate.SetupAmis3LangpackNsiTemplate(searchList = terms)
        return main_template.respond(), langpack_template.respond()

    # save the file to disk
    @staticmethod
    def write_file(data, filename):
        f = open(filename, "w")
        f.write(data.encode('utf-8'))
        f.close()

    # main entry point: create the installer files and return their paths
    def create(self):
        langid = self.lang["langid"]
        print "\nFilling in template for {0}".format(langid)
        #curr_path = os.path.dirname(os.path.abspath(__file__))
        if langid not in self.config["Global"]["installer_translations"]:
            langid = "eng-US"
        xmlfile = "{0}/{1}/installer-data.xml".format(self.config["Global"]["translations"], langid)
        outdir = os.path.join(self.config["Global"]["data_dir"], "nsi/{0}".format(langid))

        main, langpack = self.export_nsi(xmlfile)

        setup_filename = os.path.join(outdir, "setup-amis3.nsi")
        setup_langpack_filename = os.path.join(outdir, "setup-amis3-langpack.nsi")
        NsiFromTemplate.write_file(main, setup_filename)
        NsiFromTemplate.write_file(langpack, setup_langpack_filename)
        print "Created {0} and {1}".format(setup_filename, setup_langpack_filename)
        return setup_filename, setup_langpack_filename
