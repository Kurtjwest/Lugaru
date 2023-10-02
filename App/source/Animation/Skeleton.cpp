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

#include "Animation/Skeleton.hpp"

#include "Objects/Object.hpp"
#include "Audio/Sounds.hpp"
#include "Animation/Animation.hpp"

#include "Utils/Folders.hpp"

extern float multiplier;
extern float gravity;
extern Terrain terrain;
extern int environment;
extern float camerashake;
extern bool freeze;
extern int detail;

extern int whichjointstartarray[26];
extern int whichjointendarray[26];

Skeleton::Skeleton()
	: selected(0)
	, id(0)
	, num_models(0)
	, clothes(false)
	, spinny(false)
	, skinsize(0)
	, checkdelay(0)
	, longdead(0)
	, broken(false)
	, free(0)
	, oldfree(0)
	, freetime(0)
	, freefall(false)
{
	memset(forwardjoints, 0, sizeof(forwardjoints));
	memset(lowforwardjoints, 0, sizeof(lowforwardjoints));
	memset(jointlabels, 0, sizeof(jointlabels));
	memset(skinText, 0, sizeof(skinText));
}

/* EFFECT
 * sets forward, lowforward, specialforward[]
 *
 * USES:
 * Skeleton::Load
 * Person/Person::DoAnimations
 * Person/Person::DrawSkeleton
 */
void Skeleton::FindForwards()
{
	//Find forward vectors
	CrossProduct(joints[forwardjoints[1]].position - joints[forwardjoints[0]].position, joints[forwardjoints[2]].position - joints[forwardjoints[0]].position, &forward);
	Normalise(&forward);

	CrossProduct(joints[lowforwardjoints[1]].position - joints[lowforwardjoints[0]].position, joints[lowforwardjoints[2]].position - joints[lowforwardjoints[0]].position, &lowforward);
	Normalise(&lowforward);

	//Special forwards
	specialforward[0] = forward;

	specialforward[1] = jointPos(rightshoulder) + jointPos(rightwrist);
	specialforward[1] = jointPos(rightelbow) - specialforward[1] / 2.f;
	specialforward[1] += forward * .4f;
	Normalise(&specialforward[1]);
	specialforward[2] = jointPos(leftshoulder) + jointPos(leftwrist);
	specialforward[2] = jointPos(leftelbow) - specialforward[2] / 2.f;
	specialforward[2] += forward * .4f;
	Normalise(&specialforward[2]);

	specialforward[3] = jointPos(righthip) + jointPos(rightankle);
	specialforward[3] = specialforward[3] / 2.f - jointPos(rightknee);
	specialforward[3] += lowforward * .4f;
	Normalise(&specialforward[3]);
	specialforward[4] = jointPos(lefthip) + jointPos(leftankle);
	specialforward[4] = specialforward[4] / 2.f - jointPos(leftknee);
	specialforward[4] += lowforward * .4f;
	Normalise(&specialforward[4]);
}

/* EFFECT
 * TODO
 *
 * USES:
 * Person/Person::RagDoll
 * Person/Person::DoStuff
 * Person/IKHelper
 * 
 * Tutorial::active
 */
float Skeleton::DoConstraints(Vector3* coords, float* scale, bool tutorialActive, bool bloodtoggle)
{
	const float elasticity = .3f;
	Vector3 bounceness;
	const int numrepeats = 3;
	float groundlevel = .15f;
	unsigned i = 0;
	Vector3 temp;
	Vector3 terrainnormal;
	int whichhit = 0;
	float frictionness = 0.f;
	Vector3 terrainlight;
	int whichpatchx = 0;
	int whichpatchz = 0;
	float damage = 0; // eventually returned from function
	bool breaking = false;

	if (free) {
		freetime += multiplier;

		whichpatchx = (int)(coords->x / (terrain.size / subdivision * terrain.scale));
		whichpatchz = (int)(coords->z / (terrain.size / subdivision * terrain.scale));

		terrainlight = *coords;
		Object::SphereCheckPossible(&terrainlight, 1);

		//Add velocity
		for (i = 0; i < joints.size(); i++) {
			joints[i].position = joints[i].position + joints[i].velocity * multiplier;

			switch (joints[i].label) {
			case head:
				groundlevel = .8f;
				break;
			case righthand:
			case rightwrist:
			case rightelbow:
			case lefthand:
			case leftwrist:
			case leftelbow:
				groundlevel = .2f;
				break;
			default:
				groundlevel = .15f;
				break;
			}

			joints[i].position.y -= groundlevel;
			joints[i].oldvelocity = joints[i].velocity;
		}

		float tempmult = multiplier;
		//multiplier/=numrepeats;

		for (int j = 0; j < numrepeats; j++) {
			float r = .05f;
			// right leg constraints?
			if (!joint(rightknee).locked && !joint(righthip).locked) {
				temp = jointPos(rightknee) - (jointPos(righthip) + jointPos(rightankle)) / 2;
				while (normaldotproduct(temp, lowforward) > -.1 && !sphere_line_intersection(&jointPos(righthip), &jointPos(rightankle), &jointPos(rightknee), &r)) {
					jointPos(rightknee) -= lowforward * .05f;
					if (spinny) {
						jointVel(rightknee) -= lowforward * .05f / multiplier / 4.f;
					}
					else {
						jointVel(rightknee) -= lowforward * .05f;
					}
					jointPos(rightankle) += lowforward * .025f;
					if (spinny) {
						jointVel(rightankle) += lowforward * .025f / multiplier / 4.f;
					}
					else {
						jointVel(rightankle) += lowforward * .25f;
					}
					jointPos(righthip) += lowforward * .025f;
					if (spinny) {
						jointVel(righthip) += lowforward * .025f / multiplier / 4.f;
					}
					else {
						jointVel(righthip) += lowforward * .025f;
					}
					temp = jointPos(rightknee) - (jointPos(righthip) + jointPos(rightankle)) / 2.f;
				}
			}

			// left leg constraints?
			if (!joint(leftknee).locked && !joint(lefthip).locked) {
				temp = jointPos(leftknee) - (jointPos(lefthip) + jointPos(leftankle)) / 2;
				while (normaldotproduct(temp, lowforward) > -.1 && !sphere_line_intersection(&jointPos(lefthip), &jointPos(leftankle), &jointPos(leftknee), &r)) {
					jointPos(leftknee) -= lowforward * .05f;
					if (spinny) {
						jointVel(leftknee) -= lowforward * .05f / multiplier / 4.f;
					}
					else {
						jointVel(leftknee) -= lowforward * .05f;
					}
					jointPos(leftankle) += lowforward * .025f;
					if (spinny) {
						jointVel(leftankle) += lowforward * .025f / multiplier / 4.f;
					}
					else {
						jointVel(leftankle) += lowforward * .25f;
					}
					jointPos(lefthip) += lowforward * .025f;
					if (spinny) {
						jointVel(lefthip) += lowforward * .025f / multiplier / 4.f;
					}
					else {
						jointVel(lefthip) += lowforward * .025f;
					}
					temp = jointPos(leftknee) - (jointPos(lefthip) + jointPos(leftankle)) / 2.f;
				}
			}

			for (i = 0; i < joints.size(); i++) {
				if (joints[i].locked && !spinny && findLengthfast(&joints[i].velocity) > 320.f) {
					joints[i].locked = 0;
				}
				if (spinny && findLengthfast(&joints[i].velocity) > 600.f) {
					joints[i].locked = 0;
				}
				if (joints[i].delay > 0) {
					bool freely = true;
					for (unsigned j = 0; j < joints.size(); j++) {
						if (joints[j].locked) {
							freely = false;
						}
					}
					if (freely) {
						joints[i].delay -= multiplier * 3.f;
					}
				}
			}

			for (i = 0; i < muscles.size(); i++) {
				//Length constraints
				muscles[i].DoConstraint(spinny);
			}

			float friction;
			for (i = 0; i < joints.size(); i++) {
				//Length constraints
				//Ground constraint
				groundlevel = 0;
				if (joints[i].position.y * (*scale) + coords->y < terrain.getHeight(joints[i].position.x * (*scale) + coords->x, joints[i].position.z * (*scale) + coords->z) + groundlevel) {
					freefall = 0;
					friction = 1.5;
					if (joints[i].label == groin && !joints[i].locked && joints[i].delay <= 0) {
						joints[i].locked = 1;
						joints[i].delay = 1;
						if (!tutorialActive || id == 0) {
							emit_sound_at(landsound1, joints[i].position * (*scale) + *coords, 128.f);
						}
						breaking = true;
					}

					if (joints[i].label == head && !joints[i].locked && joints[i].delay <= 0) {
						joints[i].locked = 1;
						joints[i].delay = 1;
						if (!tutorialActive || id == 0) {
							emit_sound_at(landsound2, joints[i].position * (*scale) + *coords, 128.f);
						}
					}

					terrainnormal = terrain.getNormal(joints[i].position.x * (*scale) + coords->x, joints[i].position.z * (*scale) + coords->z);
					ReflectVector(&joints[i].velocity, &terrainnormal);
					bounceness = terrainnormal * findLength(&joints[i].velocity) * (abs(normaldotproduct(joints[i].velocity, terrainnormal)));
					if (!joints[i].locked) {
						damage += findLengthfast(&bounceness) / 4000;
					}
					if (findLengthfast(&joints[i].velocity) < findLengthfast(&bounceness)) {
						bounceness = 0;
					}
					frictionness = abs(normaldotproduct(joints[i].velocity, terrainnormal));
					joints[i].velocity -= bounceness;
					if (1 - friction * frictionness > 0) {
						joints[i].velocity *= 1 - friction * frictionness;
					}
					else {
						joints[i].velocity = 0;
					}

					if (!tutorialActive || id == 0) {
						if (findLengthfast(&bounceness) > 8000 && breaking) {
							// FIXME: this crashes because k is not initialized!
							// to reproduce, type 'wolfie' in console and play a while
							// I'll just comment it out for now
							//Object::objects[k]->model.MakeDecal(breakdecal, DoRotation(temp - Object::objects[k]->position, 0, -Object::objects[k]->yaw, 0), .4, .5, rand() % 360);
							Sprite::MakeSprite(cloudsprite, joints[i].position * (*scale) + *coords, joints[i].velocity * .06f, 1, 1, 1, 4, .2f, bloodtoggle);
							breaking = false;
							camerashake += .6f;

							emit_sound_at(breaksound2, joints[i].position * (*scale) + *coords);

							addEnvSound(*coords, 64);
						}
					}

					if (findLengthfast(&bounceness) > 2500) {
						Normalise(&bounceness);
						bounceness = bounceness * 50;
					}

					joints[i].velocity += bounceness * elasticity;

					if (findLengthfast(&joints[i].velocity) > findLengthfast(&joints[i].oldvelocity)) {
						bounceness = 0;
						joints[i].velocity = joints[i].oldvelocity;
					}

					if (joints[i].locked == 0) {
						if (findLengthfast(&joints[i].velocity) < 1) {
							joints[i].locked = 1;
						}
					}

					if (environment == snowyenvironment && findLengthfast(&bounceness) > 500 && terrain.getOpacity(joints[i].position.x * (*scale) + coords->x, joints[i].position.z * (*scale) + coords->z) < .2f) {
						terrainlight = terrain.getLighting(joints[i].position.x * (*scale) + coords->x, joints[i].position.z * (*scale) + coords->z);
						Sprite::MakeSprite(cloudsprite, joints[i].position * (*scale) + *coords, joints[i].velocity * .06f, terrainlight.x, terrainlight.y, terrainlight.z, .5f, .7f, bloodtoggle);
						if (detail == 2) {
							terrain.MakeDecal(bodyprintdecal, joints[i].position * (*scale) + *coords, .4f, .4f, 0, environment);
						}
					}
					else if (environment == desertenvironment && findLengthfast(&bounceness) > 500 && terrain.getOpacity(joints[i].position.x * (*scale) + coords->x, joints[i].position.z * (*scale) + coords->z) < .2f) {
						terrainlight = terrain.getLighting(joints[i].position.x * (*scale) + coords->x, joints[i].position.z * (*scale) + coords->z);
						Sprite::MakeSprite(cloudsprite, joints[i].position * (*scale) + *coords, joints[i].velocity * .06f, terrainlight.x * 190 / 255, terrainlight.y * 170 / 255, terrainlight.z * 108 / 255, .5f, .7f, bloodtoggle);
					}

					else if (environment == grassyenvironment && findLengthfast(&bounceness) > 500 && terrain.getOpacity(joints[i].position.x * (*scale) + coords->x, joints[i].position.z * (*scale) + coords->z) < .2f) {
						terrainlight = terrain.getLighting(joints[i].position.x * (*scale) + coords->x, joints[i].position.z * (*scale) + coords->z);
						Sprite::MakeSprite(cloudsprite, joints[i].position * (*scale) + *coords, joints[i].velocity * .06f, terrainlight.x * 90 / 255, terrainlight.y * 70 / 255, terrainlight.z * 8 / 255, .5f, .5f, bloodtoggle);
					}
					else if (findLengthfast(&bounceness) > 500) {
						Sprite::MakeSprite(cloudsprite, joints[i].position * (*scale) + *coords, joints[i].velocity * .06f, terrainlight.x, terrainlight.y, terrainlight.z, .5f, .2f, bloodtoggle);
					}

					joints[i].position.y = (terrain.getHeight(joints[i].position.x * (*scale) + coords->x, joints[i].position.z * (*scale) + coords->z) + groundlevel - coords->y) / (*scale);
					if (longdead > 100) {
						broken = 1;
					}
				}
				for (unsigned int m = 0; m < terrain.patchobjects[whichpatchx][whichpatchz].size(); m++) {
					unsigned int k = terrain.patchobjects[whichpatchx][whichpatchz][m];
					if (k < Object::objects.size()) {
						if (Object::objects[k]->possible) {
							friction = Object::objects[k]->friction;
							Vector3 start = joints[i].realoldposition;
							Vector3 end = joints[i].position * (*scale) + *coords;
							whichhit = Object::objects[k]->model.LineCheckPossible(&start, &end, &temp, &Object::objects[k]->position, &Object::objects[k]->yaw);
							if (whichhit != -1) {
								if (joints[i].label == groin && !joints[i].locked && joints[i].delay <= 0) {
									joints[i].locked = 1;
									joints[i].delay = 1;
									if (!tutorialActive || id == 0) {
										emit_sound_at(landsound1, joints[i].position * (*scale) + *coords, 128.);
									}
									breaking = true;
								}

								if (joints[i].label == head && !joints[i].locked && joints[i].delay <= 0) {
									joints[i].locked = 1;
									joints[i].delay = 1;
									if (!tutorialActive || id == 0) {
										emit_sound_at(landsound2, joints[i].position * (*scale) + *coords, 128.);
									}
								}

								terrainnormal = DoRotation(Object::objects[k]->model.Triangles[whichhit].facenormal, 0, Object::objects[k]->yaw, 0) * -1;
								if (terrainnormal.y > .8) {
									freefall = 0;
								}
								bounceness = terrainnormal * findLength(&joints[i].velocity) * (abs(normaldotproduct(joints[i].velocity, terrainnormal)));
								if (findLengthfast(&joints[i].velocity) > findLengthfast(&joints[i].oldvelocity)) {
									bounceness = 0;
									joints[i].velocity = joints[i].oldvelocity;
								}
								if (!tutorialActive || id == 0) {
									if (findLengthfast(&bounceness) > 4000 && breaking) {
										Object::objects[k]->model.MakeDecal(breakdecal, DoRotation(temp - Object::objects[k]->position, 0, -Object::objects[k]->yaw, 0), .4f, .5f, (float)(rand() % 360));
										Sprite::MakeSprite(cloudsprite, joints[i].position * (*scale) + *coords, joints[i].velocity * .06f, 1, 1, 1, 4, .2f, bloodtoggle);
										breaking = false;
										camerashake += .6f;

										emit_sound_at(breaksound2, joints[i].position * (*scale) + *coords);

										addEnvSound(*coords, 64);
									}
								}
								if (Object::objects[k]->type == treetrunktype) {
									Object::objects[k]->rotx += joints[i].velocity.x * multiplier * .4f;
									Object::objects[k]->roty += joints[i].velocity.z * multiplier * .4f;
									Object::objects[k + 1]->rotx += joints[i].velocity.x * multiplier * .4f;
									Object::objects[k + 1]->roty += joints[i].velocity.z * multiplier * .4f;
								}
								if (!joints[i].locked) {
									damage += findLengthfast(&bounceness) / 2500;
								}
								ReflectVector(&joints[i].velocity, &terrainnormal);
								frictionness = abs(normaldotproduct(joints[i].velocity, terrainnormal));
								joints[i].velocity -= bounceness;
								if (1 - friction * frictionness > 0) {
									joints[i].velocity *= 1 - friction * frictionness;
								}
								else {
									joints[i].velocity = 0;
								}
								if (findLengthfast(&bounceness) > 2500) {
									Normalise(&bounceness);
									bounceness = bounceness * 50;
								}
								joints[i].velocity += bounceness * elasticity;

								if (!joints[i].locked) {
									if (findLengthfast(&joints[i].velocity) < 1) {
										joints[i].locked = 1;
									}
								}
								if (findLengthfast(&bounceness) > 500) {
									Sprite::MakeSprite(cloudsprite, joints[i].position * (*scale) + *coords, joints[i].velocity * .06f, 1, 1, 1, .5f, .2f, bloodtoggle);
								}
								joints[i].position = (temp - *coords) / (*scale) + terrainnormal * .005f;
								if (longdead > 100) {
									broken = 1;
								}
							}
						}
					}
				}
				joints[i].realoldposition = joints[i].position * (*scale) + *coords;
			}
		}
		multiplier = tempmult;

		for (unsigned int m = 0; m < terrain.patchobjects[whichpatchx][whichpatchz].size(); m++) {
			unsigned int k = terrain.patchobjects[whichpatchx][whichpatchz][m];
			if (Object::objects[k]->possible) {
				for (i = 0; i < 26; i++) {
					//Make this less stupid
					Vector3 start = joints[jointlabels[whichjointstartarray[i]]].position * (*scale) + *coords;
					Vector3 end = joints[jointlabels[whichjointendarray[i]]].position * (*scale) + *coords;
					whichhit = Object::objects[k]->model.LineCheckSlidePossible(&start, &end, &Object::objects[k]->position, &Object::objects[k]->yaw);
					if (whichhit != -1) {
						joints[jointlabels[whichjointendarray[i]]].position = (end - *coords) / (*scale);
						for (unsigned j = 0; j < muscles.size(); j++) {
							if ((muscles[j].parent1->label == whichjointstartarray[i] && muscles[j].parent2->label == whichjointendarray[i]) || (muscles[j].parent2->label == whichjointstartarray[i] && muscles[j].parent1->label == whichjointendarray[i])) {
								muscles[j].DoConstraint(spinny);
							}
						}
					}
				}
			}
		}

		for (i = 0; i < joints.size(); i++) {
			switch (joints[i].label) {
			case head:
				groundlevel = .8f;
				break;
			case righthand:
			case rightwrist:
			case rightelbow:
			case lefthand:
			case leftwrist:
			case leftelbow:
				groundlevel = .2f;
				break;
			default:
				groundlevel = .15f;
				break;
			}
			joints[i].position.y += groundlevel;
			joints[i].mass = 1;
			if (joints[i].label == lefthip || joints[i].label == leftknee || joints[i].label == leftankle || joints[i].label == righthip || joints[i].label == rightknee || joints[i].label == rightankle) {
				joints[i].mass = 2;
			}
			if (joints[i].locked) {
				joints[i].mass = 4;
			}
		}

		return damage;
	}

	if (!free) {
		for (i = 0; i < muscles.size(); i++) {
			if (muscles[i].type == boneconnect) {
				muscles[i].DoConstraint(0);
			}
		}
	}

	return 0;
}

/* EFFECT
 * applies gravity to the skeleton
 *
 * USES:
 * Person/Person::DoStuff
 */
void Skeleton::DoGravity(float* scale)
{
	for (unsigned i = 0; i < joints.size(); i++) {
		if (
			(
				((joints[i].label != leftknee) && (joints[i].label != rightknee)) ||
				(lowforward.y > -.1) ||
				(joints[i].mass < 5)) &&
			(((joints[i].label != leftelbow) && (joints[i].label != rightelbow)) ||
				(forward.y < .3))) {
			joints[i].velocity.y += gravity * multiplier / (*scale);
		}
	}
}

/* EFFECT
 * set muscles[which].rotate1
 *     .rotate2
 *     .rotate3
 *
 * special case if animation == hanganim
 */
void Skeleton::FindRotationMuscle(int which, int animation)
{
	Vector3 p1, p2, fwd;
	float dist;

	p1 = muscles[which].parent1->position;
	p2 = muscles[which].parent2->position;
	dist = findDistance(&p1, &p2);
	if (p1.y - p2.y <= dist) {
		muscles[which].rotate2 = asin((p1.y - p2.y) / dist);
	}
	if (p1.y - p2.y > dist) {
		muscles[which].rotate2 = asin(1.f);
	}
	muscles[which].rotate2 *= 360.0f / 6.2831853f;

	p1.y = 0;
	p2.y = 0;
	dist = findDistance(&p1, &p2);
	if (p1.z - p2.z <= dist) {
		muscles[which].rotate1 = acos((p1.z - p2.z) / dist);
	}
	if (p1.z - p2.z > dist) {
		muscles[which].rotate1 = acos(1.f);
	}
	muscles[which].rotate1 *= 360.0f / 6.2831853f;
	if (p1.x > p2.x) {
		muscles[which].rotate1 = 360 - muscles[which].rotate1;
	}
	if (!isnormal(muscles[which].rotate1)) {
		muscles[which].rotate1 = 0;
	}
	if (!isnormal(muscles[which].rotate2)) {
		muscles[which].rotate2 = 0;
	}

	const int label1 = muscles[which].parent1->label;
	const int label2 = muscles[which].parent2->label;
	switch (label1) {
	case head:
		fwd = specialforward[0];
		break;
	case rightshoulder:
	case rightelbow:
	case rightwrist:
	case righthand:
		fwd = specialforward[1];
		break;
	case leftshoulder:
	case leftelbow:
	case leftwrist:
	case lefthand:
		fwd = specialforward[2];
		break;
	case righthip:
	case rightknee:
	case rightankle:
	case rightfoot:
		fwd = specialforward[3];
		break;
	case lefthip:
	case leftknee:
	case leftankle:
	case leftfoot:
		fwd = specialforward[4];
		break;
	default:
		if (muscles[which].parent1->lower) {
			fwd = lowforward;
		}
		else {
			fwd = forward;
		}
		break;
	}

	if (animation == hanganim) {
		if (label1 == righthand || label2 == righthand) {
			fwd = 0;
			fwd.x = -1;
		}
		if (label1 == lefthand || label2 == lefthand) {
			fwd = 0;
			fwd.x = 1;
		}
	}

	if (free == 0) {
		if (label1 == rightfoot || label2 == rightfoot) {
			fwd.y -= .3f;
		}
		if (label1 == leftfoot || label2 == leftfoot) {
			fwd.y -= .3f;
		}
	}

	fwd = DoRotation(fwd, 0, muscles[which].rotate1 - 90, 0);
	fwd = DoRotation(fwd, 0, 0, muscles[which].rotate2 - 90);
	fwd.y = 0;
	fwd /= findLength(&fwd);
	if (fwd.z <= 1 && fwd.z >= -1) {
		muscles[which].rotate3 = acos(0 - fwd.z);
	}
	else {
		muscles[which].rotate3 = acos(-1.f);
	}
	muscles[which].rotate3 *= 360.0f / 6.2831853f;
	if (0 > fwd.x) {
		muscles[which].rotate3 = 360 - muscles[which].rotate3;
	}
	if (!isnormal(muscles[which].rotate3)) {
		muscles[which].rotate3 = 0;
	}
}

/* EFFECT
 * load skeleton
 * takes filenames for three skeleton files and various models
 */
void Skeleton::Load(const std::string& filename, const std::string& lowfilename, const std::string& clothesfilename,
	const std::string& modelfilename, const std::string& model2filename,
	const std::string& model3filename, const std::string& model4filename,
	const std::string& model5filename, const std::string& model6filename,
	const std::string& model7filename, const std::string& modellowfilename,
	const std::string& modelclothesfilename, bool clothes, bool tutorialActive, ProgressCallback callback)
{
	GLfloat M[16];
	FILE* tfile = nullptr;
	size_t lSize = 0;
	int j, num_joints, num_muscles;

	num_models = 7;

	// load various models
	// rotate, scale, do normals, do texcoords for each as needed

	model[0].loadnotex(modelfilename);
	model[1].loadnotex(model2filename);
	model[2].loadnotex(model3filename);
	model[3].loadnotex(model4filename);
	model[4].loadnotex(model5filename);
	model[5].loadnotex(model6filename);
	model[6].loadnotex(model7filename);

	for (int i = 0; i < num_models; i++) {
		model[i].Rotate(180, 0, 0);
		model[i].Scale(.04f, .04f, .04f);
		model[i].CalculateNormals(0, callback);
	}

	drawmodel.load(modelfilename, callback);
	drawmodel.Rotate(180, 0, 0);
	drawmodel.Scale(.04f, .04f, .04f);
	drawmodel.FlipTexCoords();
	if ((tutorialActive) && (id != 0)) {
		drawmodel.UniformTexCoords();
		drawmodel.ScaleTexCoords(0.1);
	}
	drawmodel.CalculateNormals(0, callback);

	modellow.loadnotex(modellowfilename);
	modellow.Rotate(180, 0, 0);
	modellow.Scale(.04f, .04f, .04f);
	modellow.CalculateNormals(0, callback);

	drawmodellow.load(modellowfilename, callback);
	drawmodellow.Rotate(180, 0, 0);
	drawmodellow.Scale(.04f, .04f, .04f);
	drawmodellow.FlipTexCoords();
	if (tutorialActive && id != 0) {
		drawmodellow.UniformTexCoords();
	}
	if (tutorialActive && id != 0) {
		drawmodellow.ScaleTexCoords(0.1f);
	}
	drawmodellow.CalculateNormals(0, callback);

	if (clothes) {
		modelclothes.loadnotex(modelclothesfilename);
		modelclothes.Rotate(180, 0, 0);
		modelclothes.Scale(.041f, .04f, .041f);
		modelclothes.CalculateNormals(0, callback);

		drawmodelclothes.load(modelclothesfilename, callback);
		drawmodelclothes.Rotate(180, 0, 0);
		drawmodelclothes.Scale(.04f, .04f, .04f);
		drawmodelclothes.FlipTexCoords();
		drawmodelclothes.CalculateNormals(0, callback);
	}

	// FIXME: three similar blocks follow, one for each of:
	// filename, lowfilename, clothesfilename

	// load skeleton

	tfile = Folders::openMandatoryFile(Folders::getResourcePath(filename), "rb");

	// read num_joints
	funpackf(tfile, "Bi", &num_joints);

	joints.clear();
	joints.resize(num_joints);

	// read info for each joint
	for (int i = 0; i < num_joints; i++) {
		joints[i].load(tfile, joints);
	}

	// read num_muscles
	funpackf(tfile, "Bi", &num_muscles);

	// allocate memory
	muscles.clear();
	muscles.resize(num_muscles);

	// for each muscle...
	for (int i = 0; i < num_muscles; i++) {
		muscles[i].load(tfile, model[0].vertexNum, joints);
	}

	// read forwardjoints (?)
	for (j = 0; j < 3; j++) {
		funpackf(tfile, "Bi", &forwardjoints[j]);
	}
	// read lowforwardjoints (?)
	for (j = 0; j < 3; j++) {
		funpackf(tfile, "Bi", &lowforwardjoints[j]);
	}

	// ???
	for (j = 0; j < num_muscles; j++) {
		for (unsigned i = 0; i < muscles[j].vertices.size(); i++) {
			for (int k = 0; k < num_models; k++) {
				if (muscles[j].vertices[i] < model[k].vertexNum) {
					model[k].owner[muscles[j].vertices[i]] = j;
				}
			}
		}
	}

	// calculate some stuff
	FindForwards();
	for (int i = 0; i < num_muscles; i++) {
		FindRotationMuscle(i, -1);
	}
	// this seems to use opengl purely for matrix calculations
	for (int k = 0; k < num_models; k++) {
		for (int i = 0; i < model[k].vertexNum; i++) {
			model[k].vertex[i] = model[k].vertex[i] - (muscles[model[k].owner[i]].parent1->position + muscles[model[k].owner[i]].parent2->position) / 2;
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glRotatef(muscles[model[k].owner[i]].rotate3, 0, 1, 0);
			glRotatef(muscles[model[k].owner[i]].rotate2 - 90, 0, 0, 1);
			glRotatef(muscles[model[k].owner[i]].rotate1 - 90, 0, 1, 0);
			glTranslatef(model[k].vertex[i].x, model[k].vertex[i].y, model[k].vertex[i].z);
			glGetFloatv(GL_MODELVIEW_MATRIX, M);
			model[k].vertex[i].x = M[12] * 1;
			model[k].vertex[i].y = M[13] * 1;
			model[k].vertex[i].z = M[14] * 1;
			glPopMatrix();
		}
		model[k].CalculateNormals(0, callback);
	}
	fclose(tfile);

	// load ???

	tfile = Folders::openMandatoryFile(Folders::getResourcePath(lowfilename), "rb");

	// skip joints section

	fseek(tfile, sizeof(num_joints), SEEK_CUR);
	for (int i = 0; i < num_joints; i++) {
		// skip joint info
		lSize = sizeof(Vector3) + sizeof(float) + sizeof(float) + 1 //sizeof(bool)
			+ 1                                             //sizeof(bool)
			+ sizeof(int) + 1                               //sizeof(bool)
			+ 1                                             //sizeof(bool)
			+ sizeof(int) + sizeof(int) + 1                 //sizeof(bool)
			+ sizeof(int);
		fseek(tfile, lSize, SEEK_CUR);
	}

	// skip num_muscles
	fseek(tfile, sizeof(num_muscles), SEEK_CUR);

	for (int i = 0; i < num_muscles; i++) {
		// skip muscle info
		lSize = sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(int);
		fseek(tfile, lSize, SEEK_CUR);

		muscles[i].loadVerticesLow(tfile, modellow.vertexNum);

		// skip more stuff
		lSize = 1; //sizeof(bool);
		fseek(tfile, lSize, SEEK_CUR);
		lSize = sizeof(int);
		fseek(tfile, lSize, SEEK_CUR);
		fseek(tfile, lSize, SEEK_CUR);
	}

	for (j = 0; j < num_muscles; j++) {
		for (unsigned i = 0; i < muscles[j].verticeslow.size(); i++) {
			if (muscles[j].verticeslow[i] < modellow.vertexNum) {
				modellow.owner[muscles[j].verticeslow[i]] = j;
			}
		}
	}

	// use opengl for its matrix math
	for (int i = 0; i < modellow.vertexNum; i++) {
		modellow.vertex[i] = modellow.vertex[i] - (muscles[modellow.owner[i]].parent1->position + muscles[modellow.owner[i]].parent2->position) / 2;
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glRotatef(muscles[modellow.owner[i]].rotate3, 0, 1, 0);
		glRotatef(muscles[modellow.owner[i]].rotate2 - 90, 0, 0, 1);
		glRotatef(muscles[modellow.owner[i]].rotate1 - 90, 0, 1, 0);
		glTranslatef(modellow.vertex[i].x, modellow.vertex[i].y, modellow.vertex[i].z);
		glGetFloatv(GL_MODELVIEW_MATRIX, M);
		modellow.vertex[i].x = M[12];
		modellow.vertex[i].y = M[13];
		modellow.vertex[i].z = M[14];
		glPopMatrix();
	}

	modellow.CalculateNormals(0, callback);

	// load clothes

	if (clothes) {
		tfile = Folders::openMandatoryFile(Folders::getResourcePath(clothesfilename), "rb");

		// skip num_joints
		fseek(tfile, sizeof(num_joints), SEEK_CUR);

		for (int i = 0; i < num_joints; i++) {
			// skip joint info
			lSize = sizeof(Vector3) + sizeof(float) + sizeof(float) + 1 //sizeof(bool)
				+ 1                                             //sizeof(bool)
				+ sizeof(int) + 1                               //sizeof(bool)
				+ 1                                             //sizeof(bool)
				+ sizeof(int) + sizeof(int) + 1                 //sizeof(bool)
				+ sizeof(int);
			fseek(tfile, lSize, SEEK_CUR);
		}

		// skip num_muscles
		fseek(tfile, sizeof(num_muscles), SEEK_CUR);

		for (int i = 0; i < num_muscles; i++) {
			// skip muscle info
			lSize = sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(int);
			fseek(tfile, lSize, SEEK_CUR);

			muscles[i].loadVerticesClothes(tfile, modelclothes.vertexNum);

			// skip more stuff
			lSize = 1; //sizeof(bool);
			fseek(tfile, lSize, SEEK_CUR);
			lSize = sizeof(int);
			fseek(tfile, lSize, SEEK_CUR);
			fseek(tfile, lSize, SEEK_CUR);
		}

		// ???
		lSize = sizeof(int);
		for (j = 0; j < num_muscles; j++) {
			for (unsigned i = 0; i < muscles[j].verticesclothes.size(); i++) {
				if (muscles[j].verticesclothes.size() && muscles[j].verticesclothes[i] < modelclothes.vertexNum) {
					modelclothes.owner[muscles[j].verticesclothes[i]] = j;
				}
			}
		}

		// use opengl for its matrix math
		for (int i = 0; i < modelclothes.vertexNum; i++) {
			modelclothes.vertex[i] = modelclothes.vertex[i] - (muscles[modelclothes.owner[i]].parent1->position + muscles[modelclothes.owner[i]].parent2->position) / 2;
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glRotatef(muscles[modelclothes.owner[i]].rotate3, 0, 1, 0);
			glRotatef(muscles[modelclothes.owner[i]].rotate2 - 90, 0, 0, 1);
			glRotatef(muscles[modelclothes.owner[i]].rotate1 - 90, 0, 1, 0);
			glTranslatef(modelclothes.vertex[i].x, modelclothes.vertex[i].y, modelclothes.vertex[i].z);
			glGetFloatv(GL_MODELVIEW_MATRIX, M);
			modelclothes.vertex[i].x = M[12];
			modelclothes.vertex[i].y = M[13];
			modelclothes.vertex[i].z = M[14];
			glPopMatrix();
		}

		modelclothes.CalculateNormals(0, callback);
	}
	fclose(tfile);

	for (int i = 0; i < num_joints; i++) {
		for (j = 0; j < num_joints; j++) {
			if (joints[i].label == j) {
				jointlabels[j] = i;
			}
		}
	}

	free = 0;
}
