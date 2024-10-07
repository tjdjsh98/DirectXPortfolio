#pragma once

struct QuadTreeNode;

class Quadtree
{
public:
	Quadtree();
	~Quadtree();

	void Build(vector<Vector3>* vertices,const vector<uint32>& indices);
	bool RayCastQuadIntersect(Vector3 rayPos, Vector3 rayDir);

	const vector<uint32>& GetIntersectedIndices() { return _intersectedIndices; }
	float GetDistance() { return _distance; }
private:
	BoundingBox BuildAABB();
	void BuildQuadtree(shared_ptr<QuadTreeNode> parent, const std::vector<uint32>& indices);
	bool RayQuadtreeIntersect(shared_ptr<QuadTreeNode> parent, Vector3 rayPos, Vector3 rayDir);

private:
	shared_ptr<QuadTreeNode> _root;
	vector<Vector3>* _vertices;
	float _distance;

	vector<uint32> _intersectedIndices;
};


struct QuadTreeNode
{
	BoundingBox Bounds;

	vector<uint32> indices;
	shared_ptr<QuadTreeNode> children[4];
	bool isLeaf;

	QuadTreeNode()
	{
		for (int i = 0; i < 4; i++)
		{
			children[i] = 0;
		}

		Bounds.Center = Vector3(0.0f, 0.0f, 0.0f);
		Bounds.Extents = Vector3(0.0f, 0.0f, 0.0f);
		isLeaf = false;
	}

	void Subdvide(BoundingBox box[4])
	{
		Vector3 halfExtent(
			0.5f * Bounds.Extents.x,
			Bounds.Extents.y,
			0.5f * Bounds.Extents.z
		);

		// LeftTop
		box[0].Center = Vector3(
			Bounds.Center.x - halfExtent.x,
			Bounds.Center.y,
			Bounds.Center.z + halfExtent.z
		);
		box[0].Extents = halfExtent;
		// RIghtTop
		box[1].Center = Vector3(
			Bounds.Center.x + halfExtent.x,
			Bounds.Center.y,
			Bounds.Center.z + halfExtent.z
		);
		box[1].Extents = halfExtent;
		// LeftBottom
		box[2].Center = Vector3(
			Bounds.Center.x - halfExtent.x,
			Bounds.Center.y,
			Bounds.Center.z - halfExtent.z
			);
		box[2].Extents = halfExtent;
		// RightBottom
		box[3].Center = Vector3(
			Bounds.Center.x + halfExtent.x,
			Bounds.Center.y,
			Bounds.Center.z - halfExtent.z
		);
		box[3].Extents = halfExtent;
	}

};

