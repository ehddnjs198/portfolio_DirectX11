#include "stdafx.h"
#include "cRay.h"

#include "SkinnedMesh.h"

#include "Graphic/Graphic.h"

cRay::cRay()
	: m_vOrg(0, 0, 0)
	, m_vDir(0, 0, 0)
{
}


cRay::~cRay()
{
}

void cRay::RayAtViewSpace(POINT vScreenPos, D3DXMATRIX proj)
{
	//D3DXMATRIX matProj;
	//g_pD3DDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	//
	//D3DVIEWPORT9 vp;
	//g_pD3DDevice->GetViewport(&vp);
	float width = Setting::GetInstance().GetWindowWidth();
	float hight = Setting::GetInstance().GetWindowHeight();

	UINT numViewport = 0;
	D3D11_VIEWPORT * view = new D3D11_VIEWPORT[0];
	Graphic::GetInstance().GetDeviceContext()->RSGetViewports(&numViewport, view);

	m_vOrg = D3DXVECTOR3(0, 0, 0);
	m_vDir.x = (((2.0f * vScreenPos.x) / width) - 1.0f) / proj._11;
	m_vDir.y = (((-2.0f * vScreenPos.y) / hight) + 1.0f) / proj._22;
	m_vDir.z = 1.0f;

	SAFE_DELETE(view);
}

void cRay::RayAtWorldSpace(POINT vScreenPos, D3DXMATRIX view, D3DXMATRIX proj)
{
	RayAtViewSpace(vScreenPos, proj);

	D3DXMATRIX matView, matInvView;
	matView = view;

	D3DXMatrixInverse(&matInvView, 0, &matView);

	D3DXVec3TransformCoord(&m_vOrg, &m_vOrg, &matInvView);
	D3DXVec3TransformNormal(&m_vDir, &m_vDir, &matInvView);
	D3DXVec3Normalize(&m_vDir, &m_vDir);
}

bool cRay::RayPickingTrangle(D3DXVECTOR3 v1, D3DXVECTOR3 v2, D3DXVECTOR3 v3, OUT D3DXVECTOR3& vPick)
{
	float u, v, fDist;
	vPick = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	if (D3DXIntersectTri(&v1, &v2, &v3, &m_vOrg, &m_vDir, &u, &v, &fDist))
	{
		vPick = m_vOrg + fDist * m_vDir;
		return true;
	}
	return false;

}

bool cRay::RayPickingMesh( OUT D3DXVECTOR3& vPick)
{
	float u, v, fDist;
	vPick = D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX);
	for (size_t i = 0; i < SkinnedMesh::GetInstance().Indices.size(); i+= 3)
	{
		UINT v0 = SkinnedMesh::GetInstance().Indices[i];
		UINT v1 = SkinnedMesh::GetInstance().Indices[i+1];
		UINT v2 = SkinnedMesh::GetInstance().Indices[i+2];
		
		
		if (D3DXIntersectTri(
			&SkinnedMesh::GetInstance().Positions[v0],
			&SkinnedMesh::GetInstance().Positions[v1],
			&SkinnedMesh::GetInstance().Positions[v2],
			&m_vOrg, &m_vDir, &u, &v, &fDist))
		{
			vPick = m_vOrg + fDist * m_vDir;
			return true;
		}
	}
	return false;
}
