// Javascript driver for client-side transformation of the UI document
// Creates the form for the language/container pair specified.
// $Id: transform.js 1723 2006-10-13 04:18:38Z julienq $

// much taken from
// http://users.telenet.be/cking/webstuff/dynamic-xslt/dynamic.html

var src_url = "http://amis.sourceforge.net/l10ndocs/ui.xml"
var stylesheet_url = "http://amis.sourceforge.net/l10ndocs/form.xslt";

// "Detect" Mozilla or IE. Other browsers are not supported.
var isMoz = document.implementation && document.implementation.createDocument;
var isIE = window.ActiveXObject;

var src_doc;
var stylesheet;

var param_type;
var param_id;
var param_lang;
var param_url;

// This function is called from the body of the host HTML document with the
// values of the four parameters.
function transform(type, id, lang, url)
{
  var p = document.getElementById("msg");
  try {
    if (isMoz || isIE) {
      p.removeChild(p.firstChild);
      p.appendChild(document.createTextNode("Please wait..."));
      src_doc = create_document();
      stylesheet = create_document();
      param_type = type;
      param_id = id;
      param_lang = lang;
      param_url = url;
      if (isMoz) {
        src_doc.addEventListener("load", load_stylesheet, false);
        src_doc.load(src_url);
      } else {
        stylesheet.load(stylesheet_url);
        src_doc.load(src_url);
        do_transform();
      }
    } else {
      throw("unsupported");
    }
  } catch (e) {
    p.removeChild(p.firstChild);
    p.appendChild(document.createTextNode("Something went wrong :("));
  }
}

// Actually do the transformation: create the processor, pass the
// parameters, replace the contents of the host document with those of the
// result of the transformation (I don't know how to replace the document
// wholesale, so I keep the html root and change everything underneath. This
// does not change the title in Firefox though.)
function do_transform()
{
  if (isMoz) {
    processor = new XSLTProcessor();
    processor.importStylesheet(stylesheet);
    processor.setParameter(null, "type", param_type);
    processor.setParameter(null, "id", param_id);
    processor.setParameter(null, "lang", param_lang);
    processor.setParameter(null, "url", param_url);
    var fragment = processor.transformToFragment(src_doc, document);
    var root = document.documentElement;
    while (root.hasChildNodes()) {
      root.removeChild(root.firstChild);
    }
    while (fragment.firstChild.hasChildNodes()) {
      root.appendChild(fragment.firstChild.firstChild);
    }
  } else if (isIE) {
    // todo
  }
}

// Callback to load the stylesheet for Mozilla.
function load_stylesheet()
{
  if (isMoz) {
    stylesheet.addEventListener("load", do_transform, false);
    stylesheet.load(stylesheet_url);
  }
}

// Convenience function to create document for Mozilla and IE.
function create_document()
{
  var doc = null;
  if (isMoz) {
    doc = document.implementation.createDocument("", "", null);
  } else if (isIE) {
    doc = new ActiveXObject("Microsoft.XMLDOM");
    doc.async = false;
  }
  return doc;
}
