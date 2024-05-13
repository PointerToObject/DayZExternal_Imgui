#include "Vector.h"

#include <cmath>



CVector CVector::vEmpty;

CVector::CVector(float fX, float fY, float fZ) :
	v{fX, fY, fZ} { }

float CVector::Magnitude(CVector vRValue, bool bSqrt, const char cDisableComponent)
{
	float fTemp = 0.0F;

	switch (cDisableComponent)
	{
		case 'x':
			fTemp = ((y - vRValue.y) * (y - vRValue.y) + (z - vRValue.z) * (z - vRValue.z));
			break;
		case 'y':
			fTemp = ((vRValue.x - x) * (vRValue.x - x) + (vRValue.z - z) * (vRValue.z - z));
			break;
		case 'z':
			fTemp = ((x - vRValue.x) * (x - vRValue.x) + (y - vRValue.y) * (y - vRValue.y));
			break;
		default:
			fTemp = ((vRValue.x - x) * (vRValue.x - x) + (vRValue.y - y) * (vRValue.y - y) + (vRValue.z - z) * (vRValue.z - z));
	}

	return bSqrt ? sqrt(fTemp) : fTemp;
}

float CVector::Dot(CVector vRValue)
{
	return x * vRValue.x + y * vRValue.y + z * vRValue.z;
}

CVector CVector::Normalized(const char cDisableComponent)
{
	return Normalized(Magnitude(), cDisableComponent);
}
CVector CVector::Normalized(float fMagnitude, const char cDisableComponent)
{
	CVector vTemp = *this;

	switch (cDisableComponent)
	{
		case 'x':
			vTemp.x = 0.0F;
			vTemp = (vTemp / fMagnitude);
			break;
		case 'y':
			vTemp.y = 0.0F;
			vTemp = (vTemp / fMagnitude);
			break;
		case 'z':
			vTemp.z = 0.0F;
			vTemp = (vTemp / fMagnitude);
			break;
		default:
			vTemp = (vTemp / fMagnitude);
	}

	return vTemp;
}

CVector CVector::Multiply(float fMultiplicand, const char cMultiplier)
{
	CVector vTemp = *this;

	switch (cMultiplier)
	{
		case 'x':
			vTemp.x *= fMultiplicand;
			break;
		case 'y':
			vTemp.y *= fMultiplicand;
			break;
		case 'z':
			vTemp.z *= fMultiplicand;
			break;
		default:
			vTemp *= fMultiplicand;
	}

	return vTemp;
}

CVector CVector::Floor()
{
	x = std::floor(x);
	y = std::floor(y);

	return *this;
}

inline float clamp(float fValue, float fMin, float fMax)
{
	fValue = (fValue < fMin) ? fMin : fValue;
	fValue = (fValue > fMax) ? fMax : fValue;

	return fValue;
}

void CVector::Clamp(float fMin, float fMax, const char cComponent)
{
	switch (cComponent)
	{
		case 'x':
			x = clamp(x, fMin, fMax);
			break;
		case 'y':
			y = clamp(y, fMin, fMax);
			break;
		case 'z':
			z = clamp(z, fMin, fMax);
			break;
		default:
			x = clamp(x, fMin, fMax);
			y = clamp(y, fMin, fMax);
			z = clamp(z, fMin, fMax);
	}
}
void CVector::Clamp(CVector vMin, CVector vMax, const char cDisableComponent)
{
	switch (cDisableComponent)
	{
		case 'x':
			y = clamp(y, vMin.y, vMax.y);
			z = clamp(z, vMin.z, vMax.z);
			break;
		case 'y':
			x = clamp(x, vMin.x, vMax.x);
			z = clamp(z, vMin.z, vMax.z);
			break;
		case 'z':
			x = clamp(x, vMin.x, vMax.x);
			y = clamp(y, vMin.y, vMax.y);
			break;
		default:
			x = clamp(x, vMin.x, vMax.x);
			y = clamp(y, vMin.y, vMax.y);
			z = clamp(z, vMin.z, vMax.z);
	}
}

bool CVector::IsEmpty()
{
	return x == 0.0F && y == 0.0F && z == 0.0F;
}

bool CVector::InRegion(CVector vRValue, CVector vSize) const
{
	return x <= vRValue.x && (x + vSize.x) >= vRValue.x &&
		y <= vRValue.y && (y + vSize.y) >= vRValue.y;
}

float& CVector::operator[](unsigned int uIndex)
{
	return v[uIndex];
}

bool CVector::operator==(CVector vRValue)
{
	return (x == vRValue.x && y == vRValue.y && z == vRValue.z);
}
bool CVector::operator<(CVector vRValue)
{
	return x < vRValue.x && y < vRValue.y && z < vRValue.z;
}
bool CVector::operator>(CVector vRValue)
{
	return x > vRValue.x && y > vRValue.y && z > vRValue.z;
}
bool CVector::operator<=(CVector vRValue)
{
	return x <= vRValue.x && y <= vRValue.y && z <= vRValue.z;
}
bool CVector::operator>=(CVector vRValue)
{
	return x >= vRValue.x && y >= vRValue.y && z >= vRValue.z;
}

CVector CVector::operator%(CVector vRValue)
{
	CVector vTemp = (*this - vRValue);
	float fMagnitude = Magnitude(vRValue, true, '0');

	return vTemp / fMagnitude;
}
float CVector::operator||(CVector vRValue)
{
	return sqrt(pow(x - vRValue.x, 2.0F) + pow(y - vRValue.y, 2.0F) + pow(z - vRValue.z, 2.0F));
}

CVector CVector::operator+(CVector vRValue)
{
	return CVector(x + vRValue.x, y + vRValue.y, z + vRValue.z);
}
CVector CVector::operator+(float fRValue)
{
	return CVector(x + fRValue, y + fRValue, z + fRValue);
}
CVector CVector::operator-(CVector vRValue)
{
	return CVector(x - vRValue.x, y - vRValue.y, z - vRValue.z);
}
CVector CVector::operator-(float fRValue)
{
	return CVector(x - fRValue, y - fRValue, z - fRValue);
}
CVector CVector::operator*(CVector vRValue)
{
	return CVector(x * vRValue.x, y * vRValue.y, z * vRValue.z);
}
CVector CVector::operator*(float fRValue)
{
	return CVector(x * fRValue, y * fRValue, z * fRValue);
}
CVector CVector::operator/(CVector vRValue)
{
	return CVector(x / vRValue.x, y / vRValue.y, z / vRValue.z);
}
CVector CVector::operator/(float fRValue)
{
	return CVector(x / fRValue, y / fRValue, z / fRValue);
}
CVector& CVector::operator+=(const CVector &vRValue)
{
	x += vRValue.x;
	y += vRValue.y;
	z += vRValue.z;

	return *this;
}
CVector& CVector::operator+=(const float &fRValue)
{
	x += fRValue;
	y += fRValue;
	z += fRValue;

	return *this;
}
CVector& CVector::operator-=(const CVector &vRValue)
{
	x -= vRValue.x;
	y -= vRValue.y;
	z -= vRValue.z;

	return *this;
}
CVector& CVector::operator-=(const float &fRValue)
{
	x -= fRValue;
	y -= fRValue;
	z -= fRValue;

	return *this;
}
CVector& CVector::operator*=(const CVector &vRValue)
{
	x *= vRValue.x;
	y *= vRValue.y;
	z *= vRValue.z;

	return *this;
}
CVector& CVector::operator*=(const float &fRValue)
{
	x *= fRValue;
	y *= fRValue;
	z *= fRValue;

	return *this;
}
CVector& CVector::operator/=(const CVector &vRValue)
{
	x /= vRValue.x;
	y /= vRValue.y;
	z /= vRValue.z;

	return *this;
}
CVector& CVector::operator/=(const float &fRValue)
{
	x /= fRValue;
	y /= fRValue;
	z /= fRValue;

	return *this;
}