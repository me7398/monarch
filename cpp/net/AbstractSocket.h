/*
 * Copyright (c) 2007-2010 Digital Bazaar, Inc. All rights reserved.
 */
#ifndef monarch_net_AbstractSocket_H
#define monarch_net_AbstractSocket_H

#include "monarch/net/Socket.h"
#include "monarch/io/InputStream.h"
#include "monarch/io/OutputStream.h"

#include <string>

namespace monarch
{
namespace net
{

/**
 * An AbstractSocket provides the basic implementation for a Socket. This is
 * the base class for types of Sockets that have specific protocols and
 * implementations.
 *
 * @author Dave Longley
 */
class AbstractSocket : public Socket
{
protected:
   /**
    * A file descriptor for the Socket.
    */
   int mFileDescriptor;

   /**
    * The communication domain for this Socket.
    */
   SocketAddress::CommunicationDomain mCommDomain;

   /**
    * True when this Socket is bound, false when not.
    */
   bool mBound;

   /**
    * True when this Socket is listening, false when not.
    */
   bool mListening;

   /**
    * True when this Socket is connected, false when not.
    */
   bool mConnected;

   /**
    * The stream for reading from the Socket.
    */
   monarch::io::InputStream* mInputStream;

   /**
    * The stream for writing to the Socket.
    */
   monarch::io::OutputStream* mOutputStream;

   /**
    * The send timeout (in milliseconds) for writing to the Socket.
    */
   uint32_t mSendTimeout;

   /**
    * The receive timeout (in milliseconds) for reading from the Socket.
    */
   uint32_t mReceiveTimeout;

   /**
    * The number of Socket connections to keep backlogged while listening.
    */
   int mBacklog;

   /**
    * True if sends do not block, false if not.
    */
   bool mSendNonBlocking;

   /**
    * True if receives do not block, false if not.
    */
   bool mReceiveNonBlocking;

   /**
    * Creates a Socket with the specified type and protocol and assigns its
    * file descriptor to mFileDescriptor.
    *
    * @param domain the communication domain (i.e. PF_INET or PF_INET6).
    * @param type the type of Socket to create (i.e. SOCK_STREAM or SOCK_DGRAM).
    * @param protocol the protocol for the Socket
    *           (i.e. IPPROTO_TCP or IPPROTO_UDP).
    *
    * @return true if the socket could be created, false if an exception
    *         occurred.
    */
   virtual bool create(int domain, int type, int protocol);

   /**
    * Blocks until data is available for receiving, the socket can be
    * written to, a timeout, or until a connection closes.
    *
    * @param read true to block until data can be received, false to block
    *           until data can be sent.
    * @param timeout the timeout to use in milliseconds (0 for no timeout
    *           and -1 for immediate timeout).
    *
    * @return false if an exception occurred, true if not.
    */
   virtual bool waitUntilReady(bool read, int64_t timeout);

   /**
    * Acquiring a file descriptor for this Socket. This method must be called
    * before trying to use this Socket.
    *
    * This method is called automatically by the default implementation.
    *
    * @param domain the communication domain for this Socket (i.e. IPv4, IPv6).
    *
    * @return true if the file descriptor could be acquired, false if
    *         an exception occurred.
    */
   virtual bool acquireFileDescriptor(
      SocketAddress::CommunicationDomain domain) = 0;

   /**
    * Initializes the input stream for this Socket, if it is not already
    * initialized. This method must be called before trying to read from this
    * Socket.
    *
    * This method should be indempotent such that multiple calls can be
    * performed safely and will not cause the stream to be reset.
    *
    * This method is called automatically by the default implementation.
    *
    * @return true if no exception occurred, false if not.
    */
   virtual bool initializeInput();

   /**
    * Initializes the output stream for this Socket, if it is not already
    * initialized. This method must be called before trying to write to this
    * Socket.
    *
    * This method should be indempotent such that multiple calls can be
    * performed safely and will not cause the stream to be reset.
    *
    * This method is called automatically by the default implementation.
    *
    * @return true if no exception occurred, false if not.
    */
   virtual bool initializeOutput();

   /**
    * Shuts down the input stream for this Socket, if it is currently
    * initialized.
    *
    * This method is called automatically by the default implementation.
    *
    * @return true if no exception occurred, false if not.
    */
   virtual bool shutdownInput();

   /**
    * Shuts down the output stream for this Socket, if it is currently
    * initialized.
    *
    * This method is called automatically by the default implementation.
    *
    * @return true if no exception occurred, false if not.
    */
   virtual bool shutdownOutput();

   /**
    * Creates a new Socket with the given file descriptor that points to
    * the socket for an accepted connection.
    *
    * @param fd the file descriptor for the socket.
    *
    * @return the allocated connected Socket.
    */
   virtual Socket* createConnectedSocket(int fd) = 0;

public:
   /**
    * Creates a new AbstractSocket.
    */
   AbstractSocket();

   /**
    * Destructs this AbstractSocket.
    */
   virtual ~AbstractSocket();

   /**
    * Binds this Socket to a SocketAddress.
    *
    * @param address the address to bind to.
    *
    * @return true if bound, false if an exception occurred.
    */
   virtual bool bind(SocketAddress* address);

   /**
    * Causes this Socket to start listening for incoming connections.
    *
    * @param backlog the number of connections to keep backlogged.
    *
    * @return true if listening, false if an exception occurred.
    */
   virtual bool listen(int backlog = 50);

   /**
    * Accepts a connection to this Socket. This method will block until a
    * connection is made to this Socket.
    *
    * The returned Socket will wrap the file descriptor that is used to
    * communicated with the connected socket.
    *
    * @param timeout the timeout, in seconds, 0 for no timeout.
    *
    * @return an allocated Socket to use to communicate with the connected
    *         socket or NULL if an error occurs.
    */
   virtual Socket* accept(int timeout);

   /**
    * Connects this Socket to the given address.
    *
    * @param address the address to connect to.
    * @param timeout the timeout, in seconds, 0 for no timeout.
    *
    * @return true if no exception occurred, false if an exception occurred.
    */
   virtual bool connect(SocketAddress* address, int timeout = 30);

   /**
    * Writes raw data to this Socket. This method will block until all of
    * the data has been written.
    *
    * Note: This method is *not* preferred. Use getOutputStream() to obtain the
    * output stream for this Socket.
    *
    * @param b the array of bytes to write.
    * @param length the number of bytes to write to the stream.
    *
    * @return true if the data was sent, false if an exception occurred.
    */
   virtual bool send(const char* b, int length);

   /**
    * Reads raw data from this Socket. This method will block until at least
    * one byte can be read or until the end of the stream is reached (the
    * Socket has closed). A value of 0 will be returned if the end of the
    * stream has been reached, otherwise the number of bytes read will be
    * returned.
    *
    * Note: This method is *not* preferred. Use getInputStream() to obtain the
    * input stream for this Socket.
    *
    * @param b the array of bytes to fill.
    * @param length the maximum number of bytes to read into the buffer.
    *
    * @return the number of bytes read from the stream or 0 if the end of the
    *         stream (the Socket has closed) has been reached or -1 if an error
    *         occurred.
    */
   virtual int receive(char* b, int length);

   /**
    * Closes this Socket. This will be done automatically when the Socket is
    * destructed.
    */
   virtual void close();

   /**
    * Returns true if this Socket is bound, false if not.
    *
    * @return true if this Socket is bound, false if not.
    */
   virtual bool isBound();

   /**
    * Returns true if this Socket is listening, false if not.
    *
    * @return true if this Socket is listening, false if not.
    */
   virtual bool isListening();

   /**
    * Returns true if this Socket is connected, false if not.
    *
    * @return true if this Socket is connected, false if not.
    */
   virtual bool isConnected();

   /**
    * Gets the local SocketAddress for this Socket.
    *
    * @param address the SocketAddress to populate.
    *
    * @return true if the address was populated, false if an exception occurred.
    */
   virtual bool getLocalAddress(SocketAddress* address);

   /**
    * Gets the remote SocketAddress for this Socket.
    *
    * @param address the SocketAddress to populate.
    *
    * @return true if the address was populated, false if an exception occurred.
    */
   virtual bool getRemoteAddress(SocketAddress* address);

   /**
    * Gets the InputStream for reading from this Socket.
    *
    * @return the InputStream for reading from this Socket.
    */
   virtual monarch::io::InputStream* getInputStream();

   /**
    * Gets the OutputStream for writing to this Socket.
    *
    * @return the OutputStream for writing to this Socket.
    */
   virtual monarch::io::OutputStream* getOutputStream();

   /**
    * Sets the send timeout for this Socket. This is the amount of time that
    * this Socket will block waiting to send data.
    *
    * @param timeout the send timeout in milliseconds.
    */
   virtual void setSendTimeout(uint32_t timeout);

   /**
    * Gets the send timeout for this Socket. This is the amount of time that
    * this Socket will block waiting to send data.
    *
    * @return the send timeout in milliseconds.
    */
   virtual uint32_t getSendTimeout();

   /**
    * Sets the receive timeout for this Socket. This is the amount of time that
    * this Socket will block waiting to receive data.
    *
    * @param timeout the receive timeout in milliseconds.
    */
   virtual void setReceiveTimeout(uint32_t timeout);

   /**
    * Gets the receive timeout for this Socket. This is the amount of time that
    * this Socket will block waiting to receive data.
    *
    * @return the receive timeout in milliseconds.
    */
   virtual uint32_t getReceiveTimeout();

   /**
    * Gets the number of Socket connections that can be kept backlogged while
    * listening.
    *
    * @return the number of Socket connections that can be kept backlogged
    *         while listening.
    */
   virtual int getBacklog();

   /**
    * Gets the file descriptor for this Socket.
    *
    * @return the file descriptor for this Socket.
    */
   virtual int getFileDescriptor();

   /**
    * Gets the communication domain for this Socket, i.e. IPv4, IPv6.
    *
    * @return the communication domain for this Socket.
    */
   virtual SocketAddress::CommunicationDomain getCommunicationDomain();

   /**
    * Sets whether or not this Socket should not block when sending. If true,
    * then its send and OutputStream will return Exceptions when they would
    * block. An exception detail of "wouldBlock" will be set to true and
    * the number of bytes actually sent will be set to "written".
    *
    * @param on true to activate non-blocking send, false not to.
    */
   virtual void setSendNonBlocking(bool on);

   /**
    * Gets whether or not this Socket blocks when sending.
    *
    * @return true if this Socket does not block when sending, false if it
    *         does.
    */
   virtual bool isSendNonBlocking();

   /**
    * Sets whether or not this Socket should not block when receiving. If true,
    * then its receive and InputStream will return Exceptions when they would
    * block. An exception detail of "wouldBlock" will be set to true.
    *
    * @param on true to activate non-blocking receive, false not to.
    */
   virtual void setReceiveNonBlocking(bool on);

   /**
    * Gets whether or not this Socket blocks when receiving.
    *
    * @return true if this Socket does not block when receiving, false if it
    *         does.
    */
   virtual bool isReceiveNonBlocking();
};

} // end namespace net
} // end namespace monarch
#endif
