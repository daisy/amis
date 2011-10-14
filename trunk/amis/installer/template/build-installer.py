from optparse import OptionParser
from lxml import etree
import SetupAmis3NsiTemplate
import SetupAmis3LangpackNsiTemplate

def generate_terms_list(xmlfile):
    terms = {}
    tree = etree.parse(xmlfile)
    terms["include_jaws_scripts"] = tree.xpath('//options/@include_jaws_scripts')[0]
    terms["nsis_language"] = tree.xpath('//options/@nsis_language')[0]
    
    strings = tree.xpath("//string")
    
    for s in strings:
        terms[s.attrib['id']] = s.text
    return terms
    

def export_nsi(xmlfile):
    
    terms = generate_terms_list(xmlfile)
    
    main_template = SetupAmis3NsiTemplate.SetupAmis3NsiTemplate(searchList = terms)
    langpack_template = SetupAmis3LangpackNsiTemplate.SetupAmis3LangpackNsiTemplate(searchList = terms)

    return main_template.respond(), langpack_template.respond()

def write_file(data, filename):
    f = open(filename, "w")
    f.write(data.encode('utf-8'))
    f.close()

def main():
    parser = OptionParser("build-installer.py XMLFile OutputDir")
    (options, args) = parser.parse_args()
    if len(args) < 2:
        print "Required: path to XML file and output directory"
        sys.exit(1)
    
    main, langpack = export_nsi(args[0])
    
    outdir = args[1]
    if not outdir.endswith("/"):
        outdir += "/"
        
    write_file(main, outdir + "setup-amis3.nsi")
    write_file(langpack, outdir + "setup-amis3-langpack.nsi")

    
if __name__ == "__main__":
    main()
    
