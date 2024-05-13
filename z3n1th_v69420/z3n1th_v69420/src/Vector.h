#pragma once



class CVector
{
public:
	static CVector vEmpty;
	union
	{
		struct
		{
			float x, y, z;
		};
		float v[3];
	};

	CVector(float fX = 0.0F, float fY = 0.0F, float fZ = 0.0F);

	float Magnitude(CVector vRValue = CVector::vEmpty, bool bSqrt = true, const char cDisableComponent = '0');
	float Dot(CVector vRValue);
	CVector Normalized(const char cDisableComponent = '0');
	CVector Normalized(float fMagnitude, const char cDisableComponent = '0');
	CVector Multiply(float fMultiplicand, const char cMultiplier = '0');
	CVector Floor();
	void Clamp(float fMin, float fMax, const char cComponent = '0');
	void Clamp(CVector vMin, CVector vMax, const char cDisableComponent = '0');
	bool IsEmpty();
	bool InRegion(CVector vRValue, CVector vSize) const;

	float& operator[](unsigned int uIndex);

	bool operator==(CVector vRValue);
	bool operator<(CVector vRValue);
	bool operator>(CVector vRValue);
	bool operator<=(CVector vRValue);
	bool operator>=(CVector vRValue);

	CVector operator%(CVector vRValue);
	float operator||(CVector vRValue);

	CVector operator+(CVector vRValue);
	CVector operator+(float fRValue);
	CVector operator-(CVector vRValue);
	CVector operator-(float fRValue);
	CVector operator*(CVector vRValue);
	CVector operator*(float fRValue);
	CVector operator/(CVector vRValue);
	CVector operator/(float fRValue);
	CVector& operator+=(const CVector &vRValue);
	CVector& operator+=(const float &fRValue);
	CVector& operator-=(const CVector &vRValue);
	CVector& operator-=(const float &fRValue);
	CVector& operator*=(const CVector &vRValue);
	CVector& operator*=(const float &fRValue);
	CVector& operator/=(const CVector &vRValue);
	CVector& operator/=(const float &fRValue);
};