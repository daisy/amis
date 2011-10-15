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
    # the languages for which we need to build custom installers
    langlist = ["eng-US", "hun-HU"]
    
    for lang in langlist:
        xmlfile = "../%s/installer-data.xml" % lang
        main, langpack = export_nsi(xmlfile)
        outdir = "../%s/" % lang
        write_file(main, outdir + "setup-amis3.nsi")
        write_file(langpack, outdir + "setup-amis3-langpack.nsi")

    
if __name__ == "__main__":
    main()
    
