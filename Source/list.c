/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include <ifaddrs.h>
#include "list.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

static void list_networkInterfaceInformationHelper(const struct ifaddrs * const Address);

/****************************************************************************************************
 * Function Definition (Public)
 ****************************************************************************************************/

/*** List Network Interface Information ***/
void list_networkInterfaceInformation(void)
{
    /*** List Network Interface Information ***/
    /* Variable */
    struct ifaddrs *addresses;

    /* Get Addresses */
    if(getifaddrs(&addresses) != 0)
    {
        perror("getifaddrs");
        return;
    }
    
    /* List Network Interface Information */
    (void)printf("Name\tFamily\tIP Address\n");
    for(struct ifaddrs *address = addresses; address != NULL; address = address->ifa_next)
        list_networkInterfaceInformationHelper(address);
    
    /* Clean Up */
    freeifaddrs(addresses);
}

/****************************************************************************************************
 * Function Definition (Private)
 ****************************************************************************************************/

/*** List Network Interface Information Helper ***/
static void list_networkInterfaceInformationHelper(const struct ifaddrs * const Address)
{
    /*** List Network Interface Information Helper ***/
    /* Error Check */
    if(Address->ifa_addr)
    {
        /* Variable */
        socklen_t addressLength;
        const char *Family;
        char ipAddress[NI_MAXHOST];
        
        /* Set Up */
        switch(Address->ifa_addr->sa_family)
        {
            case AF_INET:
                addressLength = sizeof(struct sockaddr_in);
                Family = "IPv4";
                break;
            case AF_INET6:
                addressLength = sizeof(struct sockaddr_in6);
                Family = "IPv6";
                break;
            default:
                return;
        }
        if(getnameinfo(Address->ifa_addr, addressLength, ipAddress, sizeof(ipAddress), NULL, 0, NI_NUMERICHOST) != 0)
        {
            perror("getnameinfo");
            return;
        }
        
        /* Print Information */
        (void)printf("%s\t%s\t%s\n", Address->ifa_name, Family, ipAddress);
    }
}
