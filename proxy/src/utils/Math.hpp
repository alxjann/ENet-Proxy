#pragma once

template <typename T>
class CL_Vec2 {
public:
	inline bool operator==(const CL_Vec2<T>& other) const noexcept {
		return X == other.X && Y == other.Y;
	}
	inline bool operator!=(const CL_Vec2<T>& other) const noexcept {
		return X != other.X || Y != other.Y;
	}

	inline CL_Vec2<T> operator+(const CL_Vec2<T>& other) const noexcept {
		return { X + other.X, Y + other.Y };
	}

	inline CL_Vec2<T> operator-(const CL_Vec2<T>& other) const noexcept {
		return { X - other.X, Y - other.Y };
	}

	inline CL_Vec2<T> operator/(const CL_Vec2<T>& other) const noexcept {
		return { X / other.X, Y / other.Y };
	}

	inline CL_Vec2<T> operator*(const CL_Vec2<T>& other) const noexcept {
		return { X * other.X, Y * other.Y };
	}

	inline void operator+=(const CL_Vec2<T>& other) noexcept {
		X += other.X;
		Y += other.Y;
	}

	inline void operator-=(const CL_Vec2<T>& other) noexcept {
		X -= other.X;
		Y -= other.Y;
	}

	inline void operator/=(const CL_Vec2<T>& other) noexcept {
		X /= other.X;
		Y /= other.Y;
	}

	inline void operator*=(const CL_Vec2<T>& other) noexcept {
		X *= other.X;
		Y *= other.Y;
	}

	T X, Y;
};

template <typename T>
class CL_Vec3 {
public:
	T X, Y, Z;
};

template <typename T>
class CL_Rect {
public:
	T X, Y, Z, W;
};

using CL_Vec2F = CL_Vec2<float>;
using CL_Vec3F = CL_Vec3<float>;
using CL_RectF = CL_Rect<float>;

using CL_Vec2I = CL_Vec2<int>;
using CL_Vec3I = CL_Vec3<int>;
using CL_RectI = CL_Rect<int>;