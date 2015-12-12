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
class DocumentSettings;
class QImage;

//rect restrict_size(rect rt, QSize sz);
std::vector<rect> automark_scan(const image_array& bimage, unsigned int threshold, const unsigned int lineThreshold, bool boundingBox);
rect transform_rect(const DocumentSettings& settings, rect rt);
std::vector<document_units::rect<document_units::centimeter>> autoMarkCombinedInternal(const QImage& qimage, const DocumentSettings& settings, unsigned int threshold, document_units::centimeter dHeightThreshold, bool ignore_width, bool boundingBox, double rendered_scale, document_units::size<document_units::centimeter> page);

#endif // AUTOMARKER_H
