#pragma once

class Utils
{
public:
	static bool StartsWith(string str, string comp);
	static bool StartsWith(wstring str, wstring comp);

	static void Replace(OUT string& str, string comp, string rep);
	static void Replace(OUT wstring& str, wstring comp, wstring rep);

	static wstring ToWString(string value);
	static string ToString(wstring value);
	static string ToUtf8(wstring value);

	static Vector3 ToVector3(const PxVec3& vec);
	static PxVec3 ToPxVec3(const Vector3& vec);

};

