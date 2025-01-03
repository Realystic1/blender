/* SPDX-FileCopyrightText: 2006 NaN Holding BV. All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once

/** \file
 * \ingroup bke
 *
 * This header encapsulates necessary code to build a BVH.
 */

#include <mutex>

#include "BLI_bit_span.hh"
#include "BLI_index_mask_fwd.hh"
#include "BLI_kdopbvh.hh"
#include "BLI_math_vector_types.hh"
#include "BLI_span.hh"

struct BVHCache;
struct BVHTree;
struct MFace;
struct Mesh;
struct PointCloud;

/**
 * Struct that stores basic information about a #BVHTree built from a mesh.
 */
struct BVHTreeFromMesh {
  const BVHTree *tree = nullptr;

  /** Default callbacks to BVH nearest and ray-cast. */
  BVHTree_NearestPointCallback nearest_callback;
  BVHTree_RayCastCallback raycast_callback;

  /* Vertex array, so that callbacks have instant access to data. */
  blender::Span<blender::float3> vert_positions;
  blender::Span<blender::int2> edges;
  blender::Span<int> corner_verts;
  blender::Span<blender::int3> corner_tris;

  const MFace *face = nullptr;

  std::unique_ptr<BVHTree, BVHTreeDeleter> owned_tree;
};

enum BVHCacheType {
  BVHTREE_FROM_VERTS,
  BVHTREE_FROM_EDGES,
  BVHTREE_FROM_FACES,
  BVHTREE_FROM_CORNER_TRIS,
  BVHTREE_FROM_CORNER_TRIS_NO_HIDDEN,

  BVHTREE_FROM_LOOSEVERTS,
  BVHTREE_FROM_LOOSEEDGES,

  /* These types consider geometry visibility when getting loose elements.
   * NOTE: If the element is linked to a face or edge that is hidden, but the element itself is not
   * hidden, it is considered a loose element. */
  BVHTREE_FROM_LOOSEVERTS_NO_HIDDEN,
  BVHTREE_FROM_LOOSEEDGES_NO_HIDDEN,

  /* Keep `BVHTREE_MAX_ITEM` as last item. */
  BVHTREE_MAX_ITEM,
};

/**
 * Builds a BVH-tree where nodes are the given vertices (NOTE: does not copy given `vert`!).
 * \param vert_allocated: if true, vert freeing will be done when freeing data.
 * \param verts_mask: if not null, true elements give which vert to add to BVH-tree.
 * \param verts_num_active: if >= 0, number of active verts to add to BVH-tree
 * (else will be computed from `verts_mask`).
 */
BVHTree *bvhtree_from_mesh_verts_ex(BVHTreeFromMesh *data,
                                    blender::Span<blender::float3> vert_positions,
                                    blender::BitSpan verts_mask,
                                    int verts_num_active,
                                    float epsilon,
                                    int tree_type,
                                    int axis);

/**
 * Builds a BVH-tree where nodes are the given edges.
 * \param vert, vert_allocated: if true, elem freeing will be done when freeing data.
 * \param edge, edge_allocated: if true, elem freeing will be done when freeing data.
 * \param edges_mask: if not null, true elements give which vert to add to BVH-tree.
 * \param edges_num_active: if >= 0, number of active edges to add to BVH-tree
 * (else will be computed from `edges_mask`).
 */
BVHTree *bvhtree_from_mesh_edges_ex(BVHTreeFromMesh *data,
                                    blender::Span<blender::float3> vert_positions,
                                    blender::Span<blender::int2> edges,
                                    blender::BitSpan edges_mask,
                                    int edges_num_active,
                                    float epsilon,
                                    int tree_type,
                                    int axis);

/**
 * Builds a BVH-tree where nodes are the triangle faces (#Mesh::corner_tris()) of the given mesh.
 */
BVHTree *bvhtree_from_mesh_corner_tris_ex(BVHTreeFromMesh *data,
                                          blender::Span<blender::float3> vert_positions,
                                          blender::Span<int> corner_verts,
                                          blender::Span<blender::int3> corner_tris,
                                          blender::BitSpan corner_tris_mask,
                                          int corner_tris_num_active,
                                          float epsilon,
                                          int tree_type,
                                          int axis);

/**
 * Build a bvh tree from the triangles in the mesh that correspond to the faces in the given mask.
 */
void BKE_bvhtree_from_mesh_tris_init(const Mesh &mesh,
                                     const blender::IndexMask &faces_mask,
                                     BVHTreeFromMesh &r_data);

/**
 * Build a bvh tree containing the given edges.
 */
void BKE_bvhtree_from_mesh_edges_init(const Mesh &mesh,
                                      const blender::IndexMask &edges_mask,
                                      BVHTreeFromMesh &r_data);

/**
 * Build a bvh tree containing the given vertices.
 */
void BKE_bvhtree_from_mesh_verts_init(const Mesh &mesh,
                                      const blender::IndexMask &verts_mask,
                                      BVHTreeFromMesh &r_data);

/**
 * Math functions used by callbacks
 */
float bvhtree_ray_tri_intersection(
    const BVHTreeRay *ray, float m_dist, const float v0[3], const float v1[3], const float v2[3]);
float bvhtree_sphereray_tri_intersection(const BVHTreeRay *ray,
                                         float radius,
                                         float m_dist,
                                         const float v0[3],
                                         const float v1[3],
                                         const float v2[3]);

struct BVHTreeFromPointCloud {
  BVHTree *tree;

  BVHTree_NearestPointCallback nearest_callback;

  const float (*coords)[3];
};

void BKE_bvhtree_from_pointcloud_get(const PointCloud &pointcloud,
                                     const blender::IndexMask &points_mask,
                                     BVHTreeFromPointCloud &r_data);

void free_bvhtree_from_pointcloud(BVHTreeFromPointCloud *data);
