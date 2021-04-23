#ifndef NETID_H
#define NETID_H

namespace jr
{
	struct NetID;
}


struct jr::NetID
{
	unsigned char id = 0, layer = 0; //255x255 for entities if desired
};

#endif //NETID_H