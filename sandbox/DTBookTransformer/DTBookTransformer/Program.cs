using System;
using System.Collections.Generic;
using System.Text;
using Saxon.Api;

namespace DTBookTransformer
{
    ///
    /// A sample XmlResolver. In the case of a URI ending with ".txt", it returns the
    /// URI itself, wrapped as an XML document. In the case of the URI "empty.xslt", it returns an empty
    /// stylesheet. In all other cases, it returns null, which has the effect of delegating
    /// processing to the standard XmlResolver.
    ///
    public class UserXmlResolver : XmlUrlResolver
    {

        public String Message = null;

        public override object GetEntity(Uri absoluteUri, String role, Type ofObjectToReturn)
        {
            if (Message != null)
            {
                Console.WriteLine(Message + absoluteUri + " (role=" + role + ")");
            }

            if (absoluteUri.ToString().EndsWith(".txt"))
            {
                MemoryStream ms = new MemoryStream();
                StreamWriter tw = new StreamWriter(ms);
                tw.Write("<uri>");
                tw.Write(absoluteUri);
                tw.Write("</uri>");
                tw.Flush();
                return new MemoryStream(ms.GetBuffer(), 0, (int)ms.Length);
            }
            if (absoluteUri.ToString().EndsWith("empty.xslt"))
            {
                String ss = "<transform xmlns='http://www.w3.org/1999/XSL/Transform' version='2.0'/>";
                MemoryStream ms = new MemoryStream();
                StreamWriter tw = new StreamWriter(ms);
                tw.Write(ss);
                tw.Flush();
                return new MemoryStream(ms.GetBuffer(), 0, (int)ms.Length);
            }
            else
            {
                return null;
            }
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            string infile = @"c:\daisybooks\verysimplebook\verysimplebook.xml";
            string infile_dir = @"c:\daisybooks\verysimplebook\";
            string xsltfile = @"c:\devel\amis\trunk\amis\bin\xslt\dtbook\dtbook2xhtml.xsl";
            string outfile = @"c:\devel\amis\sandbox\dtbooktransformer_out.xml";

            // Create a Processor instance.
            Processor processor = new Processor();

            // Load the source document
            XdmNode input = processor.NewDocumentBuilder().Build(new Uri(infile));

            // Create a transformer for the stylesheet.
            XsltTransformer transformer = 
                processor.NewXsltCompiler().Compile(new Uri(xsltfile)).Load();

            QName basedir = new QName("", "baseDir");

            List<XdmAtomicValue> elementNames = new List<XdmAtomicValue>();
            elementNames.Add(new XdmAtomicValue(infile_dir));
            XdmValue basedir_value = new XdmValue(elementNames); 

            transformer.SetParameter(basedir, basedir_value);

            // Set the user-written XmlResolver
            UserXmlResolver runTimeResolver = new UserXmlResolver();
            runTimeResolver.Message = "** Calling transformation-time XmlResolver: ";
            transformer.InputXmlResolver = runTimeResolver;

            // Set the root node of the source document to be the initial context node
            transformer.InitialContextNode = input;

            /*
             *  String outfile = "OutputFromXsltSimple2.xml";
        Serializer serializer = new Serializer();
        serializer.SetOutputStream(new FileStream(outfile, FileMode.Create, FileAccess.Write));
*/
              // Create a serializer, with output to the standard output stream
            Serializer serializer = new Serializer();
            serializer.SetOutputWriter(Console.Out);

            // Transform the source XML and serialize the result document
            transformer.Run(serializer);

            Console.ReadLine();
        }
    }
}
