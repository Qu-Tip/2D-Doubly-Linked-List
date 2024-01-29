/**
 *  @file        imglist-given.cpp
 *  @description Contains partial implementation of ImgList class
 *               for CPSC 221 PA1
 *               Instructor-implemented functions
 *
 *  THIS FILE WILL NOT BE SUBMITTED
 */

#include "imglist.h"

 /**
  * Copy constructor.
  * Creates this this to become a separate copy of the data in otherlist
  */
ImgList::ImgList(const ImgList& otherlist) {
    // build the linked node structure using otherlist as a template
    Copy(otherlist);
}

/**
 * Assignment operator. Enables statements such as list1 = list2;
 *   where list1 and list2 are both variables of ImgList type.
 * @post the contents of this list will be a physically separate copy of rhs
 */
ImgList& ImgList::operator=(const ImgList& rhs) {
    // Re-build any existing structure using rhs as a template

    if (this != &rhs) { // if this list and rhs are different lists in memory
        // release all existing heap memory of this list
        Clear();

        // and then rebuild this list using rhs as a template
        Copy(rhs);
    }

    return *this;
}

/**
 * Destructor.
 * Releases any heap memory associated with this list.
 */
ImgList::~ImgList() {
    // Ensure that any existing heap memory is deallocated
    Clear();
}