/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2016, Carnegie Mellon University
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Carnegie Mellon University nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/** \author Jeongseok Lee <jslee02@gmail.com> */

#define BOOST_TEST_MODULE FCL_SHAPE_MESH_CONSISTENCY
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/utility/binary.hpp>

#include <math.h>

#include "hpp/fcl/narrowphase/narrowphase.h"
#include "hpp/fcl/shape/geometric_shape_to_BVH_model.h"
#include "hpp/fcl/distance.h"
#include "hpp/fcl/collision.h"
#include "test_fcl_utility.h"

using namespace fcl;


namespace
{

    bool checkBoundingBox(const Vec3f& min, const Vec3f& max,
                          const CollisionResult& result, double margin = 1e-6)
    {
        for (std::size_t i = 0; i < result.numContacts(); ++i)
        {
            const Vec3f& point = result.getContact(i).pos;
            for (std::size_t i = 0u; i < 3u; ++i)
            {
                if (min[i] - margin > point[i] || point[i] > max[i] + margin)
                {
                    std::cout << "invalid contact point: " << point << std::endl;
                    return false;
                }
            }
        }
         return true;
    }
}

BOOST_AUTO_TEST_CASE(collision_filter_redundant_contactpoint)
{
  const Transform3f identity;
  Transform3f pose;

  Box s1(1, 1, 1);
  Box s2(0.5, 0.5, 0.5);
  BVHModel<OBBRSS> s1_obbrss;
  BVHModel<OBBRSS> s2_obbrss;

  generateBVHModel(s1_obbrss, s1, identity);
  generateBVHModel(s2_obbrss, s2, identity);

  CollisionRequest request(1e+3, true);
  CollisionRequest requestFilter(1e+3, true);
  requestFilter.filter_contact_points = true;

  CollisionResult result, resultFilter;

  //----------------------------------------------------------------------------
  // Case2: The left side (-y axis) of s2 touching the right side (+y axis) of
  // s1. The intersection is a face where the size is the same with the face of
  // s2 (0.5 x 0.5).
  //----------------------------------------------------------------------------
  pose.setTranslation(Vec3f(0, 0.75, 0));

  result.clear();resultFilter.clear();
  collide(&s1_obbrss, identity, &s2_obbrss, pose, request, result);
  collide(&s1_obbrss, identity, &s2_obbrss, pose, requestFilter, resultFilter);
  BOOST_CHECK(resultFilter.numContacts() == 5);
  BOOST_CHECK(resultFilter.numContacts() < result.numContacts());
}


BOOST_AUTO_TEST_CASE(collision_meshmesh_triangle_coplanar)
{
  Vec3f min(-0.25, 0.5, -0.25);
  Vec3f max(0.25, 0.5, 0.25);

  const Transform3f identity;
  Transform3f pose;

  Box s1(1, 1, 1);
  Box s2(0.5, 0.5, 0.5);

  BVHModel<AABB> s1_aabb;
  BVHModel<AABB> s2_aabb;
  BVHModel<OBB> s1_obb;
  BVHModel<OBB> s2_obb;
  BVHModel<RSS> s1_rss;
  BVHModel<RSS> s2_rss;
  BVHModel<OBBRSS> s1_obbrss;
  BVHModel<OBBRSS> s2_obbrss;

  generateBVHModel(s1_aabb, s1, identity);
  generateBVHModel(s2_aabb, s2, identity);
  generateBVHModel(s1_obb, s1, identity);
  generateBVHModel(s2_obb, s2, identity);
  generateBVHModel(s1_rss, s1, identity);
  generateBVHModel(s2_rss, s2, identity);
  generateBVHModel(s1_obbrss, s1, identity);
  generateBVHModel(s2_obbrss, s2, identity);

  CollisionRequest request(1e+3, true);
  request.filter_contact_points = true;

  CollisionResult result;

  //----------------------------------------------------------------------------
  // Case1: s2 is completely inside of s1.
  //
  // Primitive collision returns the maximum number of contact points, 4,
  // whereas mesh collision returns no contact.
  //----------------------------------------------------------------------------

  //TODO
  //collide(&s1, identity, &s2, identity, request, result);
  //BOOST_CHECK_EQUAL(result.numContacts(), 4);  // maximum contact number of box-box
  //result.clear();

  //----------------------------------------------------------------------------
  // Case2: The left side (-y axis) of s2 touching the right side (+y axis) of
  // s1. The intersection is a face where the size is the same with the face of
  // s2 (0.5 x 0.5).
  //----------------------------------------------------------------------------

  pose.setTranslation(Vec3f(0, 0.75, 0));

  //TODO
  //collide(&s1, identity, &s2, pose, request, result);
  //BOOST_CHECK(checkExpectedValue(result,points));

  result.clear();
  // TODO NOT WORKING
  collide(&s1_aabb, identity, &s2_aabb, pose, request, result);
  //BOOST_CHECK( checkBoundingBox(min, max, result));

  result.clear();
  collide(&s1_obb, identity, &s2_obb, pose, request, result);
  BOOST_CHECK( checkBoundingBox(min, max, result));

  result.clear();
  // TODO NOT WORKING
  collide(&s1_rss, identity, &s2_rss, pose, request, result);
  //BOOST_CHECK( checkBoundingBox(min, max, result));

  result.clear();
  collide(&s1_obbrss, identity, &s2_obbrss, pose, request, result);
  BOOST_CHECK( checkBoundingBox(min, max, result));
}

BOOST_AUTO_TEST_CASE(collision_meshmesh_triangle_non_coplanar)
{


  const Transform3f identity;
  Transform3f pose;

  Box s1(1, 1, 1);
  Box s2(0.5, 0.5, 0.5);

  BVHModel<AABB> s1_aabb;
  BVHModel<AABB> s2_aabb;
  BVHModel<OBB> s1_obb;
  BVHModel<OBB> s2_obb;
  BVHModel<RSS> s1_rss;
  BVHModel<RSS> s2_rss;
  BVHModel<OBBRSS> s1_obbrss;
  BVHModel<OBBRSS> s2_obbrss;

  generateBVHModel(s1_aabb, s1, identity);
  generateBVHModel(s2_aabb, s2, identity);
  generateBVHModel(s1_obb, s1, identity);
  generateBVHModel(s2_obb, s2, identity);
  generateBVHModel(s1_rss, s1, identity);
  generateBVHModel(s2_rss, s2, identity);
  generateBVHModel(s1_obbrss, s1, identity);
  generateBVHModel(s2_obbrss, s2, identity);

  CollisionRequest request(1e+3, true);
  request.filter_contact_points = true;

  CollisionResult result;

  //----------------------------------------------------------------------------
  // Case2: The left side (-y axis) of s2 touching the right side (+y axis) of
  // s1. The intersection is a face where the size is the same with the face of
  // s2 (0.5 x 0.5).
  //----------------------------------------------------------------------------


  pose.setTranslation(Vec3f(0, 0.75, 0));
  fcl::Quaternion3f q;
  q.fromEuler(0,0,M_PI/4);
  Matrix3f r;
  q.toRotation(r);
  pose.setRotation(r);

  Vec3f min(-0.25, 0.5, -0.103553);
  Vec3f max(0.25, 0.5, 0.103553);

  result.clear();
  // TODO NOT WORKING
  collide(&s1_aabb, identity, &s2_aabb, pose, request, result);
  //BOOST_CHECK( checkBoundingBox(min, max, result));

  result.clear();
  collide(&s1_obb, identity, &s2_obb, pose, request, result);
  BOOST_CHECK( checkBoundingBox(min, max, result));

  result.clear();
  // TODO NOT WORKING
  collide(&s1_rss, identity, &s2_rss, pose, request, result);
  //BOOST_CHECK( checkBoundingBox(min, max, result));

  result.clear();
  collide(&s1_obbrss, identity, &s2_obbrss, pose, request, result);
  BOOST_CHECK( checkBoundingBox(min, max, result));
}
