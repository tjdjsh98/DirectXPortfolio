#include "pch.h"
#include "Quadtree.h"

Quadtree::Quadtree()
{
}

Quadtree::~Quadtree()
{
}

void Quadtree::Build(vector<Vector3>* vertices,const vector<uint32>& indices)
{
	_vertices = vertices;

	BoundingBox sceneBounds = BuildAABB();

	_root = make_shared<QuadTreeNode>();
	_root->Bounds = sceneBounds;

	BuildQuadtree(_root, indices);
}

bool Quadtree::RayCastQuadIntersect(Vector3 rayPos, Vector3 rayDir)
{
	_distance = FLT_MAX;
	return RayQuadtreeIntersect(_root, rayPos, rayDir);
}

BoundingBox Quadtree::BuildAABB()
{
	Vector3 vmin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3 vmax = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (size_t i = 0; i < (*_vertices).size(); i++)
	{
		Vector3 P = (*_vertices)[i];

		vmin = Vector3::Min(vmin, P);
		vmax = Vector3::Max(vmax, P);
	}
	vmax.y = FLT_MAX / 3;
	vmin.y = -FLT_MAX / 3;
	BoundingBox bounds;
	bounds.Center = (vmin + vmax) * 0.5f;
	bounds.Extents = (vmax - vmin) * 0.5f;

	return bounds;
}

void Quadtree::BuildQuadtree(shared_ptr<QuadTreeNode> parent, const std::vector<uint32>& indices)
{
	size_t triCount = indices.size() / 3;

	if (triCount < 60)
	{
		parent->isLeaf = true;
		parent->indices = indices;
	}
	else
	{
		parent->isLeaf = false;

		BoundingBox subbox[4];
		parent->Subdvide(subbox);

		for (int8 i = 0; i < 4; i++)
		{
			parent->children[i] = make_shared<QuadTreeNode>();
			parent->children[i]->Bounds = subbox[i];

			vector<uint32> intersectTriangleIndices;

			for (size_t j = 0; j < triCount; j++)
			{
				uint32 i0 = indices[j * 3 + 0];
				uint32 i1 = indices[j * 3 + 1];
				uint32 i2 = indices[j * 3 + 2];

				Vector3 v0 = (*_vertices)[i0];
				Vector3 v1 = (*_vertices)[i1];
				Vector3 v2 = (*_vertices)[i2];

				if (subbox[i].Intersects(v0, v1, v2))
				{
					intersectTriangleIndices.push_back(i0);
					intersectTriangleIndices.push_back(i1);
					intersectTriangleIndices.push_back(i2);
				}
			}
			BuildQuadtree(parent->children[i], intersectTriangleIndices);
		}
	}
}

bool Quadtree::RayQuadtreeIntersect(shared_ptr<QuadTreeNode> parent, Vector3 rayPos, Vector3 rayDir)
{
	
	if (!parent->isLeaf)
	{
		bool success = false;
		for (uint8 i = 0; i < 4; i++)
		{
			float t;

			if (parent->children[i]->Bounds.Intersects(rayPos, rayDir,OUT t))
			{
				if (RayQuadtreeIntersect(parent->children[i], rayPos, rayDir))
					success = true;
			}
		}

		return success;
	}
	else
	{
		size_t triCount = parent->indices.size() / 3;

		bool success = false;
		for (size_t i = 0; i < triCount; i++)
		{
			uint32 i0 = parent->indices[i * 3 + 0];
			uint32 i1 = parent->indices[i * 3 + 1];
			uint32 i2 = parent->indices[i * 3 + 2];

			Vector3 v0 = (*_vertices)[i0];
			Vector3 v1 = (*_vertices)[i1];
			Vector3 v2 = (*_vertices)[i2];

			float t;

			if (TriangleTests::Intersects(rayPos, rayDir, v0, v1, v2, OUT t))
			{
				if (_distance > t)
				{

					_intersectedIndices.clear();
					_distance = t;
					_intersectedIndices.push_back(i0);
					_intersectedIndices.push_back(i1);
					_intersectedIndices.push_back(i2);
					success = true;
				}
			}
		}
		return success;
	}
}
