#ifndef DOCUMENT_UNITS_H
#define DOCUMENT_UNITS_H

namespace document_units
{
	const double inch_factor = 2.54;

	struct inch;
	struct centimeter;
	struct pixel;
	struct dpi;

	template<typename T>
	struct units {
		//typedef decltype(T::value) value_type;

		T operator+(T rhs) const {
			return T(raw_value() + rhs.value);
		}

		T operator-(T rhs) const {
			return T(raw_value() - rhs.value);
		}

		T operator*(double rhs) const {
			return T(raw_value() * rhs);
		}

		T operator/(double rhs) const {
			return T(raw_value() / rhs);
		}

		inline auto raw_value() const {
			return static_cast<const T*>(this)->value;
		}
	};

	struct inch : public units<inch> {
		explicit inch(double pvalue) : value(pvalue) {}
		inch(centimeter pvalue);
		inch(pixel pvalue, dpi resolution);
		double value;
	};

	struct centimeter : public units<centimeter>{
		explicit centimeter(double pvalue) : value(pvalue) {}
		centimeter(inch pvalue);
		centimeter(pixel pvalue, dpi resolution);
		double value;
	};

	struct pixel : public units<pixel> {
		explicit pixel(int pvalue) : value(pvalue) {}
		pixel(inch pvalue, dpi resolution);
		pixel(centimeter pvalue, dpi resolution);
		int value;
	};

	struct dpi {
		explicit dpi(int pvalue) : value(pvalue) {}
		int value;
	};

	template<typename T>
	struct size {
		template<typename src_t>
		size(size<src_t> sz) : width(sz.width), height(sz.height) {}

		size(T pwidth, T pheight) : width(pwidth), height(pheight) {}
		inline size<T> scale(double factor) const {
			return size<T>(width*factor, height*factor);
		}
		T width, height;
	};

	template<typename T>
	struct coordinate {
		template<typename src_t>
		coordinate(coordinate<src_t> coord) : x(coord.x), y(coord.y) {}

		coordinate(T px, T py) : x(px), y(py) {}
		inline coordinate<T> scale(double factor) const {
			return coordinate<T>(x*factor, y*factor);
		}
		inline coordinate<T> move(T amount) const {
			return coordinate<T>(x+amount, y+amount);
		}
		T x,y;
	};

	template<typename T>
	struct rect {
		rect(coordinate<T> pcoordinate, size<T> psize) : _coordinate(pcoordinate), _size(psize) {}
		rect(coordinate<T> pcoordinate1, coordinate<T> pcoordinate2) : _coordinate(pcoordinate1), _size(pcoordinate2.x - pcoordinate1.x, pcoordinate2.y - pcoordinate1.y) {}

		inline coordinate<T> start_point() const {
			return _coordinate;
		}

		inline coordinate<T> end_point() const {
			return coordinate<T>(_coordinate.x + _size.width, _coordinate.y + _size.height);
		}

		inline rect<T> scale(double factor) const {
			return rect<T>(_coordinate.scale(factor), _size.scale(factor));
		}

		inline rect<T> move(T x, T y) const {
			const coordinate<T> ncoord(_coordinate.x + x, _coordinate.y + y);
			return rect<T>(ncoord, _size);
		}

		inline rect<T> grow(T amount) const{
			return rect<T>(start_point().move(amount * -1.0), end_point().move(amount * 2.0));
		}

		inline rect<T> bounded_grow(T amount, rect<T> bound) const {
			return grow(amount).fit_into(bound);
		}

		inline rect<T> fit_into(rect<T> rt) const {
			coordinate<T> start = start_point();
			start.x = T(std::max(rt.start_point().x, start_point().x));
			start.y = T(std::max(rt.start_point().y, start_point().y));

			coordinate<T> end = end_point();
			end.x = std::min(rt.end_point().x, end_point().x);
			end.y = std::min(rt.end_point().y, end_point().y);

			return rect<T>(start, end);
		}

		coordinate<T> _coordinate;
		size<T> _size;
	};

	inline inch::inch(centimeter pvalue) : value(pvalue.value / inch_factor) {}
	inline inch::inch(pixel pvalue, dpi resolution) : value(static_cast<double>(pvalue.value) / resolution.value) {}
	inline centimeter::centimeter(inch pvalue) : value(pvalue.value * inch_factor) {}
	inline centimeter::centimeter(pixel pvalue, dpi resolution) : value(static_cast<double>(pvalue.value * inch_factor) / resolution.value) {}
	inline pixel::pixel(inch pvalue, dpi resolution) : value(pvalue.value * resolution.value) {}
	inline pixel::pixel(centimeter pvalue, dpi resolution) : value(pvalue.value / inch_factor * resolution.value) {}

	struct resolution_setting {
		resolution_setting(dpi px, dpi py) : x(px), y(py) {}

		template<typename desired, typename T>
		inline size<desired> to(size<T> value) const {
			return size<desired>(desired(value.width, x), desired(value.height, y));
		}

		template<typename desired, typename T>
		inline coordinate<desired> to(coordinate<T> value) const {
			return coordinate<desired>(desired(value.x, x), desired(value.y, y));
		}

		template<typename desired, typename T>
		inline rect<desired> to(rect<T> value) const {
			return rect<desired>(to<desired>(value._coordinate), to<desired>(value._size));
		}

		template<typename desired, typename T>
		inline desired x_to(T value) const {
			return desired(value, x);
		}

		template<typename desired, typename T>
		inline desired y_to(T value) const {
			return desired(value, y);
		}

		dpi x, y;
	};

	struct margins {
		margins(centimeter ptop, centimeter pleft, centimeter pbottom, centimeter pright) : top(ptop), left(pleft), bottom(pbottom), right(pright) {}

		centimeter top, left, bottom, right;
	};

	template<typename T>
	bool operator<(const units<T>& lhs, const units<T>& rhs)
	{
		return lhs.raw_value() < rhs.raw_value();
	}

	template<typename T>
	bool operator==(const units<T>& lhs, const units<T>& rhs)
	{
		return lhs.raw_value() == rhs.raw_value();
	}
}



#endif
