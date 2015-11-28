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
		T operator+(T rhs) const {
			return T(value() + rhs.value);
		}

		T operator-(T rhs) const {
			return T(value() - rhs.value);
		}

		T operator*(double rhs) const {
			return T(value() * rhs);
		}

		T operator/(double rhs) const {
			return T(value() / rhs);
		}
	private:
		inline auto value() const {
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
		size(T pwidth, T pheight) : width(pwidth), height(pheight) {}
		T width, height;
	};

	template<typename T>
	struct coordinate {
		coordinate(T px, T py) : x(px), y(py) {}
		T x,y;
	};

	template<typename T>
	struct rect {
		rect(coordinate<T> pcoordinate, size<T> psize) : _coordinate(pcoordinate), _size(psize) {}

		inline coordinate<T> start_point() const {
			return _coordinate;
		}

		inline coordinate<T> end_point() const {
			return coordinate<T>(_coordinate.x + _size.width, _coordinate.y + _size.height);
		}


		coordinate<T> _coordinate;
		size<T> _size;
	};

	inline inch::inch(centimeter pvalue) : value(pvalue.value / inch_factor) {}
	inline inch::inch(pixel pvalue, dpi resolution) : value(static_cast<double>(pvalue.value) / resolution.value) {}
	inline centimeter::centimeter(inch pvalue) : value(pvalue.value * inch_factor) {}
	inline centimeter::centimeter(pixel pvalue, dpi resolution) : value(static_cast<double>(pvalue.value * inch_factor) / resolution.value) {}
	inline pixel::pixel(inch pvalue, dpi resolution) : value(pvalue.value * resolution.value) {}
	inline pixel::pixel(centimeter pvalue, dpi resolution) : value(pvalue.value * inch_factor * resolution.value) {}

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
}

#endif
