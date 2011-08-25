/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011, Willow Garage, Inc.
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
 *   * Neither the name of Willow Garage, Inc. nor the names of its
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

/** \author Jia Pan */


#ifndef FCL_SIMPLE_SETUP_H
#define FCL_SIMPLE_SETUP_H

#include "fcl/traversal_node_bvhs.h"
#include "fcl/traversal_node_shapes.h"
#include "fcl/traversal_node_bvh_shape.h"
#include "fcl/BVH_utility.h"

namespace fcl
{


template<typename S1, typename S2>
bool initialize(ShapeCollisionTraversalNode<S1, S2>& node, const S1& shape1, const S2& shape2, bool enable_contact = false)
{
  node.enable_contact = enable_contact;
  return true;
}

template<typename S1, typename S2>
bool initialize(ShapeCollisionTraversalNode<S1, S2>& node, const S1& shape1, const S2& shape2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2,
                bool enable_contact = false)
{
  for(int i = 0; i < 3; ++i)
  {
    node.R1[i] = R1[i];
    node.R2[i] = R2[i];
  }
  node.T1 = T1;
  node.T2 = T2;
  node.enable_contact = enable_contact;

  return true;
}


template<typename BV, typename S>
bool initialize(MeshShapeCollisionTraversalNode<BV, S>& node, const BVHModel<BV>& model1, const S& model2, int num_max_contacts = 1, bool exhaustive = false, bool enable_contact = false)
{
  if(model1.getModelType() != BVH_MODEL_TRIANGLES)
    return false;

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices = model1.vertices;
  node.tri_indices = model1.tri_indices;
  node.num_max_contacts = num_max_contacts;
  node.exhaustive = exhaustive;
  node.enable_contact = enable_contact;

  return true;
}


template<typename BV, typename S>
bool initialize(MeshShapeCollisionTraversalNode<BV, S>& node, BVHModel<BV>& model1, S& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2,
                int num_max_contacts = 1, bool exhaustive = false, bool enable_contact = false,
                bool use_refit = false, bool refit_bottomup = false)
{
  if(model1.getModelType() != BVH_MODEL_TRIANGLES)
    return false;

  node.model1 = &model1;
  node.model2 = &model2;

  std::vector<Vec3f> vertices_transformed1(model1.num_vertices);
  for(int i = 0; i < model1.num_vertices; ++i)
  {
    Vec3f& p = model1.vertices[i];
    Vec3f new_v = MxV(R1, p) + T1;
    vertices_transformed1[i] = new_v;
  }

  model1.beginReplaceModel();
  model1.replaceSubModel(vertices_transformed1);
  model1.endReplaceModel(use_refit, refit_bottomup);

  model2.advanceLocalTransform(R2, T2);

  node.vertices = model1.vertices;
  node.tri_indices = model1.tri_indices;
  node.num_max_contacts = num_max_contacts;
  node.exhaustive = exhaustive;
  node.enable_contact = enable_contact;

  return true;
}



template<typename S>
bool initialize(MeshShapeCollisionTraversalNodeOBB<S>& node, const BVHModel<OBB>& model1, const S& model2, int num_max_contacts = 1, bool exhaustive = false, bool enable_contact = false)
{
  if(model1.getModelType() != BVH_MODEL_TRIANGLES)
    return false;

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices = model1.vertices;
  node.tri_indices = model1.tri_indices;
  node.num_max_contacts = num_max_contacts;
  node.exhaustive = exhaustive;
  node.enable_contact = enable_contact;

  return true;
}


template<typename S>
bool initialize(MeshShapeCollisionTraversalNodeOBB<S>& node, const BVHModel<OBB>& model1, S& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2,
                int num_max_contacts = 1, bool exhaustive = false, bool enable_contact = false)
{
  if(model1.getModelType() != BVH_MODEL_TRIANGLES)
    return false;

  node.model1 = &model1;
  node.model2 = &model2;

  model2.advanceLocalTransform(R2, T2);

  node.vertices = model1.vertices;
  node.tri_indices = model1.tri_indices;
  node.num_max_contacts = num_max_contacts;
  node.exhaustive = exhaustive;
  node.enable_contact = enable_contact;
  node.R[0] = R1[0]; node.R[1] = R1[1]; node.R[2] = R1[2];
  node.T = T1;

  return true;
}


template<typename BV>
bool initialize(MeshCollisionTraversalNode<BV>& node, const BVHModel<BV>& model1, const BVHModel<BV>& model2, int num_max_contacts = 1, bool exhaustive = false, bool enable_contact = false)
{
  if(model1.getModelType() != BVH_MODEL_TRIANGLES || model2.getModelType() != BVH_MODEL_TRIANGLES)
    return false;

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices1 = model1.vertices;
  node.vertices2 = model2.vertices;

  node.tri_indices1 = model1.tri_indices;
  node.tri_indices2 = model2.tri_indices;

  node.enable_contact = enable_contact;
  node.exhaustive = exhaustive;
  node.num_max_contacts = num_max_contacts;

  return true;
}


template<typename BV>
bool initialize(MeshCollisionTraversalNode<BV>& node, BVHModel<BV>& model1, BVHModel<BV>& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2, int num_max_contacts = 1, bool exhaustive = false, bool enable_contact = false,
                bool use_refit = false, bool refit_bottomup = false)
{
  if(model1.getModelType() != BVH_MODEL_TRIANGLES || model2.getModelType() != BVH_MODEL_TRIANGLES)
    return false;

  std::vector<Vec3f> vertices_transformed1(model1.num_vertices);
  for(int i = 0; i < model1.num_vertices; ++i)
  {
    Vec3f& p = model1.vertices[i];
    Vec3f new_v = MxV(R1, p) + T1;
    vertices_transformed1[i] = new_v;
  }


  std::vector<Vec3f> vertices_transformed2(model2.num_vertices);
  for(int i = 0; i < model2.num_vertices; ++i)
  {
    Vec3f& p = model2.vertices[i];
    Vec3f new_v = MxV(R2, p) + T2;
    vertices_transformed2[i] = new_v;
  }

  model1.beginReplaceModel();
  model1.replaceSubModel(vertices_transformed1);
  model1.endReplaceModel(use_refit, refit_bottomup);
  model2.beginReplaceModel();
  model2.replaceSubModel(vertices_transformed2);
  model2.endReplaceModel(use_refit, refit_bottomup);

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices1 = model1.vertices;
  node.vertices2 = model2.vertices;

  node.tri_indices1 = model1.tri_indices;
  node.tri_indices2 = model2.tri_indices;

  node.num_max_contacts = num_max_contacts;
  node.exhaustive = exhaustive;
  node.enable_contact = enable_contact;

  return true;
}

bool initialize(MeshCollisionTraversalNodeOBB& node, const BVHModel<OBB>& model1, const BVHModel<OBB>& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2, int num_max_contacts = 1, bool exhaustive = false, bool enable_contact = false);

bool initialize(MeshCollisionTraversalNodeRSS& node, const BVHModel<RSS>& model1, const BVHModel<RSS>& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2, int num_max_contacts = 1, bool exhaustive = false, bool enable_contact = false);

#if USE_SVMLIGHT

template<typename BV>
bool initialize(PointCloudCollisionTraversalNode<BV>& node, BVHModel<BV>& model1, BVHModel<BV>& model2,
                BVH_REAL collision_prob_threshold = 0.5,
                BVH_REAL max_collision_prob = 0,
                int leaf_size_threshold = 1,
                int num_max_contacts = 1,
                bool exhaustive = false,
                bool enable_contact = false,
                BVH_REAL expand_r = 1)
{
  if(!(model1.getModelType() == BVH_MODEL_TRIANGLES || model1.getModelType() == BVH_MODEL_POINTCLOUD)
      || !(model2.getModelType() == BVH_MODEL_TRIANGLES || model2.getModelType() == BVH_MODEL_POINTCLOUD))
    return false;

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices1 = model1.vertices;
  node.vertices2 = model2.vertices;

  node.uc1.reset(new Uncertainty[model1.num_vertices]);
  node.uc2.reset(new Uncertainty[model2.num_vertices]);

  estimateSamplingUncertainty(model1.vertices, model1.num_vertices, node.uc1);
  estimateSamplingUncertainty(model2.vertices, model2.num_vertices, node.uc2);

  BVHExpand(model1, node.uc1.get(), expand_r);
  BVHExpand(model2, node.uc2.get(), expand_r);

  node.num_max_contacts = num_max_contacts;
  node.exhaustive = exhaustive;
  node.enable_contact = enable_contact;
  node.collision_prob_threshold = collision_prob_threshold;
  node.max_collision_prob = max_collision_prob;
  node.leaf_size_threshold = leaf_size_threshold;

  return true;
}

template<typename BV, bool use_refit, bool refit_bottomup>
bool initialize(PointCloudCollisionTraversalNode<BV>& node, BVHModel<BV>& model1, BVHModel<BV>& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2,
                BVH_REAL collision_prob_threshold = 0.5,
                int leaf_size_threshold = 1,
                int num_max_contacts = 1,
                bool exhaustive = false,
                bool enable_contact = false,
                BVH_REAL expand_r = 1)
{
  if(!(model1.getModelType() == BVH_MODEL_TRIANGLES || model1.getModelType() == BVH_MODEL_POINTCLOUD)
      || !(model2.getModelType() == BVH_MODEL_TRIANGLES || model2.getModelType() == BVH_MODEL_POINTCLOUD))
    return false;

  std::vector<Vec3f> vertices_transformed1(model1.num_vertices);
  for(int i = 0; i < model1.num_vertices; ++i)
  {
    Vec3f& p = model1.vertices[i];
    Vec3f new_v = MxV(R1, p) + T1;
    vertices_transformed1[i] = new_v;
  }


  std::vector<Vec3f> vertices_transformed2(model2.num_vertices);
  for(int i = 0; i < model2.num_vertices; ++i)
  {
    Vec3f& p = model2.vertices[i];
    Vec3f new_v = MxV(R2, p) + T2;
    vertices_transformed2[i] = new_v;
  }

  model1.beginReplaceModel();
  model1.replaceSubModel(vertices_transformed1);
  model1.endReplaceModel(use_refit, refit_bottomup);

  model2.beginReplaceModel();
  model2.replaceSubModel(vertices_transformed2);
  model2.endReplaceModel(use_refit, refit_bottomup);

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices1 = model1.vertices;
  node.vertices2 = model2.vertices;

  node.uc1.reset(new Uncertainty[model1.num_vertices]);
  node.uc2.reset(new Uncertainty[model2.num_vertices]);

  estimateSamplingUncertainty(model1.vertices, model1.num_vertices, node.uc1.get());
  estimateSamplingUncertainty(model2.vertices, model2.num_vertices, node.uc2.get());

  BVHExpand(model1, node.uc1.get(), expand_r);
  BVHExpand(model2, node.uc2.get(), expand_r);

  node.num_max_contacts = num_max_contacts;
  node.exhaustive = exhaustive;
  node.enable_contact = enable_contact;
  node.collision_prob_threshold = collision_prob_threshold;
  node.leaf_size_threshold = leaf_size_threshold;

  return true;
}


bool initialize(PointCloudCollisionTraversalNodeOBB& node, BVHModel<OBB>& model1, BVHModel<OBB>& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2,
                BVH_REAL collision_prob_threshold = 0.5,
                int leaf_size_threshold = 1,
                int num_max_contacts = 1,
                bool exhaustive = false,
                bool enable_contact = false,
                BVH_REAL expand_r = 1);

bool initialize(PointCloudCollisionTraversalNodeRSS& node, BVHModel<RSS>& model1, BVHModel<RSS>& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2,
                BVH_REAL collision_prob_threshold = 0.5,
                int leaf_size_threshold = 1,
                int num_max_contacts = 1,
                bool exhaustive = false,
                bool enable_contact = false,
                BVH_REAL expand_r = 1);

template<typename BV>
bool initialize(PointCloudMeshCollisionTraversalNode<BV>& node, BVHModel<BV>& model1, const BVHModel<BV>& model2,
                BVH_REAL collision_prob_threshold = 0.5,
                int leaf_size_threshold = 1,
                int num_max_contacts = 1,
                bool exhaustive = false,
                bool enable_contact = false,
                BVH_REAL expand_r = 1)
{
  if(!(model1.getModelType() == BVH_MODEL_TRIANGLES || model1.getModelType() == BVH_MODEL_POINTCLOUD) || model2.getModelType() != BVH_MODEL_TRIANGLES)
    return false;

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices1 = model1.vertices;
  node.vertices2 = model2.vertices;

  node.tri_indices2 = model2.tri_indices;
  node.uc1.reset(new Uncertainty[model1.num_vertices]);

  estimateSamplingUncertainty(model1.vertices, model1.num_vertices, node.uc1.get());

  BVHExpand(model1, node.uc1.get(), expand_r);

  node.num_max_contacts = num_max_contacts;
  node.exhaustive = exhaustive;
  node.enable_contact = enable_contact;
  node.collision_prob_threshold = collision_prob_threshold;
  node.leaf_size_threshold = leaf_size_threshold;

  return true;
}

template<typename BV, bool use_refit, bool refit_bottomup>
bool initialize(PointCloudMeshCollisionTraversalNode<BV>& node, BVHModel<BV>& model1, BVHModel<BV>& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2,
                BVH_REAL collision_prob_threshold = 0.5,
                int leaf_size_threshold = 1,
                int num_max_contacts = 1,
                bool exhaustive = false,
                bool enable_contact = false,
                BVH_REAL expand_r = 1)
{
  if(!(model1.getModelType() == BVH_MODEL_TRIANGLES || model1.getModelType() == BVH_MODEL_POINTCLOUD) || model2.getModelType() != BVH_MODEL_TRIANGLES)
    return false;

  std::vector<Vec3f> vertices_transformed1(model1.num_vertices);
  for(int i = 0; i < model1.num_vertices; ++i)
  {
    Vec3f& p = model1.vertices[i];
    Vec3f new_v = MxV(R1, p) + T1;
    vertices_transformed1[i] = new_v;
  }


  std::vector<Vec3f> vertices_transformed2(model2.num_vertices);
  for(int i = 0; i < model2.num_vertices; ++i)
  {
    Vec3f& p = model2.vertices[i];
    Vec3f new_v = MxV(R2, p) + T2;
    vertices_transformed2[i] = new_v;
  }

  model1.beginReplaceModel();
  model1.replaceSubModel(vertices_transformed1);
  model1.endReplaceModel(use_refit, refit_bottomup);

  model2.beginReplaceModel();
  model2.replaceSubModel(vertices_transformed2);
  model2.endReplaceModel(use_refit, refit_bottomup);

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices1 = model1.vertices;
  node.vertices2 = model2.vertices;

  node.tri_indices2 = model2.tri_indices;
  node.uc1.reset(new Uncertainty[model1.num_vertices]);

  estimateSamplingUncertainty(model1.vertices, model1.num_vertices, node.uc1.get());

  BVHExpand(model1, node.uc1.get(), expand_r);

  node.num_max_contacts = num_max_contacts;
  node.exhaustive = exhaustive;
  node.enable_contact = enable_contact;
  node.collision_prob_threshold = collision_prob_threshold;
  node.leaf_size_threshold = leaf_size_threshold;

  return true;
}



bool initialize(PointCloudMeshCollisionTraversalNodeOBB& node, BVHModel<OBB>& model1, const BVHModel<OBB>& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2,
                BVH_REAL collision_prob_threshold = 0.5,
                int leaf_size_threshold = 1,
                int num_max_contacts = 1,
                bool exhaustive = false,
                bool enable_contact = false,
                BVH_REAL expand_r = 1);


bool initialize(PointCloudMeshCollisionTraversalNodeRSS& node, BVHModel<RSS>& model1, const BVHModel<RSS>& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2,
                BVH_REAL collision_prob_threshold = 0.5,
                int leaf_size_threshold = 1,
                int num_max_contacts = 1,
                bool exhaustive = false,
                bool enable_contact = false,
                BVH_REAL expand_r = 1);

#endif

template<typename BV>
bool initialize(MeshContinuousCollisionTraversalNode<BV>& node, const BVHModel<BV>& model1, const BVHModel<BV>& model2, int num_max_contacts = 1, bool exhaustive = false, bool enable_contact = false)
{
  if(model1.getModelType() != BVH_MODEL_TRIANGLES || model2.getModelType() != BVH_MODEL_TRIANGLES)
    return false;

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices1 = model1.vertices;
  node.vertices2 = model2.vertices;

  node.tri_indices1 = model1.tri_indices;
  node.tri_indices2 = model2.tri_indices;

  node.prev_vertices1 = model1.prev_vertices;
  node.prev_vertices2 = model2.prev_vertices;

  node.num_max_contacts = num_max_contacts;
  node.exhaustive = exhaustive;
  node.enable_contact = enable_contact;

  return true;
}

template<typename BV>
bool initialize(MeshPointCloudContinuousCollisionTraversalNode<BV>& node, const BVHModel<BV>& model1, const BVHModel<BV>& model2, int num_max_contacts = 1, bool exhaustive = false, bool enable_contact = false)
{
  if(model1.getModelType() != BVH_MODEL_TRIANGLES || !(model2.getModelType() == BVH_MODEL_TRIANGLES || model2.getModelType() == BVH_MODEL_POINTCLOUD))
    return false;

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices1 = model1.vertices;
  node.vertices2 = model2.vertices;

  node.tri_indices1 = model1.tri_indices;
  node.prev_vertices1 = model1.prev_vertices;
  node.prev_vertices2 = model2.prev_vertices;

  node.num_max_contacts = num_max_contacts;
  node.exhaustive = exhaustive;
  node.enable_contact = enable_contact;

  return true;
}

template<typename BV>
bool initialize(PointCloudMeshContinuousCollisionTraversalNode<BV>& node, const BVHModel<BV>& model1, const BVHModel<BV>& model2, int num_max_contacts = 1, bool exhaustive = false, bool enable_contact = false)
{
  if(!(model1.getModelType() == BVH_MODEL_TRIANGLES || model1.getModelType() == BVH_MODEL_POINTCLOUD) || model2.getModelType() != BVH_MODEL_TRIANGLES)
    return false;

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices1 = model1.vertices;
  node.vertices2 = model2.vertices;

  node.tri_indices2 = model2.tri_indices;
  node.prev_vertices1 = model1.prev_vertices;
  node.prev_vertices2 = model2.prev_vertices;

  node.num_max_contacts = num_max_contacts;
  node.exhaustive = exhaustive;
  node.enable_contact = enable_contact;

  return true;
}

template<typename BV>
bool initialize(MeshDistanceTraversalNode<BV>& node, const BVHModel<BV>& model1, const BVHModel<BV>& model2)
{
  if(model1.getModelType() != BVH_MODEL_TRIANGLES || model2.getModelType() != BVH_MODEL_TRIANGLES)
    return false;

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices1 = model1.vertices;
  node.vertices2 = model2.vertices;

  node.tri_indices1 = model1.tri_indices;
  node.tri_indices2 = model2.tri_indices;

  return true;
}

template<typename BV>
bool initialize(MeshDistanceTraversalNode<BV>& node, BVHModel<BV>& model1, BVHModel<BV>& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2,
                bool use_refit = false, bool refit_bottomup = false)
{
  if(model1.getModelType() != BVH_MODEL_TRIANGLES || model2.getModelType() != BVH_MODEL_TRIANGLES)
    return false;

  std::vector<Vec3f> vertices_transformed1(model1.num_vertices);
  for(int i = 0; i < model1.num_vertices; ++i)
  {
    Vec3f& p = model1.vertices[i];
    Vec3f new_v = MxV(R1, p) + T1;
    vertices_transformed1[i] = new_v;
  }


  std::vector<Vec3f> vertices_transformed2(model2.num_vertices);
  for(int i = 0; i < model2.num_vertices; ++i)
  {
    Vec3f& p = model2.vertices[i];
    Vec3f new_v = MxV(R2, p) + T2;
    vertices_transformed2[i] = new_v;
  }

  model1.beginReplaceModel();
  model1.replaceSubModel(vertices_transformed1);
  model1.endReplaceModel(use_refit, refit_bottomup);

  model2.beginReplaceModel();
  model2.replaceSubModel(vertices_transformed2);
  model2.endReplaceModel(use_refit, refit_bottomup);

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices1 = model1.vertices;
  node.vertices2 = model2.vertices;

  node.tri_indices1 = model1.tri_indices;
  node.tri_indices2 = model2.tri_indices;

  return true;
}

bool initialize(MeshDistanceTraversalNodeRSS& node, const BVHModel<RSS>& model1, const BVHModel<RSS>& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2);

template<typename BV>
bool initialize(MeshConservativeAdvancementTraversalNode<BV>& node, BVHModel<BV>& model1, BVHModel<BV>& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2, BVH_REAL w = 1,
                bool use_refit = false, bool refit_bottomup = false)
{
  if(model1.getModelType() != BVH_MODEL_TRIANGLES || model2.getModelType() != BVH_MODEL_TRIANGLES)
    return false;

  std::vector<Vec3f> vertices_transformed1(model1.num_vertices);
  for(int i = 0; i < model1.num_vertices; ++i)
  {
    Vec3f& p = model1.vertices[i];
    Vec3f new_v = MxV(R1, p) + T1;
    vertices_transformed1[i] = new_v;
  }


  std::vector<Vec3f> vertices_transformed2(model2.num_vertices);
  for(int i = 0; i < model2.num_vertices; ++i)
  {
    Vec3f& p = model2.vertices[i];
    Vec3f new_v = MxV(R2, p) + T2;
    vertices_transformed2[i] = new_v;
  }

  model1.beginReplaceModel();
  model1.replaceSubModel(vertices_transformed1);
  model1.endReplaceModel(use_refit, refit_bottomup);

  model2.beginReplaceModel();
  model2.replaceSubModel(vertices_transformed2);
  model2.endReplaceModel(use_refit, refit_bottomup);

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices1 = model1.vertices;
  node.vertices2 = model2.vertices;

  node.tri_indices1 = model1.tri_indices;
  node.tri_indices2 = model2.tri_indices;

  node.w = w;

  //HOW?
  //node.motion1 = new InterpMotion<BV>
  //node.motion2 = new InterpMotion<BV>

  return true;
}


inline bool initialize(MeshConservativeAdvancementTraversalNodeRSS& node, const BVHModel<RSS>& model1, const BVHModel<RSS>& model2,
                const Vec3f R1[3], const Vec3f& T1, const Vec3f R2[3], const Vec3f& T2, BVH_REAL w = 1)
{
  if(model1.getModelType() != BVH_MODEL_TRIANGLES || model2.getModelType() != BVH_MODEL_TRIANGLES)
    return false;

  node.model1 = &model1;
  node.model2 = &model2;

  node.vertices1 = model1.vertices;
  node.vertices2 = model2.vertices;

  node.tri_indices1 = model1.tri_indices;
  node.tri_indices2 = model2.tri_indices;

  node.w = w;

  relativeTransform(R1, T1, R2, T2, node.R, node.T);



  return true;
}

}

#endif