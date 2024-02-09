/**
 *  @file        imglist.cpp
 *  @description Contains partial implementation of ImgList class
 *               for CPSC 221 PA1
 *               Function bodies to be completed by student
 * 
 *  THIS FILE WILL BE SUBMITTED
 */

#include "imglist.h"

#include <math.h> // provides fmax, fmin, and fabs functions

/*********************
* CONSTRUCTORS, ETC. *
*********************/

/**
 * Default constructor. Makes an empty list
 */
ImgList::ImgList() {
    // set appropriate values for all member attributes here
	northwest = NULL;
    southeast = NULL;
}

/**
 * Creates a list from image data
 * @pre img has dimensions of at least 1x1
 */
ImgList::ImgList(PNG& img) {
    // build the linked node structure and set the member attributes appropriately

    ImgNode* prev;
    ImgNode* above;
    ImgNode* firstInRow;
	for (unsigned y = 0; y < img.height(); y++) {
        for (unsigned x = 0; x < img.width(); x++) {
            RGBAPixel* pixel = img.getPixel(x, y);
            ImgNode* n = new ImgNode();
            n->colour = *pixel;

            if (x == 0 && y == 0) {
                northwest = n;
                above = n;
            }

            if ((x == img.width()-1) && (y == img.height()-1)) {
                southeast = n;
            }

            if (x == 0) {
                prev = n;
                firstInRow = n;                 
            } 
            
            // set west/east pointers 
            if (x > 0) {
                prev->east = n;
                n->west = prev;
                prev = prev->east;
            }

            // set north/south pointers
            if (y > 0) {
                above->south = n;
                n->north = above;
                if (above->east != NULL) {
                    above = above->east;
                } else {
                    above = firstInRow;
                }
            }
        }
    }
}

/************
* ACCESSORS *
************/

/**
 * Returns the horizontal dimension of this list (counted in nodes)
 * Note that every row will contain the same number of nodes, whether or not
 *   the list has been carved.
 * We expect your solution to take linear time in the number of nodes in the
 *   x dimension.
 */
unsigned int ImgList::GetDimensionX() const {
    // replace the following line with your implementation    
    ImgNode* n = northwest;
    int dim = 0;
    while (n->east != NULL) {
        dim = dim + 1; //+ n->skipright;           // add current node + any skipped      Note: I changed this bc render needs a getter without skips
        n = n->east;
    }

    // last node in the row
    dim = dim + 1; //+ n->skipright;
    delete n;
    n = NULL;
    return dim;
}

/**
 * Returns the vertical dimension of the list (counted in nodes)
 * It is useful to know/assume that the grid will never have nodes removed
 *   from the first or last columns. The returned value will thus correspond
 *   to the height of the PNG image from which this list was constructed.
 * We expect your solution to take linear time in the number of nodes in the
 *   y dimension.
 */
unsigned int ImgList::GetDimensionY() const {
    // replace the following line with your implementation
    ImgNode* n = northwest;
    int dim = 0;
    while (n->south != NULL) {
        dim += 1;
        n = n->south;
    }
    return dim + 1;
}

/**
 * Returns the horizontal dimension of the list (counted in original pixels, pre-carving)
 * The returned value will thus correspond to the width of the PNG image from
 *   which this list was constructed.
 * We expect your solution to take linear time in the number of nodes in the
 *   x dimension.
 */
unsigned int ImgList::GetDimensionFullX() const {
    // replace the following line with your implementation
    ImgNode* n = northwest;
    int dim = 0;
    while (n->east != NULL) {
        dim = dim + 1 + n->skipright;           // add current node + any skipped
        n = n->east;
    }

    // last node in the row
    dim = dim + 1 + n->skipright;
    return dim;

}

/**
 * Returns a pointer to the node which best satisfies the specified selection criteria.
 * The first and last nodes in the row cannot be returned.
 * @pre rowstart points to a row with at least 3 physical nodes
 * @pre selectionmode is an integer in the range [0,1]
 * @param rowstart - pointer to the first node in a row
 * @param selectionmode - criterion used for choosing the node to return
 *          0: minimum "brightness" across row, not including extreme left or right nodes
 *          1: node with minimum total of "colour difference" with its left neighbour and with its right neighbour.
 *        In the (likely) case of multiple candidates that best match the criterion,
 *        the left-most node satisfying the criterion (excluding the row's starting node)
 *        will be returned.
 * A note about "brightness" and "colour difference":
 * For PA1, "brightness" will be the sum over the RGB colour channels, multiplied by alpha.
 * "colour difference" between two pixels can be determined
 * using the "distanceTo" function found in RGBAPixel.h.
 */
ImgNode* ImgList::SelectNode(ImgNode* rowstart, int selectionmode) {
    double minSoFar;
    double brightness;
    double dist;

    rowstart = rowstart->east;
    ImgNode* min = rowstart;

    if (selectionmode == 0) { //assume exclude first and last nodes

        minSoFar = ((rowstart->colour.r + rowstart->colour.g + rowstart->colour.b) * rowstart->colour.a);
        rowstart = rowstart->east;

        while (rowstart->east != NULL) {

            brightness = ((rowstart->colour.r + rowstart->colour.g + rowstart->colour.b) * rowstart->colour.a);

            if (brightness < minSoFar) {
                minSoFar = brightness;
                min = rowstart;

            }

            rowstart = rowstart->east;

        }

    } else if (selectionmode == 1) {

        minSoFar = rowstart->colour.distanceTo(rowstart->east->colour) + rowstart->colour.distanceTo(rowstart->west->colour);
        rowstart = rowstart->east;

        while (rowstart->east != NULL) {
            dist = rowstart->colour.distanceTo(rowstart->east->colour) + rowstart->colour.distanceTo(rowstart->west->colour);

            if (dist < minSoFar) {
                minSoFar = dist;
                min = rowstart;
            }

            rowstart = rowstart->east;

        }
    }
  
    return min;
}

/**
 * Renders this list's pixel data to a PNG, with or without filling gaps caused by carving.
 * @pre fillmode is an integer in the range of [0,2]
 * @param fillgaps - whether or not to fill gaps caused by carving
 *          false: render one pixel per node, ignores fillmode
 *          true: render the full width of the original image,
 *                filling in missing nodes using fillmode
 * @param fillmode - specifies how to fill gaps
 *          0: solid, uses the same colour as the node at the left of the gap
 *          1: solid, using the averaged values (all channels) of the nodes at the left and right of the gap
 *          2: linear gradient between the colour (all channels) of the nodes at the left and right of the gap
 *             e.g. a gap of width 1 will be coloured with 1/2 of the difference between the left and right nodes
 *             a gap of width 2 will be coloured with 1/3 and 2/3 of the difference
 *             a gap of width 3 will be coloured with 1/4, 2/4, 3/4 of the difference, etc.
 *             Like fillmode 1, use the smaller difference interval for hue,
 *             and the smaller-valued average for diametric hues
 */
PNG ImgList::Render(bool fillgaps, int fillmode) const {
<<<<<<< HEAD

    PNG outpng;                 // return png
    ImgNode* n = northwest;
    ImgNode* rowStart = northwest;
    unsigned gap = 0;
    RGBAPixel* px;

    if (fillgaps == false) {
        outpng.resize(GetDimensionX(), GetDimensionY());
        for (unsigned int h = 0; h < GetDimensionY(); h++) {
            for (unsigned int w = 0; w < GetDimensionX(); w++) {
                *outpng.getPixel(w, h) = n->colour;
                n = n->east;
            }
            rowStart = rowStart->south;
            n = rowStart;
        }

    } else {
        outpng.resize(GetDimensionFullX(), GetDimensionY());
        for (unsigned int h = 0; h < GetDimensionY(); h++) {
            for (unsigned int w = 0; w < GetDimensionFullX(); w++) {
                *outpng.getPixel(w, h) = n->colour; 

                if (fillmode == 0) {    // same color as node at left of gap
                    if (n->skipright != 0) {                                   
                        for (unsigned i = w + 1; i <= w + n->skipright; i++) {
                            *outpng.getPixel(i, h) = n->colour;
                            gap++;
                        }
                    }
                } 

                if (fillmode == 1) {    // avg of left and right node channels
                    if (n->skipright != 0) {                                   
                        for (unsigned i = w + 1; i <= w + n->skipright; i++) {  
                            px = outpng.getPixel(i, h);                         
                            px->r = ((n->colour.r + n->east->colour.r)/2);
                            px->g = ((n->colour.g + n->east->colour.g)/2);
                            px->b = ((n->colour.b + n->east->colour.b)/2);
                            gap++;
                        }
                    }
                }

                if (fillmode == 2) {    // linear gradient between left and right node channels
                    if (n->skipright != 0) {                                    
                        
                        for (unsigned i = w + 1; i <= w + n->skipright; i++) {       
                            double fraction = (double) (i + 1) / (n->skipright + 1);
                            px = outpng.getPixel(i, h);

                            px->r = n->colour.r + fraction * (n->east->colour.r - n->colour.r);
                            px->g = n->colour.g + fraction * (n->east->colour.g - n->colour.g);
                            px->b = n->colour.b + fraction * (n->east->colour.b - n->colour.b);
                            px->a = n->colour.a + fraction * (n->east->colour.a - n->colour.a);
                            gap++;
                        }
                    }
                }

                w += gap;
                gap = 0;
                n = n->east;
            }
            rowStart = rowStart->south;
            n = rowStart;
        }
    }

=======
    // Add/complete your implementation below
    PNG outpng;
    unsigned x = 0;
    unsigned y = 0;
    ImgNode* n = northwest;
    RGBAPixel* px;

    if (fillgaps == false) {
        outpng.resize(this->GetDimensionX(), this->GetDimensionY());
    } else {
        outpng.resize(this->GetDimensionFullX(), this->GetDimensionY());
    }

    for (ImgNode* n = northwest; n != NULL; n = n->south) {
            
        while (n != NULL) {

            if (fillgaps == false) {
            *outpng.getPixel(x, y) = n->colour;
            n = n->east;
            x++;
            } else {

                if (fillmode == 0) {
                    *outpng.getPixel(x, y) = n->colour;
                    if (n->skipright != 0) {
                        for (int i = x + 1; i <= x + n->skipright; i++) {
                            *outpng.getPixel(i, y) = n->colour;
                        }
                    }

                    n = n->east;
                    x += n->skipright;

                } else if (fillmode == 1) {

                    *outpng.getPixel(x, y) = n->colour;

                    if (n->skipright != 0) {
                        for (int i = x + 1; i <= x + n->skipright; i++) {

                            px = outpng.getPixel(i, y);

                            px->r = ((n->colour.r + n->east->colour.r)/2);
                            px->g = ((n->colour.g + n->east->colour.g)/2);
                            px->b = ((n->colour.b + n->east->colour.b)/2);
                              
                        }

                        n = n->east;
                        x += n->skipright + 1;

                    }

                   
                    n = n->east;
                    x += n->skipright + 1;

                } else if (fillmode == 2) {

                    *outpng.getPixel(x, y) = n->colour;
                    if (n->skipright != 0) {
                        for (int i = x + 1; i <= x + n->skipright; i++) {

                            //TODO : help
                        }
                    }

                    n = n->east;
                    x += n->skipright + 1;

                }

            }

        }

        x=0;
        y++;
    }
  
>>>>>>> 3103b32775899d5cf26aefddd843b8924aa38f04
    return outpng;
}
  

/************
* MODIFIERS *
************/

/**
 * Removes exactly one node from each row in this list, according to specified criteria.
 * The first and last nodes in any row cannot be carved.
 * @pre this list has at least 3 nodes in each row
 * @pre selectionmode is an integer in the range [0,1]
 * @param selectionmode - see the documentation for the SelectNode function.
 * @param this list has had one node removed from each row. Neighbours of the created
 *       gaps are linked appropriately, and their skip values are updated to reflect
 *       the size of the gap.
 * 
 *  *  selectionmode - criterion used for choosing the node to return
 *          0: minimum "brightness" across row, not including extreme left or right nodes
 *          1: node with minimum total of "colour difference" with its left neighbour and with its right neighbour.
 *        In the (likely) case of multiple candidates that best match the criterion,
 *        the left-most node satisfying the criterion (excluding the row's starting node)
 *        will be returned.
 */
<<<<<<< HEAD
void ImgList::Carve(int selectionmode) { 
    
    ImgNode* n = northwest;

    for (unsigned int i = 0; i < GetDimensionY(); i++) {
        ImgNode* r = SelectNode(n, selectionmode);
        ImgNode* rl = r->west; 
        ImgNode* rr = r->east; 

        if (!(i == 0 || i == GetDimensionY() - 1)) {
            ImgNode * ru = r->north; 
            ImgNode * rd = r->south;

            ru->skipdown++; 
            rd->skipup++; 

            rd->north = ru; 
            ru->south = rd; 
        }

        rl->skipright++; 
        rr->skipleft++; 

        rl->east = rr; 
        rr->west = rl; 
        delete r; 
        r = NULL;

        n = n->south; 
    }
}
=======
void ImgList::Carve(int selectionmode) {
    ImgNode* n = northwest;
    ImgNode* selected;

    while (n->south != NULL) {
        selected = SelectNode(n, selectionmode);
        selected->west->east = selected->east;
        selected->west->skipright++;
        selected->east->west = selected->west;
        selected->west->skipleft++;
        delete selected;
        selected = NULL;

        n = n->south;
        }
        
        selected = SelectNode(n, selectionmode);
        selected->west->east = selected->east;
        selected->west->skipright++;
        selected->east->west = selected->west;
        selected->west->skipleft++;
        delete selected;
        selected = NULL;

    }

>>>>>>> 3103b32775899d5cf26aefddd843b8924aa38f04



// void ImgList::Carve(int selectionmode) {

//     ImgNode* rowStart = northwest;                                  // might be breaking rule
//     while (rowStart != NULL) {
//         ImgNode* selected = SelectNode(rowStart, selectionmode);    // selected = node chosen for removal

//         selected->west->east = selected->east;                      // update west node
//         selected->west->skipright += selected->skipright + 1;

//         selected->east->west = selected->west;                      // update east node
//         selected->east->skipleft += selected->skipleft + 1; 

//         if (selected->north != NULL) {                              // not in first row
//             selected->north->south = selected->south;               // update north node
//             selected->north->skipdown += selected->skipdown + 1;
//         } 

//         if (selected->south != NULL) {                              // not in last row
//             selected->south->north = selected->north;               // update south node
//             selected->south->skipup += selected->skipup + 1;       
//         }

//         delete selected;
//         selected = NULL;
//         rowStart = rowStart->south;
//     }
    
// }

// note that a node on the boundary will never be selected for removal
/**
 * Removes "rounds" number of nodes (up to a maximum of node width - 2) from each row,
 * based on specific selection criteria.
 * Note that this should remove one node from every row, repeated "rounds" times,
 * and NOT remove "rounds" nodes from one row before processing the next row.
 * @pre selectionmode is an integer in the range [0,1]
 * @param rounds - number of nodes to remove from each row
 *        If rounds exceeds node width - 2, then remove only node width - 2 nodes from each row.
 *        i.e. Ensure that the final list has at least two nodes in each row.
 * @post this list has had "rounds" nodes removed from each row. Neighbours of the created
 *       gaps are linked appropriately, and their skip values are updated to reflect
 *       the size of the gap.
 * 

 */
void ImgList::Carve(unsigned int rounds, int selectionmode) {
    if (rounds > (this->GetDimensionX() -2)) {

        for (unsigned x = 0; x < (this->GetDimensionX() -2); x++) {
            this->Carve(selectionmode);
        }
    } else {
        for (unsigned x = 0; x < rounds; x++) {
            this->Carve(selectionmode);
        }
    }
	
}


/*
 * Helper function deallocates all heap memory associated with this list,
 * puts this list into an "empty" state. Don't forget to set your member attributes!
 * @post this list has no currently allocated nor leaking heap memory,
 *       member attributes have values consistent with an empty list.
 */
void ImgList::Clear() {
    // add your implementation here
	
}

/**
 * Helper function copies the contents of otherlist and sets this list's attributes appropriately
 * @pre this list is empty
 * @param otherlist - list whose contents will be copied
 * @post this list has contents copied from by physically separate from otherlist
 */
void ImgList::Copy(const ImgList& otherlist) {
    // add your implementation here
	
}

/*************************************************************************************************
* IF YOU DEFINED YOUR OWN PRIVATE FUNCTIONS IN imglist-private.h, YOU MAY ADD YOUR IMPLEMENTATIONS BELOW *
*************************************************************************************************/

