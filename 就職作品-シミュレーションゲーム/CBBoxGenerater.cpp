#include <stdio.h>
#include <DirectXMath.h>
#include "obb.h"
#include "dx11mathutil.h"
#include "CBBoxGenerater.h"

using namespace DirectX;

const double CBBoxGenerater::TOL = 1.0e-10;
const double PI=3.14159265358979323846;

CBBoxGenerater::CBBoxGenerater(void)
{
}

CBBoxGenerater::~CBBoxGenerater(void)
{
}

/*----------------------------
	共分散行列を作る(頂点群から)
------------------------------*/
XMFLOAT4X4 CBBoxGenerater::GetCovarianceMatrix(std::vector<XMFLOAT3>& vertices) {
	float mx = 0, my = 0, mz = 0;
	XMFLOAT4X4 retmat = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	//ボーンを構成する頂点について平均値を求める
	for (int j = 0; j < vertices.size(); j++) {
		mx += vertices[j].x;		// Ｘ値合計
		my += vertices[j].y;		// Ｙ値合計
		mz += vertices[j].z;		// Ｚ値合計
	}
	mx /= vertices.size();			// Ｘ値平均
	my /= vertices.size();			// Ｙ値平均
	mz /= vertices.size();			// Ｚ値平均

	//共分散行列の各成分を求める
	for (int j = 0; j < vertices.size(); j++) {
		retmat._11 += (vertices[j].x - mx) * (vertices[j].x - mx);
		retmat._22 += (vertices[j].y - my) * (vertices[j].y - my);
		retmat._33 += (vertices[j].z - mz) * (vertices[j].z - mz);
		retmat._12 += (vertices[j].x - mx) * (vertices[j].y - my);
		retmat._13 += (vertices[j].x - mx) * (vertices[j].z - mz);
		retmat._23 += (vertices[j].y - my) * (vertices[j].z - mz);
	}
	retmat._11 /= vertices.size();
	retmat._22 /= vertices.size();
	retmat._33 /= vertices.size();
	retmat._12 /= vertices.size();
	retmat._13 /= vertices.size();
	retmat._23 /= vertices.size();
	retmat._21 = retmat._12;
	retmat._31 = retmat._13;
	retmat._32 = retmat._23;

	return retmat;
}

/*----------------------------
	共分散行列を作る(頂点バッファとインデックスから)
------------------------------*/
XMFLOAT4X4 CBBoxGenerater::GetCovarianceMatrix(
	XMFLOAT3* vertex,	// 頂点データ配列 
	unsigned int* index,		// ボーンに影響を受ける頂点インデックスの配列
	int numVertices)	// ボーンに影響を受ける頂点数
{
	float mx=0, my=0, mz=0;
	XMFLOAT4X4 retmat = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	//ボーンを構成する頂点について平均値を求める
	for(int j=0; j<numVertices; j++){
		mx += vertex[index[j]].x;		// Ｘ値合計
		my += vertex[index[j]].y;		// Ｙ値合計
		mz += vertex[index[j]].z;		// Ｚ値合計
	}
	mx /= numVertices;		// Ｘ値平均
	my /= numVertices;		// Ｙ値平均
	mz /= numVertices;		// Ｚ値平均

	//共分散行列の各成分を求める
	for(int j=0; j<numVertices; j++){
		retmat._11 += (vertex[index[j]].x - mx) * (vertex[index[j]].x - mx);
		retmat._22 += (vertex[index[j]].y - my) * (vertex[index[j]].y - my);
		retmat._33 += (vertex[index[j]].z - mz) * (vertex[index[j]].z - mz);
		retmat._12 += (vertex[index[j]].x - mx) * (vertex[index[j]].y - my);
		retmat._13 += (vertex[index[j]].x - mx) * (vertex[index[j]].z - mz);
		retmat._23 += (vertex[index[j]].y - my) * (vertex[index[j]].z - mz);
	}
	retmat._11 /= numVertices;
	retmat._22 /= numVertices;
	retmat._33 /= numVertices;
	retmat._12 /= numVertices;
	retmat._13 /= numVertices;
	retmat._23 /= numVertices;
	retmat._21 = retmat._12;
	retmat._31 = retmat._13;
	retmat._32 = retmat._23;

	return retmat;
}

/*----------------------------
	固有ベクトルを求める
------------------------------*/
bool CBBoxGenerater::GetEigenVector(XMFLOAT4X4& a, XMFLOAT4X4& x){
	int i,j,k,m,count;
	double amax, amax0, theta, co, si, co2, si2, cosi, pi = 4.0 * atan(1.0);
	double aii, aij, ajj, aik, ajk;
	bool result = false;

	// 単位行列にする
	DX11MtxIdentity(x);

	count = 0;
	while(count <= MAX){
		//非対角要素の最大値を探索
		amax = 0.0;
		for(k=0; k<N-1; k++){
			for(m=k+1; m<N; m++){
//				amax0 = fabs(*(a.m[N*k+m]));			// おかしい
				float temp=fabs(a.m[k][m]);				// ダブル型としてみていたので修正
				amax0 = temp;
				if(amax0 > amax){
					i = k;
					j = m;
					amax = amax0;
				}
			}
		}

		//収束判定
		if(amax <= TOL){
			result = true;
			break;
		}else{
			aii = a.m[i][i]; 
			aij = a.m[i][j]; 
			ajj = a.m[j][j];
			//回転角度計算
			if(fabs(aii-ajj) < TOL){
				theta  = 0.25 * pi * aij / fabs(aij);
			}
			else{
				theta = 0.5 * atan(2.0 * aij / (aii - ajj));
			}
			co = cos(theta); 
			si = sin(theta); 
			co2 = co * co; 
			si2 = si * si; 
			cosi = co * si;

			//相似変換行列
			a.m[i][i] = co2 * aii + 2.0 * cosi * aij + si2 * ajj;
			a.m[j][j] = si2 * aii - 2.0 * cosi * aij + co2 * ajj;
			a.m[i][j] = 0.0; 
			a.m[j][i] = 0.0;

			for(k=0; k<N; k++){
				if(k != i && k != j){
					aik = a.m[k][i];
					ajk = a.m[k][j];
					a.m[k][i] = co * aik + si * ajk;
					a.m[i][k] = a.m[k][i];
					a.m[k][j] = -si * aik + co * ajk;
					a.m[j][k] = a.m[k][j];
				}
			}

			//固有ベクトル
			for(k=0; k<N; k++){
				aik = x.m[k][i];
				ajk = x.m[k][j];
				x.m[k][i] = co * aik + si* ajk;
				x.m[k][j] = -si * aik + co * ajk;
			}

			count++;
		}
	}
	return result;
}

OBB CBBoxGenerater::CaclBBox(std::vector<XMFLOAT3>& vertices, XMFLOAT4X4 eigenmat)
{
	XMFLOAT3 rvec, svec, tvec, vertexvec, center;
	float rmin = 0.0, rmax = 0.0, smin = 0.0, smax = 0.0, tmin = 0.0, tmax = 0.0;
	float dot;
	OBB result;

	//固有ベクトルが格納された行列を各ベクトルへ代入し正規化
	rvec.x = eigenmat._11; rvec.y = eigenmat._21; rvec.z = eigenmat._31;
	svec.x = eigenmat._12; svec.y = eigenmat._22; svec.z = eigenmat._32;
	tvec.x = eigenmat._13; tvec.y = eigenmat._23; tvec.z = eigenmat._33;

	// ノーマライズ
	DX11Vec3Normalize(rvec, rvec);
	DX11Vec3Normalize(svec, svec);
	DX11Vec3Normalize(tvec, tvec);

	//各ベクトルとすべての頂点の内積から最小値と最大値を求める
	for (int i = 0; i < vertices.size(); i++) {
		vertexvec.x = vertices[i].x; vertexvec.y = vertices[i].y; vertexvec.z = vertices[i].z;

		DX11Vec3Dot(dot, rvec, vertexvec);

		if (i == 0 || dot < rmin) rmin = dot;
		if (i == 0 || dot > rmax) rmax = dot;

		DX11Vec3Dot(dot, svec, vertexvec);
		if (i == 0 || dot < smin) smin = dot;
		if (i == 0 || dot > smax) smax = dot;

		DX11Vec3Dot(dot, tvec, vertexvec);
		if (i == 0 || dot < tmin) tmin = dot;
		if (i == 0 || dot > tmax) tmax = dot;
	}

	center.x = (rmax + rmin) / 2;	center.y = (smax + smin) / 2;	center.z = (tmax + tmin) / 2;

	//OBB構造体に値を格納
	result.center.x = rvec.x * center.x + svec.x * center.y + tvec.x * center.z;
	result.center.y = rvec.y * center.x + svec.y * center.y + tvec.y * center.z;
	result.center.z = rvec.z * center.x + svec.z * center.y + tvec.z * center.z;
	result.fLengthX = (rmax - rmin);// / 2;
	result.fLengthY = (smax - smin);// / 2;
	result.fLengthZ = (tmax - tmin);// / 2;
	result.vecAxisX = rvec; 
	result.vecAxisY = svec; 
	result.vecAxisZ = tvec;

	return result;
}

OBB CBBoxGenerater::CaclBBox(XMFLOAT3 *vertex, unsigned int *index, int numVertices, XMFLOAT4X4 eigenmat)
{
	XMFLOAT3 rvec,svec,tvec,vertexvec,center;
	float rmin=0.0,rmax=0.0,smin=0.0,smax=0.0,tmin=0.0,tmax=0.0;
	float dot;
	OBB result;

	//固有ベクトルが格納された行列を各ベクトルへ代入し正規化
	rvec.x = eigenmat._11; rvec.y = eigenmat._21; rvec.z = eigenmat._31;
	svec.x = eigenmat._12; svec.y = eigenmat._22; svec.z = eigenmat._32;
	tvec.x = eigenmat._13; tvec.y = eigenmat._23; tvec.z = eigenmat._33;


	DX11Vec3Normalize(rvec, rvec);
	DX11Vec3Normalize(svec, svec);
	DX11Vec3Normalize(tvec, tvec);
//	D3DXVec3Normalize(&rvec,&rvec);
//	D3DXVec3Normalize(&svec,&svec);
//	D3DXVec3Normalize(&tvec,&tvec);

	//各ベクトルとすべての頂点の内積から最小値と最大値を求める
	for(int i=0; i<numVertices; i++){
		vertexvec.x = vertex[index[i]].x; vertexvec.y = vertex[index[i]].y; vertexvec.z = vertex[index[i]].z;

//		dot = D3DXVec3Dot(&rvec,&vertexvec);
		DX11Vec3Dot(dot, rvec, vertexvec);

		if(i==0 || dot < rmin) rmin = dot;
		if(i==0 || dot > rmax) rmax = dot;

//		dot= D3DXVec3Dot(&svec,&vertexvec);
		DX11Vec3Dot(dot, svec, vertexvec);
		if(i==0 || dot < smin) smin = dot;
		if(i==0 || dot > smax) smax = dot;

//		dot= D3DXVec3Dot(&tvec,&vertexvec);
		DX11Vec3Dot(dot, tvec, vertexvec);
		if(i==0 || dot < tmin) tmin = dot;
		if(i==0 || dot > tmax) tmax = dot;
	}
	
	center.x = (rmax + rmin) / 2;	center.y = (smax + smin) / 2;	center.z = (tmax + tmin) / 2;

	//BBOX構造体に値を格納
	result.center.x = rvec.x * center.x + svec.x * center.y + tvec.x * center.z;
	result.center.y = rvec.y * center.x + svec.y * center.y + tvec.y * center.z;
	result.center.z = rvec.z * center.x + svec.z * center.y + tvec.z * center.z;
	result.fLengthX = (rmax - rmin) / 2; result.fLengthY = (smax - smin) / 2; result.fLengthZ = (tmax - tmin) / 2;
//	result.vMax.x = rmax; result.vMax.y = smax; result.vMax.z = tmax;
//	result.vMin.x = rmin; result.vMin.y = smin; result.vMin.z = tmin;
	result.vecAxisX = rvec; result.vecAxisY = svec; result.vecAxisZ = tvec;
	
	return result;
}
