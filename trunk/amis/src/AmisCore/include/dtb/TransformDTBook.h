#ifndef TRANSFORMDTBOOK_H
#define TRANSFORMDTBOOK_H


#include <string>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
using namespace std;

XERCES_CPP_NAMESPACE_USE

namespace amis
{
namespace dtb
{
	class TransformDTBook
	{
	public:
		TransformDTBook(){};
		~TransformDTBook(){};
		bool transform(string);
		string getResults();
	private:
		void process(xercesc_3_0::DOMDocument*);
		string mResults;
	};
}
}
#endif
