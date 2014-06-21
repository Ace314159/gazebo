/*
 * Copyright (C) 2012-2014 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include "gazebo/common/Console.hh"
#include "gazebo/physics/simbody/simbody_inc.h"
#include "gazebo/physics/simbody/SimbodyCollision.hh"
#include "gazebo/physics/simbody/SimbodyPhysics.hh"
#include "gazebo/physics/SurfaceParams.hh"

using namespace gazebo;
using namespace physics;

//////////////////////////////////////////////////
SimbodyCollision::SimbodyCollision(LinkPtr _parent)
    : Collision(_parent)
{
  this->SetName("Simbody_Collision");
  this->collisionShape = NULL;
  this->surface.reset(new SurfaceParams());
}

//////////////////////////////////////////////////
SimbodyCollision::~SimbodyCollision()
{
  delete this->collisionShape;
  this->collisionShape = NULL;
}

//////////////////////////////////////////////////
void SimbodyCollision::Load(sdf::ElementPtr _sdf)
{
  Collision::Load(_sdf);
}

//////////////////////////////////////////////////
void SimbodyCollision::OnPoseChange()
{
  // math::Pose pose = this->GetRelativePose();
  // SimbodyLink *bbody = static_cast<SimbodyLink*>(this->body);

  // bbody->SetCollisionRelativePose(this, pose);
}

//////////////////////////////////////////////////
void SimbodyCollision::SetCategoryBits(unsigned int /*_bits*/)
{
}

//////////////////////////////////////////////////
void SimbodyCollision::SetCollideBits(unsigned int /*_bits*/)
{
}

//////////////////////////////////////////////////
math::Box SimbodyCollision::GetBoundingBox() const
{
  math::Box result;
  if (this->collisionShape == NULL)
  {
    gzerr << "This SimbodyCollision has no collisionShape.\n";
  }
  else
  {
#if 1
    // take this approach if calcSupportPoint is not working...
    SimTK::Vec3 center;
    double r;
    this->collisionShape->getBoundingSphere(center, r);

    // get AABB by calling calcSupportPoint
    math::Vector3 minCorner(center[0]-r, center[1]-r, center[2]-r);
    math::Vector3 maxCorner(center[0]+r, center[1]+r, center[2]+r);
#else
    // get AABB by calling calcSupportPoint
    math::Vector3 minCorner;
    math::Vector3 maxCorner;
    minCorner.x = SimbodyPhysics::Vec3ToVector3(
      this->collisionShape->calcSupportPoint(SimTK::UnitVec3(-1, 0, 0))).x;
    maxCorner.x = SimbodyPhysics::Vec3ToVector3(
      this->collisionShape->calcSupportPoint(SimTK::UnitVec3(1, 0, 0))).x;
    minCorner.y = SimbodyPhysics::Vec3ToVector3(
      this->collisionShape->calcSupportPoint(SimTK::UnitVec3(0, -1, 0))).y;
    maxCorner.y = SimbodyPhysics::Vec3ToVector3(
      this->collisionShape->calcSupportPoint(SimTK::UnitVec3(0, 1, 0))).y;
    minCorner.z = SimbodyPhysics::Vec3ToVector3(
      this->collisionShape->calcSupportPoint(SimTK::UnitVec3(0, 0, -1))).z;
    maxCorner.z = SimbodyPhysics::Vec3ToVector3(
      this->collisionShape->calcSupportPoint(SimTK::UnitVec3(0, 0, 1))).z;
#endif
    result = math::Box(minCorner, maxCorner);
  }
  return result;
}

//////////////////////////////////////////////////
void SimbodyCollision::SetCollisionShape(SimTK::ContactGeometry *_shape)
{
  this->collisionShape = _shape;
}

//////////////////////////////////////////////////
SimTK::ContactGeometry *SimbodyCollision::GetCollisionShape() const
{
  return this->collisionShape;
}
