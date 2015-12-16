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
    # TODO: NSIS does not support unicode (although a forked version, Unicode NSIS, does; but we're not using that version at the moment.)
    # so, write the NSIS file as latin-1 encoding, with the caveat that we will probably have to switch NSIS versions if people start creating unicode installers.
    # at present, the text for all the existing installers (english, hungarian) can be represented with latin-1
    f.write(data.encode('utf-8'))
    f.close()

def main():
    # the languages for which we need to build custom installers
    langlist = ["eng-US", "hun-HU", "jpn-JP", "ger-DE"]
    
    for lang in langlist:
        print "Processing %s" % lang
        xmlfile = "../%s/installer-data.xml" % lang
        main, langpack = export_nsi(xmlfile)
        outdir = "../%s/" % lang
        write_file(main, outdir + "setup-amis3.nsi")
        write_file(langpack, outdir + "setup-amis3-langpack.nsi")

    
if __name__ == "__main__":
    main()
    
