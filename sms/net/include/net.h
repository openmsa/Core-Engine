/** @file net.h
  This is the header file of the net library.
  @ingroup  net
  */

#ifndef __NET_H_
#define __NET_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <netinet/in.h>


/** Convert a hostname in an IP address
    @param      host    The hostname to look its address for.
    @return     INADDR_ANY if the hostname is empty.<p>
    @return     The address IP of the hostname.<p>
    @return     <b>-1</b>  if the lookup failed.
    @warning    This function does a ns-lookup and requires a DNS or a correct /etc/hosts
 */
extern int hostnameToIP(const char *host);
extern int hostnameToIPv6(char *host, struct in6_addr *sinaddr);

/** This function creates a sockect and bind this socket on the given port number and hostname
    @param      hostname        The hostname.
    @param      port    The port number.
    @return     The created socke.
    @return     <b>-1</b>  if the bind failed.
    @warning    This function does a ns-lookup and requires a DNS or a correct /etc/hosts
 */
extern int      socketBind (const char * hostname, int port);



/** This function creates a sockect and bind this socket on the given port number and hostname
    @param      hostname        The hostname.
    @param      port    The port number.
    @return     The created socke.
    @return     <b>-1</b>  if the bind failed.
    @warning    This function does a ns-lookup and requires a DNS or a correct /etc/hosts
 */
extern int      socketBindUDP6(char * hostname, int port);



/** Write a buffer in a socket
    @param      fd      The socket to write
    @param      buf     The buffer to write
    @param      n       The number of bytes to write
    @return     The number of bytes written.
    @return     If the number of bytes to write is  <= 0 the function return 0 without writting in the socket.
 */
extern int socketWrite(int fd, const char *buf, int n);

/** Read a buffer from a socket
    @param      fd      The socket to read
    @param      buf     The buffer to return
    @param      n       The number of bytes to read
    @return     The number of bytes read.
    @return     If the number of bytes to read is  <= 0 the function return 0 without reading from the socket.
 */
extern int socketRead(int fd, char *buf, int n);

/** Read a buffer from a socket
 * @param      fd      The socket to read
 * @param      buffer  The buffer to return
 * @param      maxlen  The max size of the buffer
 * @return the size of the buffer read or <0 if error.
 * IMPORTANT NOTE: the buffer have to be released by the caller.
 */
extern int socketReadBuf(int fd, char **buffer, int maxlen);

/** Read a Line from a socket
    @param      fd      The socket to read
    @param      buf     The buffer to return
    @param      n       The number of bytes maximum to read
    @param      t       The timeout in secondes;
    @return     The number of bytes read.
    @return     If the number of bytes to read is  <= 0 the function return 0 without reading from the socket.
 */
extern int socketReadLine(int fd, char *buf, int n, int t);


/** Close a socket
    @param      fd      The socket to close
    */
extern int  socketClose(int fd);

/** Accept a connection from a socket
    @param      fd      The socket
    @param      remote_ip_str     The remote address IP string pointer
    @param      remote_ip_long   A pointer on the remote address IP long pointer that will be returned
    @return     The socket with the accepted connection.
    @return     <b>remote_ip_str</b>: the remote address IP string pointer is filled and must be freed by the calling functio
    @return     <b>remote_ip_long</b>:the remote address IP long pointer is filled.

 */
extern int socketAccept (int fd, char ** remote_ip_str, long * remote_ip_long);


/** Connect to a server
    @param      host    The server name
    @param      portnum   The port of the service
    @return     -1 if the connect failed or the sockect fd if success
 */
extern int socketConnect(const char *host, int portnum);


extern int UDPsocketConnect(char *host, int portnum, char * IP);

/** This function creates a socket, send packet on this socket for the given port number and hostname
    and finally close the socket
    @param	hostname	The hostname.
    @param	port	The port number.
    @return     The created socket.
    @return	<b>-1</b>  if the socket or sendto failed.
    @warning	This function does a ns-lookup and requires a DNS or a correct /etc/hosts
 */
int socketSendUDP (char *hostname, int port, char *msg, int len);

#ifdef __cplusplus
}
#endif

#endif    /* __NET_H_ */
