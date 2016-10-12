#pragma once

#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax/HandlerBase.hpp> 
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/sax/Locator.hpp>

using namespace xercesc;

class OperErrorHandler : public DefaultHandler 
{ 
public:   
    void error(const SAXParseException& exc)
    {
        Report(exc);
    }
    void fatalError(const SAXParseException& exc)
    {
        Report(exc);
    }
    void warning(const SAXParseException& exc)
    {
        Report(exc);
    }
    virtual void Report(const SAXParseException& exc) = 0;    
};

