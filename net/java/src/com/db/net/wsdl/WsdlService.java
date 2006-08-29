/*
 * Copyright (c) 2006 Digital Bazaar, Inc.  All rights reserved.
 */
package com.db.net.wsdl;

import java.util.Iterator;
import java.util.Vector;

import com.db.logging.Logger;
import com.db.logging.LoggerManager;
import com.db.net.soap.WsdlSoapPort;
import com.db.xml.AbstractXmlSerializer;
import com.db.xml.XmlElement;

/**
 * A WSDL Service.
 * 
 * A WSDL Service describes the set of ports that web services are provided
 * over. 
 * 
 * @author Dave Longley
 */
public class WsdlService extends AbstractXmlSerializer
{
   /**
    * The WSDL this service is associated with.
    */
   protected Wsdl mWsdl;
   
   /**
    * The name of this service.
    */
   protected String mName;
   
   /**
    * The Wsdl Ports for this service.
    */
   protected WsdlPortCollection mPortCollection;
   
   /**
    * Creates a new blank WsdlService.
    * 
    * @param wsdl the wsdl this service is associated with.
    */
   public WsdlService(Wsdl wsdl)
   {
      this(wsdl, "");
   }   
   
   /**
    * Creates a new WsdlService with the given name.
    * 
    * @param wsdl the wsdl this service is associated with.
    * @param name the name of the service.
    */
   public WsdlService(Wsdl wsdl, String name)
   {
      // store wsdl
      mWsdl = wsdl;
      
      // set name
      setName(name);
   }
   
   /**
    * Gets the wsdl this service is associated with.
    * 
    * @return the wsdl this service is associated with.
    */
   public Wsdl getWsdl()
   {
      return mWsdl;
   }
   
   /**
    * Sets the name of this service.
    * 
    * @param name the name of this service.
    */
   public void setName(String name)
   {
      mName = name;
   }
   
   /**
    * Gets the name of this service.
    * 
    * @return the name of this service.
    */
   public String getName()
   {
      return mName;
   }
   
   /**
    * Gets the namespace URI for this service.
    * 
    * @return the namespace URI for this service.
    */
   public String getNamespaceUri()
   {
      return getWsdl().getTargetNamespaceUri();
   }
   
   /**
    * Gets the ports for this service.
    * 
    * @return the ports for this service.
    */
   public WsdlPortCollection getPorts()
   {
      if(mPortCollection == null)
      {
         // create the port collection for this service
         mPortCollection = new WsdlPortCollection();
      }
      
      return mPortCollection;
   }
   
   /**
    * Returns the root tag name for this serializer.
    * 
    * @return the root tag name for this serializer.
    */
   public String getRootTag()   
   {
      return "service";
   }
   
   /**
    * Creates an XmlElement from this object.
    *
    * @return the XmlElement that represents this object.
    */
   public XmlElement convertToXmlElement()
   {
      // create xml element
      XmlElement element = new XmlElement(getRootTag());

      // add attributes
      element.addAttribute("name", getName());
      
      // ports
      for(Iterator i = getPorts().iterator(); i.hasNext();)
      {
         WsdlPort port = (WsdlPort)i.next();
         element.addChild(port.convertToXmlElement());
      }
      
      // return element
      return element;      
   }
   
   /**
    * Converts this object from an XmlElement.
    *
    * @param element the XmlElement to convert from.
    * 
    * @return true if successful, false otherwise.
    */
   public boolean convertFromXmlElement(XmlElement element)   
   {
      boolean rval = false;
      
      // clear name
      setName("");
      
      // clear ports
      getPorts().clear();

      if(element.getName().equals(getRootTag()))
      {
         // get name
         setName(element.getAttributeValue("name"));

         // read ports
         for(Iterator i = element.getChildren("port").iterator(); i.hasNext();)
         {
            XmlElement child = (XmlElement)i.next();
            
            // get binding
            String bindingName = child.getAttributeValue("binding");
            WsdlBinding binding =
               getWsdl().getBindings().getBinding(bindingName);
            if(binding != null)
            {
               // FUTURE CODE: current implementation can only read
               // WsdlSoapPorts
               XmlElement soapElement = child.getFirstChild("soap:address");
               if(soapElement != null)
               {
                  WsdlPort port = new WsdlSoapPort(binding);
                  if(port.convertFromXmlElement(soapElement))
                  {
                     // port converted, add it
                     getPorts().add(port);
                  }
               }
            }
         }

         // ensure there is a name
         if(!getName().equals(""))            
         {
            // conversion successful
            rval = true;
         }         
      }
      
      return rval;
   }   
   
   /**
    * Gets the logger.
    * 
    * @return the logger.
    */
   public Logger getLogger()
   {
      return LoggerManager.getLogger("dbnet");
   }
   
   /**
    * A WSDL Port collection.
    * 
    * @author Dave Longley
    */
   public class WsdlPortCollection
   {
      /**
       * The underlying vector for storing operations. 
       */
      protected Vector mPorts;
      
      /**
       * Creates a new Wsdl Port Collection.
       */
      public WsdlPortCollection()
      {
         // initialize ports vector
         mPorts = new Vector();
      }
      
      /**
       * Adds a new port to this collection.
       * 
       * @param port the operation to add to this collection.
       */
      public void add(WsdlPort port)
      {
         mPorts.add(port);
      }
      
      /**
       * Removes a port from this collection.
       * 
       * @param port the port to remove from this collection.
       */
      public void remove(WsdlPort port)
      {
         mPorts.remove(port);
      }
      
      /**
       * Gets an port from this collection according to its name.
       * 
       * @param name the name of the port to retrieve.
       * 
       * @return the port or null if one was not found.
       */
      public WsdlPort getPort(String name)
      {
         WsdlPort rval = null;
         
         for(Iterator i = iterator(); i.hasNext() && rval == null;) 
         {
            WsdlPort port = (WsdlPort)i.next();
            if(port.getName().equals(name))
            {
               rval = port;
            }
         }
         
         return rval;
      }
      
      /**
       * Clears all the port from this collection.
       */
      public void clear()
      {
         mPorts.clear();
      }
      
      /**
       * Gets an iterator over the port in this collection.
       * 
       * @return an iterator over the port in this collection.
       */
      public Iterator iterator()
      {
         return mPorts.iterator();
      }
   }   
}
