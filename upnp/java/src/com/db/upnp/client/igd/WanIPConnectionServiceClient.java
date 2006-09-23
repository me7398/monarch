/*
 * Copyright (c) 2006 Digital Bazaar, Inc.  All rights reserved.
 */
package com.db.upnp.client.igd;

import java.net.ConnectException;

import com.db.upnp.client.AbstractClientUPnPServiceImplementation;
import com.db.upnp.service.UPnPErrorException;
import com.db.upnp.service.UPnPService;
import com.db.util.BoxingHashMap;

/**
 * A WanIPConnectionServiceClient is a client for a WanIPConnection Service.
 * 
 * FIXME: The implementation for this class is incomplete. Only port forwarding
 * functionality has been implemented.
 * 
 * @author Dave Longley
 */
public class WanIPConnectionServiceClient
extends AbstractClientUPnPServiceImplementation
{
   /**
    * The service type for a WAN IP Connection Service.
    */
   public static final String WAN_IP_CONNECTION_SERVICE_TYPE =
      "urn:schemas-upnp-org:service:WANIPConnection:1";
   
   /**
    * Creates a new WanIPConnectionServiceClient.
    * 
    * @param service the UPnPService this client implementation is for.
    */
   public WanIPConnectionServiceClient(UPnPService service)
   {
      super(service);
   }
   
   /**
    * This action will retrieve the NAT port mapping with the specified index.
    * A Control point can call this action (incrementally) until null is
    * returned indicating that no entry exists at the given index. All entries
    * are stored in a contiguous array and can be retrieved using an array
    * index that ranges from 0 to PortMappingNumberOfEntries - 1.
    * 
    * @param index the index in the port mapping entry array at which to
    *              retrieve a generic port mapping entry.
    * 
    * @return a generic port mapping entry or null if no entry exists at
    *         the given index.
    * 
    * @exception ConnectException thrown if connection to the service is
    *                             refused.
    * @exception UPnPErrorException thrown if a UPnPError occurs:
    * 
    * "402 Invalid Args" One of following: not enough IN arguments, too many IN
    * arguments, no IN argument by that name, one or more IN arguments are of
    * the wrong data type.
    */
   public PortMapping getGenericPortMappingEntry(int index)
   throws ConnectException, UPnPErrorException
   {
      PortMapping rval = null;
      
      try
      {
         // perform the action
         BoxingHashMap retvals = performAction(
            "GetGenericPortMappingEntry", new Object[]{new Integer(index)});
         rval = new PortMapping(retvals);
      }
      catch(UPnPErrorException e)
      {
         // if the error is 713 then the index was just invalid (we return
         // null), otherwise we want to throw the error
         if(e.getUPnPError().getErrorCode() != 713)
         {
            // throw the error
            throw e;
         }
      }
      
      return rval;
   }
   
   /**
    * This action will retrieve the NAT port mapping for the given
    * RemoteHost, ExternalPort, and PortMappingProtocol.
    * 
    * @param remoteHost the remote host (an IP address "x.x.x.x" as a string).
    * @param externalPort the external port.
    * @param protocol the protocol ("TCP" or "UDP").
    * 
    * @return the port mapping entry or null if none exists for the given
    *         parameters.
    * 
    * @exception ConnectException thrown if connection to the service is
    *                             refused.
    * @exception UPnPErrorException thrown if a UPnPError occurs:
    * 
    * "402 Invalid Args" One of following: not enough IN arguments, too many IN
    * arguments, no IN argument by that name, one or more IN arguments are of
    * the wrong data type.
    * 
    * "714 NoSuchEntryInArray" The specified array index is out of
    * bounds.
    */
   public PortMapping getSpecificPortMappingEntry(
      String remoteHost, int externalPort, String protocol)
   throws ConnectException, UPnPErrorException
   {
      PortMapping rval = null;
      
      try
      {
         // perform the action
         BoxingHashMap retvals = performAction(
            "GetSpecificPortMappingEntry",
            new Object[]{remoteHost, new Integer(externalPort), protocol});
         rval = new PortMapping(retvals);
      }
      catch(UPnPErrorException e)
      {
         // if the error is 714 then no such entry exists (we return null),
         // otherwise we want to throw the error
         if(e.getUPnPError().getErrorCode() != 714)
         {
            // throw the error
            throw e;
         }
      }
      
      return rval;
   }
   
   /**
    * This action will create a new port mapping or overwrite an existing
    * port mapping with the same internal client. If the passed external
    * port and PortMappingProtocol part is already mapped to another internal
    * client, then an error will be thrown.
    * 
    * Note: Not all NAT implementations will support:
    * Wilcard values (i.e. 0) for the external port
    * Internal port values that are different from the external port
    * Dynamic port mappings (i.e. with finite lease durations)
    * 
    * @param portMapping the new port mapping to add.
    * 
    * @exception ConnectException thrown if connection to the service is
    *                             refused.
    * @exception UPnPErrorException thrown if a UPnPError occurs:
    * 
    * "402 Invalid Args" One of following: not enough IN arguments, too many IN
    * arguments, no IN argument by that name, one or more IN arguments are of
    * the wrong data type.
    * 
    * "714 NoSuchEntryInArray" The specified array index is out of
    * bounds.
    * 
    * "715 WildCardNotPermittedInSrcIP" The source IP address cannot be
    * wild-carded.
    * 
    * "716 WildCardNotPermittedInExtPort" The external port cannot be
    * wild-carded.
    * 
    * "718 ConflictInMappingEntry" The port mapping entry specified conflicts
    * with a mapping assigned previously to another client.
    * 
    * "724 SamePortValuesRequired" Internal and External port values must be
    * the same.
    * 
    * "725 OnlyPermanentLeasesSupported" The NAT implementation only supports
    * permanent lease times on port mappings.
    * 
    * "726 RemoteHostOnlySupportsWildcard" RemoteHost must be a wildcard and
    * cannot be a specific IP address or DNS name.
    * 
    * "727 ExternalPortOnlySupportsWildcard" ExternalPort must be a wildcard
    * and cannot be a specific port value.
    */   
   public void addPortMapping(PortMapping portMapping)
   throws ConnectException, UPnPErrorException
   {
      // build the parameters array for the port mapping
      Object[] params = new Object[]
      {
         portMapping.getRemoteHost(),
         new Integer(portMapping.getExternalPort()),
         portMapping.getProtocol(),
         new Integer(portMapping.getInternalPort()),
         portMapping.getInternalClient(),
         new Boolean(portMapping.isEnabled()),
         portMapping.getDescription(),
         new Integer(portMapping.getLeaseDuration())
      };
      
      // perform the action
      performAction("AddPortMapping", params);
   }
   
   /**
    * This action will delete a previously created port mapping with the
    * given remote host, external port, and protocol. When an entry is
    * deleted, PortMappingNumberOfEntries decrements.
    * 
    * @param remoteHost the remote host (an IP address "x.x.x.x" as a string).
    * @param externalPort the external port.
    * @param protocol the protocol ("TCP" or "UDP").
    * 
    * @exception ConnectException thrown if connection to the service is
    *                             refused.
    * @exception UPnPErrorException thrown if a UPnPError occurs:
    * 
    * "402 Invalid Args" One of following: not enough IN arguments, too many IN
    * arguments, no IN argument by that name, one or more IN arguments are of
    * the wrong data type.
    * 
    * "714 NoSuchEntryInArray" There was no entry to delete that matched the
    * passed parameters.
    */
   public void deletePortMapping(
      String remoteHost, int externalPort, String protocol)
   throws ConnectException, UPnPErrorException
   {
      // perform the action
      performAction(
         "DeletePortMapping",
         new Object[]{remoteHost, new Integer(externalPort), protocol});
   }
   
   /**
    * This action will retrieve the external IP address for the service.
    * 
    * @return the external IP address for the service.
    * 
    * @exception ConnectException thrown if connection to the service is
    *                             refused.
    * @exception UPnPErrorException thrown if a UPnPError occurs:
    * 
    * "402 Invalid Args" One of following: not enough IN arguments, too many IN
    * arguments, no IN argument by that name, one or more IN arguments are of
    * the wrong data type.
    * 
    * "501 ActionFailed" See UPnP Device Architecture section on Control.
    */
   public String getExternalIPAddress()
   throws ConnectException, UPnPErrorException
   {
      BoxingHashMap retvals =
         performAction("GetExternalIPAddress", null);
      
      return retvals.getString("NewExternalIPAddress");
   }
}
