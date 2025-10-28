template <class T>
class Vector2Type
{
	public:
	typedef T value_type;
	T x;
	T y;
	Vector2Type(){}
	Vector2Type(const T& _x,const T& _y)
	{
		x=_x;
		y=_y;
	}
	template<class I=int32_t>
	Vector2Type(const Vector2Type<I>& v)
	{
		x=v.x;
		y=v.y;
	}
	Vector2Type operator +(const Vector2Type& b) const
	{
		return Vector2Type(x+b.x,y+b.y);
	}
	Vector2Type operator +=(const Vector2Type& b)
	{
		return *this=*this+b;
	}
	Vector2Type operator -(const Vector2Type& b) const
	{
		return Vector2Type(x-b.x,y-b.y);
	}
	Vector2Type operator -=(const Vector2Type& b)
	{
		return *this=*this-b;
	}
	Vector2Type operator -() const
	{
		return Vector2Type(-x,-y);
	}
	Vector2Type operator *(const T& b) const
	{
		return Vector2Type(x*b,y*b);
	}
	Vector2Type operator *=(const T& b)
	{
		return *this=*this*b;
	}
	Vector2Type operator /(const T& b) const
	{
		return Vector2Type(x/b,y/b);
	}
	Vector2Type operator /=(const T& b)
	{
		return *this=*this/b;
	}
	Vector2Type operator *(const Vector2Type& b) const
	{
		return Vector2Type(x*b.x,y*b.y);
	}
	Vector2Type operator *=(const Vector2Type& b)
	{
		return *this=*this*b;
	}
	Vector2Type operator /(const Vector2Type& b) const
	{
		return Vector2Type(x/b.x,y/b.y);
	}
	Vector2Type operator /=(const Vector2Type& b)
	{
		return *this=*this/b;
	}
	bool operator ==(const Vector2Type& b) const
	{
		return x==b.x && y==b.y;
	}
	bool operator !=(const Vector2Type& b) const
	{
		return !(*this==b);
	}
	bool within(const Vector2Type& min,const Vector2Type& max) const
	{
		return x>=min.x && x<=max.x && y>=min.y && y<=max.y;
	}
	template <class returnType=T>
	returnType d2() const
	{
		return x*x+y*y;
	}
	template <class returnType=T>
	returnType d() const
	{
		return sqrt(d2<returnType>());
	}
	Vector2Type n() const
	{
		T distance=d();
		if(distance==0) return Vector2Type(1,0);
		else return *this*(1/distance);
	}
	template<class I=int32_t>
	Vector2Type<I> toPos() const
	{
		return Vector2Type<I>(x,y);
	}
	template<class I=int32_t>
	Vector2Type<I> floor() const
	{
		return Vector2Type<I>(std::floor(x),std::floor(y));
	}
	template<class I=int32_t>
	Vector2Type<I> ceil() const
	{
		return Vector2Type<I>(std::ceil(x),std::ceil(y));
	}
};

using Vector2=Vector2Type<double>;
using Vector2f=Vector2Type<float>;
using Pos=Vector2Type<int32_t>;

template <class T>
class Vector3Type
{
	public:
	typedef T value_type;
	T x;
	T y;
	T z;
	Vector3Type(){}
	Vector3Type(const T& _x,const T& _y,const T& _z)
	{
		x=_x;
		y=_y;
		z=_z;
	}
	template<class I=int32_t>
	Vector3Type(const Vector3Type<I>& v)
	{
		x=v.x;
		y=v.y;
		z=v.z;
	}
	Vector3Type operator +(const Vector3Type& b) const
	{
		return Vector3Type(x+b.x,y+b.y,z+b.z);
	}
	Vector3Type operator +=(const Vector3Type& b)
	{
		return *this=*this+b;
	}
	Vector3Type operator -(const Vector3Type& b) const
	{
		return Vector3Type(x-b.x,y-b.y,z-b.z);
	}
	Vector3Type operator -=(const Vector3Type& b)
	{
		return *this=*this-b;
	}
	Vector3Type operator -() const
	{
		return Vector3Type(-x,-y,-z);
	}
	Vector3Type operator *(const T& b) const
	{
		return Vector3Type(x*b,y*b,z*b);
	}
	Vector3Type operator *=(const T& b)
	{
		return *this=*this*b;
	}
	Vector3Type operator /(const T& b) const
	{
		return Vector3Type(x/b,y/b,z/b);
	}
	Vector3Type operator /=(const T& b)
	{
		return *this=*this/b;
	}
	Vector3Type operator *(const Vector3Type& b) const
	{
		return Vector3Type(x*b.x,y*b.y,z*b.z);
	}
	Vector3Type operator *=(const Vector3Type& b)
	{
		return *this=*this*b;
	}
	Vector3Type operator /(const Vector3Type& b) const
	{
		return Vector3Type(x/b.x,y/b.y,z/b.z);
	}
	Vector3Type operator /=(const Vector3Type& b)
	{
		return *this=*this/b;
	}
	bool operator ==(const Vector3Type& b) const
	{
		return x==b.x && y==b.y && z==b.z;
	}
	bool operator !=(const Vector3Type& b) const
	{
		return !(*this==b);
	}
	bool within(const Vector3Type& min,const Vector3Type& max) const
	{
		return x>=min.x && x<=max.x && y>=min.y && y<=max.y && z>=min.z && z<=max.z;
	}
	template <class returnType=T>
	returnType d2() const
	{
		return x*x+y*y+z*z;
	}
	template <class returnType=T>
	returnType d() const
	{
		return sqrt(d2<returnType>());
	}
	Vector3Type n() const
	{
		T distance=d();
		if(distance==0) return Vector3Type(1,0,0);
		else return *this*(1/distance);
	}
	template<class I=int32_t>
	Vector3Type<I> toPos3() const
	{
		return Vector3Type<I>(x,y,z);
	}
	template<class I=int32_t>
	Vector3Type<I> floor() const
	{
		return Vector3Type<I>(std::floor(x),std::floor(y),std::floor(z));
	}
	template<class I=int32_t>
	Vector3Type<I> ceil() const
	{
		return Vector3Type<I>(std::ceil(x),std::ceil(y),std::ceil(z));
	}
};

using Vector3=Vector3Type<double>;
using Vector3f=Vector3Type<float>;
using Pos3=Vector3Type<int32_t>;
