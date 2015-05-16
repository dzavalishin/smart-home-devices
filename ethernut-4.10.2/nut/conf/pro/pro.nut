--
-- Copyright (C) 2004-2005 by egnite Software GmbH. All rights reserved.
--
-- Redistribution and use in source and binary forms, with or without
-- modification, are permitted provided that the following conditions
-- are met:
--
-- 1. Redistributions of source code must retain the above copyright
--    notice, this list of conditions and the following disclaimer.
-- 2. Redistributions in binary form must reproduce the above copyright
--    notice, this list of conditions and the following disclaimer in the
--    documentation and/or other materials provided with the distribution.
-- 3. Neither the name of the copyright holders nor the names of
--    contributors may be used to endorse or promote products derived
--    from this software without specific prior written permission.
--
-- THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
-- ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
-- LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
-- FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
-- SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
-- INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
-- BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
-- OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
-- AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
-- OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
-- THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
-- SUCH DAMAGE.
--
-- For additional information see http://www.ethernut.de/
--

-- Operating system functions
--
-- $Log$
-- Revision 1.14  2009/02/06 15:52:14  haraldkipp
-- Removed stack size defaults.
--
-- Revision 1.13  2009/01/16 17:03:50  haraldkipp
-- Configurable discovery protocol version and port plus
-- configurable service thread stack size. The new version 1.1
-- allows host names up to 98 characters. Added some code
-- to make sure, that nothing is overwritten with version 1.0
-- protocol and too long host names. Protocol version 1.0
-- is still the default.
--
-- Revision 1.12  2008/07/08 13:27:55  haraldkipp
-- Several HTTP server options are now configurable.
-- Keepalive is now disabled by default to maintain backward compatibility.
--
-- Revision 1.11  2008/05/16 03:39:31  thiagocorrea
-- Revert httpd memory allocation calls to NutHeapAlloc for consistency and
-- move DestroyRequestInfo to a shared file (reduces code size and remove duplicates
-- from httpd.c and ssi.c)
--
-- Revision 1.10  2008/04/01 10:11:34  haraldkipp
-- Added the new, enhanced httpd API library.
-- Bugs #1839026 and #1839029 fixed.
--
-- Revision 1.9  2008/02/15 17:20:05  haraldkipp
-- XML streaming parser added.
--
-- Revision 1.8  2006/09/07 09:06:17  haraldkipp
-- Discovery service added.
--
-- Revision 1.7  2005/08/05 11:26:01  olereinhardt
-- Added asp.c, ssi.c
--
-- Revision 1.6  2005/04/05 17:44:56  haraldkipp
-- Made stack space configurable.
--
-- Revision 1.5  2005/02/05 20:41:13  haraldkipp
-- Wins and FTP added.
--
-- Revision 1.4  2005/02/02 15:53:15  haraldkipp
-- DHCP configuration added
--
-- Revision 1.3  2004/09/19 11:18:44  haraldkipp
-- Syslog client added
--
-- Revision 1.2  2004/08/18 13:46:10  haraldkipp
-- Fine with avr-gcc
--
-- Revision 1.1  2004/06/07 16:38:43  haraldkipp
-- First release
--
--

nutpro =
{
    {
        name = "nutpro_discover",
        brief = "Discovery",
        description = "This service allows to scan a local network for Nut/OS nodes "..
                      "and modify their network settings by running the Nut/OS discovery "..
                      "tool on a desktop PC.\n\n"..
                      "It is not available by default and must be activated by the "..
                      "application.",
        requires = { "NET_UDP" },
        sources = { "discover.c" },
        options = 
        {
            {
                macro = "DISCOVERY_VERSION",
                brief = "Protocol Version",
                description = "Make sure, that your discovery tool and any callback "..
                              "implemented in your application supports the selected version.\n\n"..
                              "Version 0x10 truncates the host name to 7 characters, while "..
                              "version 0x11 allows host names up to 98 characters, which "..
                              "however will reduce the custom area to a single byte.",
                type = "enumerated",
                choices = { "0x10", "0x11" },
                default = "0x10",
                file = "include/cfg/discover.h"
            },
            {
                macro = "NUT_THREAD_DISTSTACK",
                brief = "Service Thread Stack",
                description = "Number of bytes to be allocated for the stack of the discovery "..
                              "service thread. May be enabled and set to fine tune RAM usage.",
                flavor = "booldata",
                file = "include/cfg/discover.h"
            },
            {
                macro = "DISCOVERY_PORT",
                brief = "Service Port",
                description = "UDP port used by the discovery service.\n\n"..
                              "Used as a default. The application may choose a different value.",
                default = "9806",
                flavor = "integer",
                file = "include/cfg/discover.h"
            },
        }
    },
    {
        name = "nutpro_dhcpc",
        brief = "DHCP/BOOTP Client",
        requires = { "NET_UDP", "NUT_EVENT" },
        sources = 
        { 
            "dhcpc.c"
        },
        options = 
        {
            {
                macro = "DHCP_SERVERPORT",
                brief = "Server Port",
                description = "UDP port of the DHCP server. Default is 67.", 
                flavor = "booldata",
                file = "include/cfg/dhcp.h"
            },
            {
                macro = "DHCP_CLIENTPORT",
                brief = "Client Port",
                description = "UDP port of the DHCP client. Default is 68.", 
                flavor = "booldata",
                file = "include/cfg/dhcp.h"
            },
            {
                macro = "DHCP_BROADCAST_FLAG",
                brief = "Broadcast Flag",
                description = "If enabled, the client will set the broadcast flag in all "..
                              "outgoing messages. This is not required, because Nut/Net is "..
                              "able to receive UPD datagrams without configuring the "..
                              "interface.",
                flavor = "boolean",
                file = "include/cfg/dhcp.h"
            },
            {
                macro = "MIN_DHCP_MSGSIZE",
                brief = "Min. Message Size",
                description = "Used to maintain BOOTP compatibility of outgoing messages. "..
                              "Default is 300 octets.",
                flavor = "booldata",
                file = "include/cfg/dhcp.h"
            },
            {
                macro = "MAX_DHCP_MSGSIZE",
                brief = "Max. Message Size",
                description = "RFC 2131 demands, that a DHCP client must be prepared to receive DHCP "..
                              "messages with an options field length of at least 312 octets. This "..
                              "implies that we must be able to accept messages of up to 576 octets (default)",
                flavor = "booldata",
                file = "include/cfg/dhcp.h"
            },
            {
                macro = "MAX_DHCP_BUFSIZE",
                brief = "UDP Buffer Size",
                description = "By default this value is set to 1728 and allows the client to concurrently "..
                              "receive offers from up to 3 DHCP servers. Setting this value to zero will "..
                              "save some heap space by disabling UDP buffering. This is fine for networks "..
                              "with a single DHCP server.",
                flavor = "booldata",
                file = "include/cfg/dhcp.h"
            },
            {
                macro = "MIN_DHCP_WAIT",
                brief = "Min. Wait Time",
                description = "The client will wait this number of milliseconds (default is 4000) before "..
                              "resending a request. The timeout is doubled on each retry up to the "..
                              "number of milliseconds specified by MAX_DHCP_WAIT.",
                flavor = "booldata",
                file = "include/cfg/dhcp.h"
            },
            {
                macro = "MAX_DHCP_WAIT",
                brief = "Max. Wait Time",
                description = "With each retry the client will double the number of milliseconds to wait "..
                              "for a response from the server. However, the maximum time can be limited "..
                              "by this item, which defaults to 64000.",
                flavor = "booldata",
                file = "include/cfg/dhcp.h"
            },
            {
                macro = "MAX_DCHP_RETRIES",
                brief = "Max. Request Retries",
                description = "The client will give up after resending this number of requests without "..
                              "receiving a response from the server. Default is 3.",
                flavor = "booldata",
                file = "include/cfg/dhcp.h"
            },
            {
                macro = "MAX_DCHP_RELEASE_RETRIES",
                brief = "Max. Release Retries",
                description = "RFC 2131 doesn't specify a server response to release messages from "..
                              "the client. If the message gets lost, then the lease isn't released. "..
                              "Setting a value greater than zero (default) will cause the client "..
                              "to resend a release message for the given number of times or until "..
                              "the server sends a response.",
                flavor = "booldata",
                file = "include/cfg/dhcp.h"
            },
            {
                macro = "DHCP_DEFAULT_LEASE",
                brief = "Default Lease",
                description = "If the server doesn't provide a maximum lease time, the client "..
                              "uses the number of seconds given by this value. Default is 43200.",
                flavor = "booldata",
                file = "include/cfg/dhcp.h"
            },
            {
                macro = "NUT_THREAD_DHCPSTACK",
                brief = "Client Thread Stack",
                description = "Number of bytes to be allocated for the stack of the DHCP client thread.\n\n"..
                              "For ARM CPUs GCC uses about 280 bytes for size optimized code.",
                flavor = "booldata",
                file = "include/cfg/dhcp.h"
            },
        }

    },
    {
        name = "nutpro_resolv",
        brief = "DNS Client API",
        requires = { "NET_UDP" },
        sources = 
        { 
            "confdns.c",
            "resolv.c"
        }
    },
    {
        name = "nutpro_ftpd",
        brief = "FTP Server API",
        description = "File transfer protocol server.",
        requires = 
        {
            "NET_TCP",
            "NET_UDP",
            "CRT_STREAM_READ", 
            "CRT_STREAM_WRITE", 
            "NUT_FS_DIR", 
            "NUT_FS_READ", 
            "NUT_FS_WRITE" 
        },
        sources = { "ftpd.c" }
    },
    {
        name = "nutpro_smtp",
        brief = "SMTP Client API",
        description = "Provides email transfer via SMTP.",
        requires = { "NET_TCP", "CRT_STREAM_READ" },
        sources = 
        { 
            "smtpc.c"
        },
        options = 
        {
            {
                macro = "MAX_MAIL_RCPTS",
                brief = "Max. Number of Recipients",
                description = "Maximum number of recipients for a single email envelope.",
                type = "integer",
                default = "4",
                file = "include/cfg/smtp.h"
            },
            {
                macro = "SMTP_BUFSIZ",
                brief = "Size of the SMTP Buffer",
                type = "integer",
                default = "256",
                file = "include/cfg/smtp.h"
            }
        }
    },
    {
        name = "nutpro_httpd",
        brief = "HTTP Server API",
        description = "Webserver helper routines. Provides simple authorization "..
                      "and registration of C functions as CGI routines",
        requires = { "NET_TCP", "CRT_STREAM_READ", "NUT_FS", "NUT_FS_READ" },
        sources = 
        { 
            "auth.c",
            "cgi.c",
            "dencode.c",
            "discover.c",
            "httpd.c",
            "httpd_p.c",
            "httpopt.c",
            "asp.c",
            "ssi.c",
            "rfctime.c"
        },
        options = 
        {
            {
                macro = "HTTP_MAJOR_VERSION",
                brief = "Major Version",
                description = "The major HTTP version number reported to the client.",
                type = "integer",
                default = "1",
                file = "include/cfg/http.h"
            },
            {
                macro = "HTTP_MINOR_VERSION",
                brief = "Minor Version",
                description = "The minor HTTP version number reported to the client.",
                type = "integer",
                default = "1",
                file = "include/cfg/http.h"
            },
            {
                macro = "HTTP_DEFAULT_ROOT",
                brief = "Default Root Directory",
                description = 'The default root directory used by the server to locate '..
                              'requested files. The value must be enclosed in double quotes. '..
                              'Applications typically override this by calling '..
                              'NutRegisterHttpRoot.\n\n',
                default = '"UROM:"',
                file = "include/cfg/http.h"
            },
            {
                macro = "HTTP_MAX_REQUEST_SIZE",
                brief = "Max. Request Line Size",
                description = "HTTP request beyond this length will be discarded.",
                type = "integer",
                default = "256",
                file = "include/cfg/http.h"
            },
            {
                macro = "HTTP_FILE_CHUNK_SIZE",
                brief = "Chunk Size",
                description = "The number of bytes to be used for the file read buffer. "..
                              "Reducing this value will save RAM, but degrade performance.",
                type = "integer",
                default = "512",
                file = "include/cfg/http.h"
            },
            {
                macro = "HTTP_KEEP_ALIVE_REQ",
                brief = "Max. Requests per Connection",
                description = "The server will close a connection after the specified "..
                              "number of requests. Zero by default, which completely disables "..
                              "the HTTP keepalive function.\n\n"..
                              "Enabling keepalive will significantly increase the performance. "..
                              "However, browsers may not properly close keepalive connections. "..
                              "Thus, it is required to enable socket receive timeouts.",
                type = "integer",
                default = "0",
                file = "include/cfg/http.h"
            },
            {
                macro = "HTTPD_EXCLUDE_DATE",
                brief = "Exclude Date Information",
                description = "By default, the server includes file date information "..
                              "in the response header. This may not always make sense "..
                              "and some memory can be saved by enabling this option.",
                flavor = "boolean",
                file = "include/cfg/http.h"
            }
        }
    },
    {
        name = "nutpro_snmp",
        brief = "SNMP",
        description = "Simple network management protocol.",
        sources =
        { 
            "snmp.c",
            "snmp_api.c",
            "snmp_auth.c",
            "snmp_config.c",
            "snmp_mib.c",
            "snmp_pdu.c",
            "asn1.c"
        }
    },
    {
        name = "nutpro_snmp_udp",
        brief = "SNMP Agent",
        description = "SNMP agent module.",
        requires = { "NET_UDP" },
        sources =
        { 
            "snmp_agent.c",
            "snmp_session.c"
        }
    },
    {
        name = "nutpro_sntp",
        brief = "SNTP Client API",
        description = "Simple network time protocol.",
        requires = { "NET_UDP" },
        provides = { "PRO_SNTP" },
        sources =  { "sntp.c" },
        options = 
        {
            {
                macro = "NUT_THREAD_SNTPSTACK",
                brief = "Client Thread Stack",
                description = "Number of bytes to be allocated for the stack of the SNTP client thread.",
                flavor = "booldata",
                file = "include/cfg/sntp.h"
            }
        }
    },
    {
        name = "nutpro_smtpc",
        brief = "SMTP Client API",
        description = "Simple mail transfer protocol. Not implemented.",
        requires = 
        {
            "NOT_AVAILABLE",
            "NET_TCP",
            "CRT_STREAM_READ", 
            "CRT_STREAM_WRITE"
        }
    },
    {
        name = "nutpro_syslog",
        brief = "Syslog Client API",
        description = "Logs system and debug information to a remote server.",
        requires = { "PRO_SNTP", "NET_UDP" },
        sources =  { "syslog.c", "syslog_P.c" },
        options = 
        {
            {
                macro = "SYSLOG_PERROR_ONLY",
                brief = "Disable Network",
                description = "If set, syslog will not support network transfers.\n\n"..
                              "This option will significantly decrease memory usage. "..
                              "The application should call openlog() with option "..
                              "LOG_PERROR to redirect all syslog messages to stderr. "..
                              "This requires that stderr had been opened.",
                flavor = "boolean",
                file = "include/cfg/syslog.h"
            },
            {
                macro = "SYSLOG_PORT",
                brief = "Default Port",
                description = "Default UDP port for sending syslog messages.\n\n"..
                              "The application may override this value when calling "..
                              "setlogserver().",
                default = "514",
                flavor = "integer",
                file = "include/cfg/syslog.h"
            },
            {
                macro = "SYSLOG_MAXBUF",
                brief = "Output buffer size",
                description = "This is a critical value. If set too low, then "..
                              "syslog may crash with long messages.",
                default = 256,
                flavor = "integer",
                file = "include/cfg/syslog.h"
            },
            {
                macro = "SYSLOG_RFC5424",
                brief = "RFC5424 Headers",
                description = "Set this option to activate RFC5424 compatible message headers.\n\n"..
                              "By default, the syslog client sends BSD-like messages, which are "..
                              "not well defined.",
                flavor = "boolean",
                file = "include/cfg/syslog.h"
            },
            {
                macro = "SYSLOG_OMIT_TIMESTAMP",
                brief = "Disable Timestamp",
                description = "Set this option to exclude timestamps from syslog messages.",
                flavor = "boolean",
                file = "include/cfg/syslog.h"
            },
            {
                macro = "SYSLOG_OMIT_HOSTNAME",
                brief = "Disable Hostname",
                description = "Set this option to exclude the hostname from syslog messages.",
                flavor = "boolean",
                file = "include/cfg/syslog.h"
            },
        }
    },
    {
        name = "nutpro_wins",
        brief = "NetBIOS WINS Responder",
        description = "Netbios WINS (RFC 1002) Name Query Response.\n\n"..
                      "Only query request client routine sending/positive name query response "..
                      "receiving is implemented.\n\n"..
                      "When the NetBIOS name query request UDP datagram is on the ethernet "..
                      "network, asking 'Who is name?', NutWinsNameQuery answers with the "..
                      "specified 'ipaddr' Ethernut IP address.",
        requires =  { "NET_UDP" },
        sources =  { "wins.c" }
    },
    {
        name = "nutpro_uxml",
        brief = "XML Stream Parser",
        description = "Quite limited parser. See API documentation.",
        sources =  { "uxmlparse.c", "uxmlstream.c", "uxmltree.c" }
    }    
}
