#pragma once
class cRay
{
private:
	D3DXVECTOR3 m_vOrg;
	D3DXVECTOR3 m_vDir;
public:
	cRay();
	~cRay();

public:
	void RayAtViewSpace(POINT vScreenPos, D3DXMATRIX proj);
	void RayAtWorldSpace(POINT vScreenPos, D3DXMATRIX view, D3DXMATRIX proj);

	bool RayPickingTrangle(D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 v3, OUT D3DXVECTOR3& vPick);

	bool RayPickingMesh( OUT D3DXVECTOR3& vPick);
};

