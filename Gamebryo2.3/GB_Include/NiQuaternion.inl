// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not
// be copied or disclosed except in accordance with the terms of that
// agreement.
//
//      Copyright (c) 1996-2007 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net
//---------------------------------------------------------------------------
//  NiQuaternion inline functions

//---------------------------------------------------------------------------
inline void NiQuaternion::SetW(float w)
{
    m_fW = w;
}
//---------------------------------------------------------------------------
inline void NiQuaternion::SetX(float x)
{
    m_fX = x;
}
//---------------------------------------------------------------------------
inline void NiQuaternion::SetY(float y)
{
    m_fY = y;
}
//---------------------------------------------------------------------------
inline void NiQuaternion::SetZ(float z)
{
    m_fZ = z;
}
//---------------------------------------------------------------------------
inline float NiQuaternion::GetW() const
{
    return m_fW;
}
//---------------------------------------------------------------------------
inline float NiQuaternion::GetX() const
{
    return m_fX;
}
//---------------------------------------------------------------------------
inline float NiQuaternion::GetY() const
{
    return m_fY;
}
//---------------------------------------------------------------------------
inline float NiQuaternion::GetZ() const
{
    return m_fZ;
}
//---------------------------------------------------------------------------
inline void NiQuaternion::SetValues(float w, float x, float y, float z)
{
    m_fW = w;
    m_fX = x;
    m_fY = y;
    m_fZ = z;
}
//---------------------------------------------------------------------------
inline void NiQuaternion::GetValues(float& w, float& x, float& y, float& z)
    const
{
    w = m_fW;
    x = m_fX;
    y = m_fY;
    z = m_fZ;
}
//---------------------------------------------------------------------------
inline bool NiQuaternion::operator== (const NiQuaternion& q) const
{
    return
    m_fW == q.m_fW && m_fX == q.m_fX && m_fY == q.m_fY && m_fZ == q.m_fZ;
}
//---------------------------------------------------------------------------
inline bool NiQuaternion::operator!= (const NiQuaternion& q) const
{
    return
    m_fW != q.m_fW || m_fX != q.m_fX || m_fY != q.m_fY || m_fZ != q.m_fZ;
}
//---------------------------------------------------------------------------
inline void NiQuaternion::ToRotation(NiMatrix3& rot) const
{
    // operations (*,+,-) = 24

    float tx  = 2.0f*m_fX;
    float ty  = 2.0f*m_fY;
    float tz  = 2.0f*m_fZ;
    float twx = tx*m_fW;
    float twy = ty*m_fW;
    float twz = tz*m_fW;
    float txx = tx*m_fX;
    float txy = ty*m_fX;
    float txz = tz*m_fX;
    float tyy = ty*m_fY;
    float tyz = tz*m_fY;
    float tzz = tz*m_fZ;

    rot.SetCol(0, 1.0f-(tyy+tzz), txy+twz, txz-twy);
    rot.SetCol(1, txy-twz, 1.0f-(txx+tzz), tyz+twx);
    rot.SetCol(2, txz+twy, tyz-twx, 1.0f-(txx+tyy));
}
//---------------------------------------------------------------------------
inline float NiQuaternion::Dot(const NiQuaternion& p, const NiQuaternion& q)
{
    return p.m_fW * q.m_fW + p.m_fX * q.m_fX + 
           p.m_fY * q.m_fY + p.m_fZ * q.m_fZ;
}
//---------------------------------------------------------------------------
inline void NiQuaternion::FromAngleAxisX(float fAngle)
{
    NiSinCos(fAngle * 0.5f, m_fX, m_fW);
    m_fY = m_fZ = 0.0f;
}
//---------------------------------------------------------------------------
inline void NiQuaternion::FromAngleAxisY(float fAngle)
{
    NiSinCos(fAngle * 0.5f, m_fY, m_fW);
    m_fX = m_fZ = 0.0f;
}
//---------------------------------------------------------------------------
inline void NiQuaternion::FromAngleAxisZ(float fAngle)
{
    NiSinCos(fAngle * 0.5f, m_fZ, m_fW);
    m_fX = m_fY = 0.0f;
}
//---------------------------------------------------------------------------
inline void NiQuaternion::LoadBinary(NiStream& stream, 
    NiQuaternion* pkValues, unsigned int uiNumValues)
{
    NiStreamLoadBinary(stream, (float*)pkValues, uiNumValues*4);
}
//---------------------------------------------------------------------------
inline void NiQuaternion::SaveBinary(NiStream& stream, 
    NiQuaternion* pkValues, unsigned int uiNumValues)
{
    NiStreamSaveBinary(stream, (float*)pkValues, uiNumValues*4);
}
//---------------------------------------------------------------------------

// Compute 1/sqrt(s) using a tangent line approximation.
// These constants are outside of the function because
// not all compilers are smart enough to precompute the values.
static const float ISQRT_NEIGHBORHOOD = 0.959066f;
static const float ISQRT_SCALE = 1.000311f;
static const float ISQRT_ADDITIVE_CONSTANT = ISQRT_SCALE /
(float)sqrt(ISQRT_NEIGHBORHOOD);
static const float ISQRT_FACTOR = ISQRT_SCALE * (-0.5f /
(ISQRT_NEIGHBORHOOD * (float)sqrt(ISQRT_NEIGHBORHOOD)));
inline float NiQuaternion::ISqrt_approx_in_neighborhood(float s)
{
	return ISQRT_ADDITIVE_CONSTANT + (s - ISQRT_NEIGHBORHOOD) * ISQRT_FACTOR;
}
//---------------------------------------------------------------------------
// Normalize a quaternion using the above approximation.
inline void NiQuaternion::FastNormalize()
{
	float s = m_fX*m_fX + m_fY*m_fY + m_fZ*m_fZ + m_fW*m_fW; // length^2
	float k = ISqrt_approx_in_neighborhood(s);

	if (s <= 0.91521198f) {
		k *= ISqrt_approx_in_neighborhood(k * k * s);

		if (s <= 0.65211970f) {
			k *= ISqrt_approx_in_neighborhood(k * k * s);
		}
	}

	m_fX *= k;
	m_fY *= k;
	m_fZ *= k;
	m_fW *= k;
}
//---------------------------------------------------------------------------
inline float NiQuaternion::Lerp(float v0, float v1, float fPerc)
{
	return v0 + fPerc * (v1 - v0);
}
//---------------------------------------------------------------------------
// CounterWarp: A helper function used by Slerp.
inline float NiQuaternion::CounterWarp(float t, float fCos)
{
	const float ATTENUATION = 0.82279687f;
	const float WORST_CASE_SLOPE = 0.58549219f;

	float fFactor = 1.0f - ATTENUATION * fCos;
	fFactor *= fFactor;
	float fK = WORST_CASE_SLOPE * fFactor;

	return t*(fK*t*(2.0f*t - 3.0f) + 1.0f + fK);
}
//---------------------------------------------------------------------------