#include "ambulant/version.h"
#include "ambulant/common/plugin_engine.h"
#include "ambulant/lib/logger.h"
#include "ambulant/lib/textptr.h"
#include "ambulant/net/datasource.h"

#define AM_DBG
#ifndef AM_DBG
#define AM_DBG if(0)
#endif

using namespace ambulant;

net::datasource_factory *g_df = NULL;
bool apply_fixes(std::string& whole_file);

class dtbook_filter_finder : public net::raw_filter_finder 
{
  public:
    net::datasource* new_raw_filter(const net::url& url, net::datasource *src);	
};

class dtbook_datasource : public net::filter_datasource_impl 
{
  public:
    dtbook_datasource(net::datasource *src);
	size_t _process(char *data, size_t size);
	static bool is_dtbook(const net::url& url);
};

extern "C"
#ifdef AMBULANT_PLATFORM_WIN32
__declspec(dllexport)
#endif
struct ambulant::common::plugin_extra_data plugin_extra_data = 
{
	"dtbook_plugin",
	NULL
};


net::datasource *
dtbook_filter_finder::new_raw_filter(const net::url& url, net::datasource *src)
{
	//check if this file is a dtbook file
	if (!dtbook_datasource::is_dtbook(url)) return src;

	// Finally test that we haven't already pushed a dtbook datasource,
	// and if we haven't create one.
    if (dynamic_cast<dtbook_datasource*>(src) == NULL) {
//        AM_DBG lib::logger::get_logger()->debug("dtbook_filter_finder: success for %s (0x%x), basetype is %s", url.get_url().c_str(), src, typeid(src).name());
        return new dtbook_datasource(src);
    }
    return src;
}

dtbook_datasource::dtbook_datasource(net::datasource *src)
:   net::filter_datasource_impl(src)
{
}

size_t
dtbook_datasource::_process(char *data, size_t size)
{
	char *optr = m_databuf.get_write_ptr(size);
	std::string whole_file = data;
	size = apply_fixes(whole_file);
	m_databuf.pushdata(size);
	return size;
}

bool 
dtbook_datasource::is_dtbook(const net::url& url)
{
	//TODO: check if this is actually a dtbook file (they're not all called "dtbook")
    std::string filename = url.get_url();
    // Get the directory part
	size_t slashpos = filename.find_last_of("/");
	if (slashpos <= 0)
		return false;
	filename = filename.substr(slashpos);
	
	if (filename == "dtbook.xml") return true;
	else return false;
}

static ambulant::common::factories * 
bug_workaround(ambulant::common::factories* factory)
{
	return factory;
}

extern "C"
#ifdef AMBULANT_PLATFORM_WIN32
__declspec(dllexport)
#endif
void initialize(
    int api_version,
    ambulant::common::factories* factory,
    ambulant::common::gui_player *player)
{
    if ( api_version != AMBULANT_PLUGIN_API_VERSION ) {
        lib::logger::get_logger()->warn("dtbook_plugin: built for plugin-api version %d, current %d. Skipping.", 
            AMBULANT_PLUGIN_API_VERSION, api_version);
        return;
    }
    if ( !ambulant::check_version() )
        lib::logger::get_logger()->warn("dtbook_plugin: built for different Ambulant version (%s)", AMBULANT_VERSION);
	factory = bug_workaround(factory);
    AM_DBG lib::logger::get_logger()->debug("dtbook_plugin: loaded.");
    g_df = factory->get_datasource_factory();
    if (g_df) {
    	dtbook_filter_finder *ff = new dtbook_filter_finder();
    	g_df->add_raw_filter(ff);
    	AM_DBG lib::logger::get_logger()->trace("dtbook_plugin: registered");
    }
}

//Fix the DTBook file.  All the logic is in this function.
bool
apply_fixes(std::string& whole_file)
{
	std::string local_css = "file:///c:/devel/marisastuff/marisa/DTBookDTDCSSFixer/dtbookbasic.css";
    std::string local_dtd_folder = "file:///c:/devel/marisastuff/marisa/DTBookDTDCSSFixer/";
    std::string input_file = "file_examples/missing_css/greatpainters.xml";
    
    std::string style_declaration = "<?xml-stylesheet";
    std::string xml_declaration = "<?xml version='1.0' encoding='UTF-8'?>";
    std::string doctype3_declaration = "<!DOCTYPE dtbook PUBLIC '-//NISO//DTD dtbook 2005-3//EN' 'http://www.daisy.org/z3986/2005/";
    std::string doctype2_declaration = "<!DOCTYPE dtbook PUBLIC '-//NISO//DTD dtbook 2005-2//EN' 'http://www.daisy.org/z3986/2005/";
    
    std::string style_replacement = "<?xml version='1.0' encoding='UTF-8'?><?xml-stylesheet href=\"";
    style_replacement.append(local_css);
    style_replacement.append("\" type=\"text/css\"?>");
    
    std::string doctype3_replacement = "<!DOCTYPE dtbook PUBLIC '-//NISO//DTD dtbook 2005-3//EN' '" + local_dtd_folder;
    std::string doctype2_replacement = "<!DOCTYPE dtbook PUBLIC '-//NISO//DTD dtbook 2005-2//EN' '" + local_dtd_folder;
    
    size_t found;

    //insert our own CSS declaration.  use xml_declaration to put it in the right spot.
    found = whole_file.find(style_declaration);
    //if there is no stylesheet
	if (found == std::string::npos)
    {
        found = whole_file.find(xml_declaration);
		if (found != std::string::npos)
        {
            whole_file.replace(found, xml_declaration.length(), style_replacement);  
        }
        else
        {
			std::cerr<<"XML declaration not found"<<std::endl;
			return false;
        }
    }
    
    found = whole_file.find(doctype3_declaration);
    if (found != std::string::npos)
    {
        whole_file.replace(found, doctype3_declaration.length(), doctype3_replacement);
    }
    else
    {
        found = whole_file.find(doctype2_declaration);
        if (found != std::string::npos)   
        {
            whole_file.replace(found, doctype2_declaration.length(), doctype2_replacement);
        }
        else
        {
            std::cerr<<"Doctype declaration not found"<<std::endl;
			return false;
        }
    }
    
	return true;
}
