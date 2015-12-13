#ifndef AUTOMARKER_H
#define AUTOMARKER_H

#include <vector>

#include "document_units.h"

struct rect
{
	int top;
	int bottom;
	int left;
	int right;
};

namespace boost {
	template<typename T, std::size_t, typename Allocator>
	class multi_array;
}

typedef boost::multi_array<unsigned char, 2, std::allocator<unsigned char>> image_array;
class QImage;

struct automark_settings
{
	automark_settings(document_units::resolution_setting presolution, document_units::margins<document_units::centimeter> pmargins, document_units::centimeter pheight_threshold, unsigned int pgrey_threshold = 250, bool pbounding_box = false,
	bool pfull_width_marker = false) : resolution(presolution), margins(pmargins), height_threshold(pheight_threshold), grey_threshold(pgrey_threshold), bounding_box(pbounding_box), full_width_marker(pfull_width_marker)	{}
	document_units::resolution_setting resolution;
	document_units::margins<document_units::centimeter> margins;
	document_units::centimeter height_threshold;
	unsigned int grey_threshold;
	bool bounding_box;
	bool full_width_marker;
};

//rect restrict_size(rect rt, QSize sz);
std::vector<rect> automark_scan(const image_array& bimage, unsigned int threshold, const unsigned int lineThreshold, bool boundingBox);
std::vector<document_units::rect<document_units::centimeter>> autoMarkCombinedInternal(const QImage& qimage, automark_settings settings, double rendered_scale, document_units::size<document_units::centimeter> page);

#endif // AUTOMARKER_H
