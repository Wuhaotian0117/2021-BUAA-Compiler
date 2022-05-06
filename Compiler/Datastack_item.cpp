//
// Created by wuhaotian on 2021/11/17.
//

#include "Datastack_item.h"

Datastack_item::Datastack_item(string fitemname, int fd1, int fd2, int fdimension, int flevel) {
    this->itemname = fitemname;
    this->d1 = fd1;
    this->d2 = fd2;
    this->dimension = fdimension;
    this->level = flevel;
}

Datastack_item::Datastack_item() {

}