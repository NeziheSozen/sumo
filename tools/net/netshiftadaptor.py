#!/usr/bin/env python
"""
@file    netshiftadaptor.py
@author  Daniel.Krajzewicz@dlr.de
@date    2008-09-01
@version $Id$


This class performs a network reprojection
 using barycentric cordinates of two triangles
 which share the same nodes of two networks.

This means: the class is initialised with two
 networks and two lists of node ids (should be
 exactly three). The according nodes should be 
 the "same" nodes in both networks.
When "reproject" is called, all nodes' position
 of the second network are reprojected so that
 they match positions within the first network.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""




import os, string, sys
sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet


class NetShiftAdaptor:
    def __init__(self, net1, net2, nodes1, nodes2):
        self._net1 = net1
        self._net2 = net2
        self._nodes1 = nodes1
        self._nodes2 = nodes2
        if len(nodes1)!=3 or len(nodes2)!=3:
            raise "Both node lists must contain exactly 3 node ids"

    def reproject(self, verbose=False):
        x11 = self._net1._id2node[self._nodes1[0]]._coord[0];
        y11 = self._net1._id2node[self._nodes1[0]]._coord[1];
        x12 = self._net1._id2node[self._nodes1[1]]._coord[0];
        y12 = self._net1._id2node[self._nodes1[1]]._coord[1];
        x13 = self._net1._id2node[self._nodes1[2]]._coord[0];
        y13 = self._net1._id2node[self._nodes1[2]]._coord[1];
        x21 = self._net2._id2node[self._nodes2[0]]._coord[0];
        y21 = self._net2._id2node[self._nodes2[0]]._coord[1];
        x22 = self._net2._id2node[self._nodes2[1]]._coord[0];
        y22 = self._net2._id2node[self._nodes2[1]]._coord[1];
        x23 = self._net2._id2node[self._nodes2[2]]._coord[0];
        y23 = self._net2._id2node[self._nodes2[2]]._coord[1];
        for n in self._net2._nodes:
            x0 = n._coord[0]
            y0 = n._coord[1]
            b0 =  (x22 - x21) * (y23 - y21) - (x23 - x21) * (y22 - y21);
            b1 = ((x22 - x0)  * (y23 - y0)  - (x23 - x0)  * (y22 - y0)) / b0; 
            b2 = ((x23 - x0)  * (y21 - y0)  - (x21 - x0)  * (y23 - y0)) / b0;
            b3 = ((x21 - x0)  * (y22 - y0)  - (x22 - x0)  * (y21 - y0)) / b0;		
            x = (b1 * x11 + b2 * x12 + b3 * x13);
            y = (b1 * y11 + b2 * y12 + b3 * y13);
            n._coord[0] = x
            n._coord[1] = y
