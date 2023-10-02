/*
Copyright (C) 2003, 2010 - Wolfire Games
Copyright (C) 2010-2017 - Lugaru contributors (see AUTHORS file)

This file is part of Lugaru.

Lugaru is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Lugaru is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lugaru.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _JOINT_HPP_
#define _JOINT_HPP_

#include "Math/Vector3.hpp"

#include <vector>

enum bodypart
{
    head,
    neck,
    leftshoulder,
    leftelbow,
    leftwrist,
    lefthand,
    rightshoulder,
    rightelbow,
    rightwrist,
    righthand,
    abdomen,
    lefthip,
    righthip,
    groin,
    leftknee,
    leftankle,
    leftfoot,
    rightknee,
    rightankle,
    rightfoot
};

class Joint
{
public:
    Vector3 position;
    Vector3 oldposition;
    Vector3 realoldposition;
    Vector3 velocity;
    Vector3 oldvelocity;
    Vector3 startpos;
    float blurred = 0.f;
    float length = 0.f;
    float mass = 0.f;
    bool lower = false;
    bool hasparent = false;
    bool locked = false;
    int modelnum = 0;
    bool visible = false;
    Joint* parent = nullptr;
    bool sametwist = false;
    bodypart label;
    int hasgun =  0;
    float delay = 0.f;
    Vector3 velchange;

    Joint();
    void load(FILE* tfile, std::vector<Joint>& joints);
};

#endif
