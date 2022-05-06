//
// Created by wuhaotian on 2021/11/17.
//

#ifndef HOMEWORK5_DATASTACK_ITEM_H
#define HOMEWORK5_DATASTACK_ITEM_H

#include <string>
#include "TypeValue.h"

using namespace std;

class Datastack_item {
public:

    Datastack_item();

    Datastack_item(string fitemname, int fd1, int fd2, int fdimension, int flevel);

    string itemname;
    ItemType itemType;
    int d1;
    int d2;
    int dimension;
    int level;
    int addr;
};


#endif //HOMEWORK5_DATASTACK_ITEM_H
