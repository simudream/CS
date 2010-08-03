/*
 *  LodGen.cpp
 *  cs
 *
 *  Created by Eduardo Poyart on 6/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
using namespace std;

#include <crystalspace.h>
#include "LodGen.h"

inline float dot(const csVector3& v0, const csVector3& v1) { return v0 * v1; }

void PointTriangleDistance(const csVector3& P, const csVector3& P0, const csVector3& P1, const csVector3& P2, float& s, float& t, float& d2)
{
  // From http://www.geometrictools.com/Documentation/DistancePoint3Triangle3.pdf
  csVector3 diff = P0 - P;
  csVector3 edge0 = P1 - P0;
  csVector3 edge1 = P2 - P0;
  float a00 = dot(edge0, edge0);
  float a01 = dot(edge0, edge1);
  float a11 = dot(edge1, edge1);
  float b0 = dot(diff, edge0);
  float b1 = dot(diff, edge1);
  float c = dot(diff, diff);
  float det = fabs(a00*a11 - a01*a01);
  s = a01*b1 - a11*b0;
  t = a01*b0 - a00*b1;
  
  if (s + t <= det)
  {
    if (s < 0.0)
    {
      if (t < 0.0)  // region 4
      {
        if (b0 < 0.0)
        {
          t = 0.0;
          if (-b0 >= a00)
          {
            s = 1.0;
            d2 = a00 + 2.0*b0 + c;
          }
          else
          {
            s = -b0/a00;
            d2 = b0*s + c;
          }
        }
        else
        {
          s = 0.0;
          if (b1 >= 0.0)
          {
            t = 0.0;
            d2 = c;
          }
          else if (-b1 >= a11)
          {
            t = 1.0;
            d2 = a11 + 2.0*b1 + c;
          }
          else
          {
            t = -b1/a11;
            d2 = b1*t + c;
          }
        }
      }
      else  // region 3
      {
        s = 0.0;
        if (b1 >= 0.0)
        {
          t = 0.0;
          d2 = c;
        }
        else if (-b1 >= a11)
        {
          t = 1.0;
          d2 = a11 + 2.0*b1 + c;
        }
        else
        {
          t = -b1/a11;
          d2 = b1*t + c;
        }
      }
    }
    else if (t < 0.0)  // region 5
    {
      t = 0.0;
      if (b0 >= 0.0)
      {
        s = 0.0;
        d2 = c;
      }
      else if (-b0 >= a00)
      {
        s = 1.0;
        d2 = a00 + 2.0*b0 + c;
      }
      else
      {
        s = -b0/a00;
        d2 = b0*s + c;
      }
    }
    else  // region 0
    {
      float invDet = 1.0/det;
      s *= invDet;
      t *= invDet;
      d2 = s*(a00*s + a01*t + 2.0*b0) + t*(a01*s + a11*t + 2.0*b1) + c;
    }
  }
  else
  {
    if (s < 0.0)  // region 2
    {
      float tmp0 = a01 + b0;
      float tmp1 = a11 + b1;
      if (tmp1 > tmp0)
      {
        float numer = tmp1 - tmp0;
        float denom = a00 - 2.0*a01 + a11;
        if (numer >= denom)
        {
          s = 1.0;
          t = 0.0;
          d2 = a00 + 2.0*b0 + c;
        }
        else
        {
          s = numer/denom;
          t = 1.0 - s;
          d2 = s*(a00*s + a01*t + 2.0*b0) + t*(a01*s + a11*t + 2.0*b1) + c;
        }
      }
      else
      {
        s = 0.0;
        if (tmp1 <= 0.0)
        {
          t = 1.0;
          d2 = a11 + 2.0*b1 + c;
        }
        else if (b1 >= 0.0)
        {
          t = 0.0;
          d2 = c;
        }
        else
        {
          t = -b1/a11;
          d2 = b1*t + c;
        }
      }
    }
    else if (t < 0.0)  // region 6
    {
      float tmp0 = a01 + b1;
      float tmp1 = a00 + b0;
      if (tmp1 > tmp0)
      {
        float numer = tmp1 - tmp0;
        float denom = a00 - 2.0*a01 + a11;
        if (numer >= denom)
        {
          t = 1.0;
          s = 0.0;
          d2 = a11 + 2.0*b1 + c;
        }
        else
        {
          t = numer/denom;
          s = 1.0 - t;
          d2 = s*(a00*s + a01*t + 2.0*b0) + t*(a01*s + a11*t + 2.0*b1) + c;
        }
      }
      else
      {
        t = 0.0;
        if (tmp1 <= 0.0)
        {
          s = 1.0;
          d2 = a00 + 2.0*b0 + c;
        }
        else if (b0 >= 0.0)
        {
          s = 0.0;
          d2 = c;
        }
        else
        {
          s = -b0/a00;
          d2 = b0*s + c;
        }
      }
    }
    else  // region 1
    {
      float numer = a11 + b1 - a01 - b0;
      if (numer <= 0.0)
      {
        s = 0.0;
        t = 1.0;
        d2 = a11 + 2.0*b1 + c;
      }
      else
      {
        float denom = a00 - 2.0*a01 + a11;
        if (numer >= denom)
        {
          s = 1.0;
          t = 0.0;
          d2 = a00 + 2.0*b0 + c;
        }
        else
        {
          s = numer/denom;
          t = 1.0 - s;
          d2 = s*(a00*s + a01*t + 2.0*b0) + t*(a01*s + a11*t + 2.0*b1) + c;
        }
      }
    }
  }
  
  // Account for numerical round-off error
  if (d2 < 0.0)
  {
    d2 = 0.0;
  }
}

void unittest1(const csVector3& p0, const csVector3& p1, const csVector3& p2, const csVector3& p, float expected)
{
  printf("p = %6.4g, %6.4g, %6.4g        ", p.x, p.y, p.z);
  float s, t, d2;
  PointTriangleDistance(p, p0, p1, p2, s, t, d2);
  csVector3 c(p0 + s*(p1-p0) + t*(p2-p0));
  float d = sqrtf(d2);
  printf("d = %6.4g       c = %6.4g, %6.4g, %6.4g\n", d, c.x, c.y, c.z);
  assert(expected == -1.0 || fabs(expected - d) < 0.0001);
}

void unittests(float z)
{
  csVector3 p0(1.0, 1.0, z);
  csVector3 p1(2.0, 2.0, z);
  csVector3 p2(3.0, 1.0, z);
  unittest1(p0, p1, p2, csVector3(1.0, 1.0, z), 0.0);
  unittest1(p0, p1, p2, csVector3(2.0, 2.0, z), 0.0);
  unittest1(p0, p1, p2, csVector3(3.0, 1.0, z), 0.0);
  unittest1(p0, p1, p2, csVector3(2.0, 1.5, z), 0.0);
  unittest1(p0, p1, p2, csVector3(1.0, 1.0, z + 1.0), 1.0);
  unittest1(p0, p1, p2, csVector3(2.0, 2.0, z + 1.0), 1.0);
  unittest1(p0, p1, p2, csVector3(3.0, 1.0, z + 1.0), 1.0);
  unittest1(p0, p1, p2, csVector3(2.0, 1.5, z + 1.0), 1.0);
  
  unittest1(p0, p1, p2, csVector3(0.0, 0.0, z), sqrtf(2.0)); 
  unittest1(p0, p1, p2, csVector3(0.5, 0.5, z), sqrtf(2.0) / 2.0); 
  unittest1(p0, p1, p2, csVector3(0.0, 1.0, z), 1.0); 
  unittest1(p0, p1, p2, csVector3(0.5, 1.0, z), 0.5);  
  unittest1(p0, p1, p2, csVector3(1.5, 1.5, z), 0.0); 
  unittest1(p0, p1, p2, csVector3(1.5, 2.0, z), 0.3536); 
  unittest1(p0, p1, p2, csVector3(2.0, 3.0, z), 1.0); 
  unittest1(p0, p1, p2, csVector3(2.5, 2.0, z), 0.3536); 
  unittest1(p0, p1, p2, csVector3(2.5, 1.5, z), 0.0); 
  unittest1(p0, p1, p2, csVector3(3.5, 1.0, z), 0.5);  
  unittest1(p0, p1, p2, csVector3(4.0, 1.0, z), 1.0); 
  unittest1(p0, p1, p2, csVector3(3.5, 0.5, z), sqrtf(2.0) / 2.0); 
  unittest1(p0, p1, p2, csVector3(4.0, 0.0, z), sqrtf(2.0)); 
  unittest1(p0, p1, p2, csVector3(2.5, 0.0, z), 1.0); 
  unittest1(p0, p1, p2, csVector3(2.0, 1.0, z), 0.0); 
  unittest1(p0, p1, p2, csVector3(1.5, 0.0, z), 1.0); 
}  

void PointTriangleDistanceUnitTests()
{
  unittests(0.0);
  unittests(1.0);
}

void LodGen::Init(iGeneralFactoryState* fstate, int submesh_index)
{
  csVertexListWalker<float, csVector3> fstate_vertices(fstate->GetRenderBuffer(CS_BUFFER_POSITION));
  for (unsigned int i = 0; i < fstate_vertices.GetSize(); i++)
  {
    vertices.Push(*fstate_vertices);
    ++fstate_vertices;
  }

  csRef<iGeneralMeshSubMesh> submesh = fstate->GetSubMesh(submesh_index);
  assert(submesh);
  csRef<iRenderBuffer> index_buffer = submesh->GetIndices();
  assert(index_buffer);
  CS::TriangleIndicesStream<size_t> fstate_triangles(index_buffer, CS_MESHTYPE_TRIANGLES);
  
  while(fstate_triangles.HasNext())
  {
    const CS::TriangleT<size_t> ttri(fstate_triangles.Next());
    csTriangle tri;
    for (int i = 0; i < 3; i++)
      tri[i] = ttri[i];
    triangles.Push(tri);
  }
  InitCoincidentVertices();
}

void LodGen::InitCoincidentVertices()
{
  coincident_vertices.SetSize(vertices.GetSize());
  static const float epsilon = 0.00001; 
  for (unsigned int i = 0; i < vertices.GetSize(); i++)
  {
    const csVector3& v = vertices[i]; 
    for (unsigned int j = 0; j < vertices.GetSize(); j++)
    {
      if (i == j)
        continue;
      if (fabs(v[0] - vertices[j][0]) < epsilon && fabs(v[1] - vertices[j][1]) < epsilon && fabs(v[2] - vertices[j][2]) < epsilon)
        coincident_vertices[i].Push(j);
    }
  }
}

float LodGen::SumOfSquareDist(const WorkMesh& k) const
{
  float s, t, d2;
  float sum = 0.0;
  const SlidingWindow& sw = k.GetLastWindow();
  // Vertex-to-mesh
  for (unsigned int i = 0; i < vertices.GetSize(); i++)
  {
    const csVector3& v = vertices[i];
    float min_d2 = FLT_MAX;
    for (int j = sw.start_index; j < sw.end_index; j++)
    {
      const csTriangle& tri = k.GetTriangle(j);
      const csVector3& p0 = vertices[tri[0]];
      const csVector3& p1 = vertices[tri[1]];
      const csVector3& p2 = vertices[tri[2]];
      PointTriangleDistance(v, p0, p1, p2, s, t, d2);
      if (d2 < min_d2)
      {
        min_d2 = d2;
        if (min_d2 == 0.0)
          break;
      }
    }
    assert(min_d2 < FLT_MAX);
    sum += min_d2;
  }
  // Barycenter-to-mesh
  const SlidingWindow& sw0 = k.sliding_windows[0];  
  for (int i = sw0.start_index; i < sw0.end_index; i++)
  {
    const csTriangle& tri = k.GetTriangle(i);
    csVector3 b = (vertices[tri[0]] + vertices[tri[1]] + vertices[tri[2]]) / 3.0;
    float min_d2 = FLT_MAX;
    for (int j = sw.start_index; j < sw.end_index; j++)
    {
      const csTriangle& tri0 = k.GetTriangle(j);
      const csVector3& p0 = vertices[tri0[0]];
      const csVector3& p1 = vertices[tri0[1]];
      const csVector3& p2 = vertices[tri0[2]];
      PointTriangleDistance(b, p0, p1, p2, s, t, d2);
      if (d2 < min_d2)
      {
        min_d2 = d2;
        if (min_d2 == 0.0)
          break;
      }
    }
    assert(min_d2 < FLT_MAX);
    sum += min_d2;
  }
  return sum;
}

float LodGen::SumOfSquareDist(const WorkMesh& k, int start_index) const
{
  float s, t, d2;
  float sum = 0.0;
  const SlidingWindow& sw = k.GetLastWindow();
  const SlidingWindow& sw0 = k.sliding_windows[0];
  int samples_per_triangle = 20 / (sw.end_index - start_index);
  if (samples_per_triangle == 0)
    samples_per_triangle = 1;

  for (int i = start_index; i < sw.end_index; i++)
  {
    const csTriangle& tri = k.GetTriangle(i);
    const csVector3& q0 = vertices[tri[0]];
    const csVector3& q1 = vertices[tri[1]];
    const csVector3& q2 = vertices[tri[2]];
    for (int m = 0; m < samples_per_triangle; m++)
    {
      float r0, r1;
      do
      {
        r0 = (float)rand() / RAND_MAX;
        r1 = (float)rand() / RAND_MAX;
      }
      while (r0 + r1 > 1.0);
      float r2 = 1.0 - r0 - r1;
      assert(r0 + r1 + r2 == 1.0);
      csVector3 b = r0 * q0 + r1 * q1 + r2 * q2;
      float min_d2 = FLT_MAX;
      for (int j = sw0.start_index; j < sw0.end_index; j++)
      {
        const csTriangle& tri0 = k.GetTriangle(j);
        const csVector3& p0 = vertices[tri0[0]];
        const csVector3& p1 = vertices[tri0[1]];
        const csVector3& p2 = vertices[tri0[2]];
        PointTriangleDistance(b, p0, p1, p2, s, t, d2);
        if (d2 < min_d2)
        {
          min_d2 = d2;
          if (min_d2 == 0.0)
            break;
        }
      }
      assert(min_d2 < FLT_MAX);
      sum += min_d2;
    }
  }
  return sum;
}

/*
// TODO
float LodGen::SumOfSquareDist(const WorkMesh& k, int start_index) const
{
  float s, t, d2;
  float sum = 0.0;
  const SlidingWindow& sw = k.GetLastWindow();
  const SlidingWindow& sw2 = k.sliding_windows[k.sliding_windows.GetSize()-2];
  const SlidingWindow& sw0 = k.sliding_windows[0];
  assert(start_index == sw2.end_index);
  csArray<csVector3> verts;
  csVector3 c(0.0, 0.0, 0.0);
  if (sw2.end_index < sw.end_index)
  {
    for (int i = sw2.end_index; i < sw.end_index; i++)
    {
      const csTriangle& tri = k.GetTriangle(i);
      for (int j = 0; j < 3; j++)
        verts.Push(vertices[tri[j]]);
    }
  }
  else
  {
    assert(sw2.start_index < sw.start_index);
    for (int i = sw2.start_index; i < sw.start_index; i++)
    {
      const csTriangle& tri = k.GetTriangle(i);
      for (int j = 0; j < 3; j++)
        verts.Push(vertices[tri[j]]);
    }
  }
  
  for (int i = 0; i < verts.GetSize(); i++)
    c += verts[i];
  c /= (float)verts.GetSize();
  float r2 = 0.0;
  for (int i = 0; i < verts.GetSize(); i++)
  {
    float r2b = (verts[i]-c).SquaredNorm();
    if (r2b > r2)
      r2 = r2b;
  }
    
  csArray<int> ntris;
  for (int i = sw0.start_index; i < sw0.end_index; i++)
  {
    const csTriangle& tri = k.GetTriangle(i);
    csVector3 b = (vertices[tri[0]] + vertices[tri[1]] + vertices[tri[2]]) / 3.0;
    if ((b-c).SquaredNorm() <= r2)
      ntris.Push(i);
  }
  
  for (int i = 0; i < ntris.
}
*/    

void LodGen::RemoveTriangleFromIncidentTris(WorkMesh& k, int itri)
{
  csTriangle& tri = k.tri_buffer[itri];
  for (int i = 0; i < 3; i++)
  {
    bool result = k.incident_tris[tri[i]].Delete(itri);
    assert(result);
  }
}

inline bool LodGen::IsDegenerate(const csTriangle& tri) const
{
  return tri[0] == tri[1] || tri[0] == tri[2] || tri[1] == tri[2];
}

bool LodGen::IsTriangleCoincident(const csTriangle& t0, const csTriangle& t1) const
{
  for (int i = 0; i < 3; i++)
    if (t0[i] != t1[0] && t0[i] != t1[1] && t0[i] != t1[2])
      return false;
  return true;
}

bool LodGen::IsCoincident(const WorkMesh& k, const csTriangle& tri) const
{
  assert(!IsDegenerate(tri));
  for (int i = 0; i < 3; i++)
  {
    const IncidentTris& incident = k.incident_tris[tri[i]];
    for (unsigned int j = 0; j < incident.GetSize(); j++)
    {
      assert(!IsDegenerate(k.tri_buffer[incident[j]]));
      if (IsTriangleCoincident(k.tri_buffer[incident[j]], tri))
        return true;
    }
  }
  return false;
}

int LodGen::FindInWindow(const WorkMesh& k, const SlidingWindow& sw, int itri) const
{
  for (int i = sw.start_index; i < sw.end_index; i++)
    if (k.tri_indices[i] == itri)
      return i;
  assert(0);
}

void LodGen::SwapIndex(WorkMesh& k, int i0, int i1)
{
  int temp = k.tri_indices[i0];
  k.tri_indices[i0] = k.tri_indices[i1];
  k.tri_indices[i1] = temp;
}

bool LodGen::Collapse(WorkMesh& k, int v0, int v1)
{
  SlidingWindow sw = k.GetLastWindow(); // copy
  
  IncidentTris incident = k.incident_tris[v0]; // copy
  for (unsigned int i = 0; i < incident.GetSize(); i++)
  {
    int itri = incident[i];
    int h = FindInWindow(k, sw, itri);
    if (h >= top_limit)
      return false;
    csTriangle new_tri = k.tri_buffer[itri]; // copy
    RemoveTriangleFromIncidentTris(k, itri);
    SwapIndex(k, sw.start_index, h);    
    //cout << "Rem " << itri << " = " << new_tri[0] << " " << new_tri[1] << " " << new_tri[2] << endl;
    sw.start_index++;

    assert(incident.GetSize() > k.incident_tris[v0].GetSize());
    for (int j = 0; j < 3; j++)
      if (new_tri[j] == v0)
        new_tri[j] = v1;
    if (!IsDegenerate(new_tri) && !IsCoincident(k, new_tri))
    {
      k.AddTriangle(new_tri);
      //cout << "Add " << k.tri_buffer.GetSize()-1 << " = " << new_tri[0] << " " << new_tri[1] << " " << new_tri[2] << endl;
      sw.end_index++;
    }
  }
  k.sliding_windows.Push(sw);

  /*
  for (int i = sw.start_index; i < sw.end_index; i++)
    for (int j = 0; j < 3; j++)
      assert(k.GetTriangle(i)[j] != v0);
  */
  return true;
}

struct MeshVerification
{
  Edge e;
  int num_t;
  bool operator==(const MeshVerification& m) const { return e == m.e; }
};

void LodGen::VerifyMesh(WorkMesh& k)
{
  return;
  csArray<MeshVerification> mvs;
  const SlidingWindow& sw = k.GetLastWindow();
  for (int i = sw.start_index; i < sw.end_index; i++)
  {
    const csTriangle& tri = k.GetTriangle(i);
    for (int j = sw.start_index; j < sw.end_index; j++)
    {
      if (i == j)
        continue;
      const csTriangle& tri2 = k.GetTriangle(j);
      if (IsTriangleCoincident(tri, tri2))
        assert(0);
    }
  }
  /*
  for (int i = sw.start_index; i < sw.end_index; i++)
  {
    const csTriangle& tri = k.GetTriangle(i);
    for (int j = 0; j < 3; j++)
    {
      Edge e(tri[j], tri[(j+1)%3]);
      MeshVerification mv;
      mv.e = e;
      size_t i = mvs.Find(mv);
      if (i == csArrayItemNotFound)
      {
        mv.num_t = 1;
        mvs.Push(mv);
      }
      else
      {
        mvs[i].num_t++;
      }
    }
  }
  for (unsigned int i = 0; i < mvs.GetSize(); i++)
  {
    assert(mvs[i].num_t == 2);
  }
  */
}

void LodGen::GenerateLODs()
{
  k.incident_tris.SetSize(vertices.GetSize());
  for (unsigned int i = 0; i < triangles.GetSize(); i++)
    k.AddTriangle(triangles[i]);
  
  SlidingWindow sw_initial;
  sw_initial.start_index = 0;
  sw_initial.end_index = triangles.GetSize();
  top_limit = sw_initial.end_index;
  k.sliding_windows.Push(sw_initial);
  int collapse_counter = 0;
  int min_num_triangles = triangles.GetSize() / 6;
  int min_triangles_for_replication = triangles.GetSize() / 2;
  csArray<Edge> edges;
  bool could_not_collapse = false;
  int edge_start = -1;
  
  while (1)
  {
    float min_d = FLT_MAX;
    int min_v0, min_v1;    
    SlidingWindow sw = k.GetLastWindow();
    edges.SetSize(0);
    for (int itri = sw.start_index; itri < top_limit; itri++)
    {
      const csTriangle& tri = k.GetTriangle(itri);
      for (int iv = 0; iv < 3; iv++)
        if (coincident_vertices[tri[iv]].GetSize() == 0)
          edges.PushSmart(Edge(tri[iv], tri[(iv+1)%3]));
    }
    int edge_step = edges.GetSize() / 5 + 1;
    edge_start = (edge_start + 1) % edge_step;
      
    for (unsigned int i = edge_start; i < edges.GetSize(); i += edge_step)
    {
      int v0 = edges[i].v0;
      int v1 = edges[i].v1;
      
      WorkMesh k_prime = k;
      bool result = Collapse(k_prime, v0, v1);
      if (result)
      {
        //VerifyMesh(k_prime);
        float d = SumOfSquareDist(k_prime);
        if (d < min_d)
        {
          min_d = d;
          min_v0 = v0;
          min_v1 = v1;
        }
      }
      if (min_d == 0.0)
        break;
    }
    if (min_d == FLT_MAX && could_not_collapse)
    {
      cout << "No more triangles to collapse" << endl;
      break;
    }
    if (min_d != FLT_MAX)
    {
      bool result = Collapse(k, min_v0, min_v1);
      assert(result);
      sw = k.GetLastWindow();
      cout << "t: " << sw.end_index-sw.start_index << " d: " << min_d << " v: " << min_v0 << "->" << min_v1 << endl;
      VerifyMesh(k);
      collapse_counter++;
      could_not_collapse = false;
      /*
      cout << "T: ";
      for (unsigned int i = 0; i < k.tri_indices.GetSize(); i++)
        cout << i << "=" << k.tri_indices[i] << " ";
      cout << endl << "W: ";
      for (unsigned int i = 0; i < k.sliding_windows.GetSize(); i++)
        cout << k.sliding_windows[i].start_index << "-" << k.sliding_windows[i].end_index << " ";
      cout << endl << "Top limit = " << top_limit << endl;
      */
    }
    
    int curr_num_triangles = sw.end_index - sw.start_index;
    if (curr_num_triangles < min_num_triangles)
    {
      cout << "Reached minimum number of triangles" << endl;
      break;
    }
    if (curr_num_triangles < min_triangles_for_replication || min_d == FLT_MAX)
    {
      if (min_d == FLT_MAX)
        could_not_collapse = true;
      // Replicate index buffer
      cout << "Replicating: " << curr_num_triangles << endl;
      sw.start_index += curr_num_triangles;
      sw.end_index += curr_num_triangles;
      k.SetLastWindow(sw);
      top_limit = sw.end_index;
      for (int i = sw.start_index; i < sw.end_index; i++)
        k.tri_indices.Push(k.tri_indices[i-curr_num_triangles]);
      VerifyMesh(k);
      min_triangles_for_replication = (sw.end_index - sw.start_index) / 2;
    }
  }
  for (unsigned int i = 0; i < k.tri_indices.GetSize(); i++)
    ordered_tris.Push(k.GetTriangle(i));
  for (unsigned int i = 0; i < ordered_tris.GetSize(); i++)
    for (unsigned int j = 0; j < 3; j++)
      assert(ordered_tris[i][j] >= 0 && ordered_tris[i][j] < (int)vertices.GetSize());
  cout << "End" << endl;
}

