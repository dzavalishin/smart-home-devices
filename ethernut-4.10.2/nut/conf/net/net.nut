--
-- Copyright (C) 2004-2007 by egnite Software GmbH. All rights reserved.
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
-- Revision 1.15  2009/02/22 12:22:44  olereinhardt
-- 2009-02-22  Ole Reinhardt <ole.reinhardt@thermotemp.de>
-- Added NUT_UDP_ICMP_SUPPORT (ICMP support on UDP sockets)
--
-- Revision 1.14  2009/02/16 03:10:50  thiagocorrea
-- Fix small typo in text, hardly worth mentioning in the Changelog.
--
-- Revision 1.13  2008/08/20 06:56:59  haraldkipp
-- Implemented IP demultiplexer.
--
-- Revision 1.12  2008/08/11 17:38:27  haraldkipp
-- Added an Ethernet protocol demultiplexer.
--
-- Revision 1.11  2008/07/27 11:43:22  haraldkipp
-- Configurable TCP retransmissions.
--
-- Revision 1.10  2007/05/02 11:18:32  haraldkipp
-- IGMP support added. Incomplete.
--
-- Revision 1.9  2007/03/23 12:41:37  haraldkipp
-- Added more detailed information about the ARP method.
--
-- Revision 1.8  2006/09/05 12:35:39  haraldkipp
-- DHCP servers may probe an IP/MAC relationship by sending an
-- ICMP request. This triggered the Nut/Net ARP method and
-- terminated the DHCP client, leaving the system with default
-- configurations of the network mask (255.255.255.0) and
-- default gateway (none). The rarely used ARP method is now
-- disabled by default.
--
-- Revision 1.7  2006/08/01 07:41:01  haraldkipp
-- New functions ether_aton() and ether_ntoa() added. They convert the ASCII
-- representation of an Ethernet MAC address to its binary form and vice versa.
--
-- Revision 1.6  2006/01/23 17:30:25  haraldkipp
-- Configurable offset into non-volatile memory for network parameters.
--
-- Revision 1.5  2005/04/05 17:44:56  haraldkipp
-- Made stack space configurable.
--
-- Revision 1.4  2005/02/04 15:05:59  haraldkipp
-- ARP configuration added.
--
-- Revision 1.3  2005/01/13 18:47:31  haraldkipp
-- Network dependencies added.
--
-- Revision 1.2  2004/08/18 13:46:09  haraldkipp
-- Fine with avr-gcc
--
-- Revision 1.1  2004/06/07 16:38:43  haraldkipp
-- First release
--
--

nutnet =
{
    {
        name = "nutnet_tcp",
        brief = "TCP",
        requires = { "NET_IP", "NET_ICMP", "NUT_EVENT" },
        provides = { "NET_TCP", "DEV_READ", "DEV_WRITE" },
        sources = 
        { 
            "tcpin.c",
            "tcpout.c",
            "tcpsm.c",
            "tcpsock.c",
            "tcputil.c"
        },
        options = 
        {
            {
                macro = "NUT_THREAD_TCPSMSTACK",
                brief = "State Machine Stack",
                description = "Number of bytes to be allocated for the stack of the TCP state machine thread.",
                flavor = "booldata",
                file = "include/cfg/tcp.h"
            },
            {
                macro = "TCP_RETRIES_MAX",
                brief = "Max. Retransmissions",
                description = "Maximum number of retries before aborting a connection. "..
                              "Increase this value if connections are dropped prematurely. "..
                              "On reliable networks you may reduce this value to discover broken "..
                              "connections earlier.",
                default = "7",
                file = "include/cfg/tcp.h"
            },
            {
                macro = "TCP_RTTO_MIN",
                brief = "Min. Retransmission Time",
                description = "Lower bound of the retransmission timeout, given in milliseconds. "..
                              "Increase this value if connections are dropped prematurely or if "..
                              "retransmissions cause too much traffic."..
                              "On reliable networks you may reduce this value to recover from "..
                              "casual packet loss earlier.",
                default = "200",
                file = "include/cfg/tcp.h"
            },
            {
                macro = "TCP_RTTO_MAX",
                brief = "Max. Retransmission Time",
                description = "Lower bound of the retransmission timeout, given in milliseconds. "..
                              "On reliable networks you may reduce this value to discover broken "..
                              "connections earlier.",
                default = "20000",
                file = "include/cfg/tcp.h"
            },
            {
                macro = "TCP_COLLECT_INADV",
                brief = "Segment Collection",
                description = "TCP segments received in advance are stored in a special queue "..
                              " with complete headers. Due to this overhead, available memory may "..
                              "become exhausted when the remote sends many small segments after one "..
                              "segment got lost.\n\n"..
                              "When the number of segments received in advance reaches the given "..
                              "value, the stack will collect all consecutive small segments in the "..
                              "buffer, removing the headers from all collected segments.\n\n"..
                              "Increasing this value will increase memory requirements, but "..
                              "give better performance on unreliable connections.",
                default = "8",
                file = "include/cfg/tcp.h"
            },
            {
                macro = "TCP_COLLECT_SLIMIT",
                brief = "Segment Collection Limit",
                description = "When the number of segments received in advance reaches the value "..
                              "given by TCP_COLLECT_INADV, the stack will collect all consecutive "..
                              "small segments.\n\n"..
                              "This option specifies the limit, below which segments are conidered "..
                              "small. Segments equal or larger in size will not be collected.",
                default = "256",
                file = "include/cfg/tcp.h"
            },
            {
                macro = "TCP_BACKLOG_MAX",
                brief = "Backlog Buffer Size",
                description = "The backlog buffers incoming TCP connections for which no server "..
                              "thread is currently listening. It simply delays the reject in the "..
                              "hope that a thread will become available soon.\n\n"..
                              "Earlier versions of the stack immediately rejected incoming SYN "..
                              "segments if no socket was listening on the specific port. Setting "..
                              "this option to zero will keep this behaviour. Otherwise "..
                              "this parameter specifies the maximum number of connection "..
                              "attempts being stored in the backlog buffer.\n\n"..
                              "If this option is not set to zero, then NutTcpAccept() will now "..
                              "honor the TCP receive timeout, which may break existing applications.",
                default = "8",
                file = "include/cfg/tcp.h"
            },
            {
                macro = "TCP_BACKLOG_TIME",
                brief = "Max. Backlog Time",
                description = "In the current implementation there is no exact configuration for the "..
                              "maximum time that a connection attempt is kept in the backlog buffer. "..
                              "Multiplying this value by 200ms provides a rough approximation, but "..
                              "the timeout may be significantly shorter with many incoming packets.\n\n"..
                              "When the time elapses without any socket becoming ready to serve "..
                              "the connection, then the connection attempt is rejected.\n\n"..
                              "Furthermore, if the remote sends a second SYN packet for the same "..
                              "connection, then the previous one is removed from the backlog and "..
                              "the new attempt is rejected immediately.\n\n"..
                              "This option is ignored, if TCP_BACKLOG_MAX is zero.",
                default = "5",
                file = "include/cfg/tcp.h"
            }
        }
    },
    {
        name = "nutnet_udp",
        brief = "UDP",
        requires = { "NET_IP", "NUT_EVENT" },
        provides = { "NET_UDP" },
        sources = 
        { 
            "udpin.c",
            "udpout.c",
            "udpsock.c"
        },
        options = 
        {
            {
                macro = "NUT_UDP_ICMP_SUPPORT",
                brief = "ICMP support for UDP sockets",
                requires = { "NET_ICMP" },
                description = "Allows ICMP error reporting on UDP sockets. e.g. ICMP destination "..
                              "unreachable, ICMP host unreachable etc. will trigger errors on "..
                              "UDP send / receive functions.",
                flavor = "boolean",
                file = "include/cfg/udp.h"
            }
        }
        
    },
    {
        name = "nutnet_ip",
        brief = "IP",
        requires = { "NET_LINK" },
        provides = { "NET_IP" },
        sources = 
        { 
            "ipcsum.c",
            "ipin.c",
            "ipout.c",
            "ipdemux.c",
            "route.c"
        },
        options = 
        {
            {
                macro = "NUTIPCONF_ICMP_ARPMETHOD",
                brief = "Enable ARP Method",
                description = "Allow IP configuration using ARP method.\n\n"..
                              "When enabling this function, be aware that DHCP servers "..
                              "may probe an IP/MAC relationship by sending an ICMP request. "..
                              "This triggers the Nut/Net ARP method and terminates the DHCP "..
                              "client, leaving the system with default configurations of "..
                              "the network mask (255.255.255.0) and default gateway (none).",
                flavor = "booldata",
                file = "include/cfg/ip.h"
            },
        }        
    },
    {
        name = "nutnet_icmp",
        brief = "ICMP",
        requires = { "NET_LINK" },
        provides = { "NET_ICMP" },
        sources = 
        { 
            "icmpin.c",
            "icmpout.c"
        }
    },
    {
        name = "nutnet_igmp",
        brief = "IGMP",
        requires = { "NET_LINK" },
        provides = { "NET_IGMP" },
        sources = 
        { 
            "igmp.c",
            "igmpin.c",
            "igmpout.c"
        }
    },
    {
        name = "nutnet_inet",
        brief = "INET",
        description = "IP helper routines",
        sources = { "inet.c" }
    },
    {
        name = "nutnet_arp",
        brief = "ARP",
        description = "Address Resolution Protocol, translates a "..
                      "32-bit IP address into a 48-bit Ethernet address.",
        requires = { "NUT_EVENT", "NET_PHY" },
        provides = { "NET_ARP" },
        sources = 
        { 
            "arpcache.c",
            "arpin.c",
            "arpout.c"
        },
        options = 
        {
            {
                macro = "MAX_ARPAGE",
                brief = "Max. ARP Age",
                description = "Maximum age of an entry in the ARP cache in minutes. "..
                              "Default is 9 minutes.",
                flavor = "booldata",
                file = "include/cfg/arp.h"
            },
            {
                macro = "MAX_ARPREQUESTS",
                brief = "Max. ARP Requests",
                description = "Maximum number of ARP requests generated per query. "..
                              "Default is 4.",
                flavor = "booldata",
                file = "include/cfg/arp.h"
            },
            {
                macro = "MIN_ARPWAIT",
                brief = "Min. ARP Wait",
                description = "Minimum number of milliseconds to wait before sending "..
                              "out a new ARP request. Default is 500 milliseconds.",
                flavor = "booldata",
                file = "include/cfg/arp.h"
            },
        }
        
    },
    {
        name = "nutnet_ethernet",
        brief = "Ethernet",
        requires = { "NET_ARP" },
        provides = { "NET_LINK" },
        sources = 
        { 
            "ethin.c",
            "ethout.c",
            "ethdemux.c",
            "ether_addr.c"
        }
    },
    {
        name = "nutnet_ppp",
        brief = "PPP",
        requires = { 
            "NET_PPPAUTH", "NUT_EVENT", "PROTO_HDLC", "DEV_FILE", "DEV_READ", "DEV_WRITE" 
        },
        provides = { "NET_PPP", "NET_LINK" },
        sources = { 
            "pppin.c",
            "pppout.c",
            "pppsm.c",
            "ipcpin.c",
            "ipcpout.c",
            "lcpin.c", 
            "lcpout.c"
        },
        options = 
        {
            {
                macro = "NUT_THREAD_PPPSMSTACK",
                brief = "State Machine Stack",
                description = "Number of bytes to be allocated for the stack of the PPP state machine thread.",
                flavor = "booldata",
                file = "include/cfg/ppp.h"
            }
        }
    },
    {
        name = "nutnet_pap",
        brief = "PPP PAP",
        requires = { "NET_PPP" },
        provides = { "NET_PPPAUTH" },
        sources = { "papin.c", "papout.c" }
    },
    {
        name = "nutnet_ifconfig",
        brief = "Network interface",
        requires = { "NUT_EVENT", "NET_LINK" },
        sources = { "ifconfig.c" }
    },
    {
        name = "nutnet_conf",
        brief = "Network Configuration",
        description = "Initial network settings may be stored in non volatile memory "..
                      "or may be hardcoded.",
        provides = { "NET_PARMS" },
        sources = { "confnet.c" },
        options =
        {
            {
                macro = "CONFNET_NVMEM",
                brief = "Use Non-Volatile Memory",
                description = "Initial network settings are stored in non volatile memory."..
                              "The current version uses 10 bytes for validity check, 6 bytes "..
                              "for the MAC address and 16 bytes for the IP configuration.\n\n"..
                              "The length of the host name is configurable.\n\n"..
                              "This item is disabled if the system doesn't offer any "..
                              "non-volatile memory. Check the non-volatile memory"..
                              "module in the device driver section.",
                requires = { "DEV_NVMEM" },
                provides = { "CONFNET_INIT_IN_NVMEM" },
                flavor = "boolean",
                exclusivity = { "CONFNET_NVMEM", "CONFNET_HARDCODED" },
                file = "include/cfg/eeprom.h"
            },
            {
                macro = "CONFNET_HARDCODED",
                brief = "Use Hard-Coded Values",
                description = "Initial network settings are hard-coded. When changed, you need to "..
                              "re-compile the Nut/OS libraries.\n\n"..
                              "Applications may handle the network configuration in a very "..
                              "special way, which is not covered by the standard API. "..
                              "Using hard-coded values will be also useful as a temporary "..
                              "solution when porting Nut/Net to a new platform.",
                provides = { "CONFNET_INIT_IN_CODE" },
                flavor = "boolean",
                exclusivity = { "CONFNET_NVMEM", "CONFNET_HARDCODED" },
                file = "include/cfg/eeprom.h"
            },
            {
                macro = "CONFNET_EE_OFFSET",
                brief = "Location",
                description = "This is the non-volatile memory address offset, where Nut/Net "..
                              "expects its configuration.\n\n"..
                              "Note, that changing this value will invalidate previously "..
                              "stored setting after upgrading to this new version. You must "..
                              "also make sure, that this memory area will not conflict with "..
                              "others, specifically the system configuration.",
                requires = { "CONFNET_INIT_IN_NVMEM" },
                default = "64",
                type = "integer",
                file = "include/cfg/eeprom.h"
            },
            {
                macro = "CONFNET_VIRGIN_MAC",
                brief = "Initial MAC Address",
                description = "Ethernet MAC address. "..
                              "The given default is a locally administered address.",
                requires = { "CONFNET_INIT_IN_CODE" },
                default = "\"02:00:00:00:00:00\"",
                file = "include/cfg/eeprom.h"
            },
            {
                macro = "CONFNET_VIRGIN_IP",
                brief = "Initial IP Address",
                description = "When set to 0.0.0.0, Nut/Net will request an IP address from the DHCP "..
                              "server during network initialization.",
                requires = { "CONFNET_INIT_IN_CODE" },
                default = "\"0.0.0.0\"",
                file = "include/cfg/eeprom.h"
            },
            {
                macro = "CONFNET_VIRGIN_NETMASK",
                brief = "Initial IP Mask",
                description = "If the initial IP address is 0.0.0.0, this will be overridden by DHCP. ",
                requires = { "CONFNET_INIT_IN_CODE" },
                default = "\"255.255.255.0\"",
                file = "include/cfg/eeprom.h"
            },
            {
                macro = "CONFNET_VIRGIN_GATE",
                brief = "Initial IP Gate",
                description = "IP address of the default gateway, may be 0.0.0.0 if routing to other "..
                              "networks is not required. If the initial IP address is 0.0.0.0, this "..
                              "will be overridden by DHCP.",
                requires = { "CONFNET_INIT_IN_CODE" },
                default = "\"0.0.0.0\"",
                file = "include/cfg/eeprom.h"
            }
        }
    },
    {
        name = "nutnet_debug",
        brief = "Network debug",
        requires = { "DEV_WRITE", "NET_LINK" },
        sources = { "netdebug.c" }
    },
    {
        name = "nutnet_debug_ppp",
        brief = "PPP debug",
        requires = { "DEV_WRITE", "NET_PPP" },
        sources = { "pppdebug.c" }
    }
}
