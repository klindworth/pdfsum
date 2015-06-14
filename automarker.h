#ifndef AUTOMARKER_H
#define AUTOMARKER_H

#include <vector>

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
class QRectF;
class QSize;
class QImage;

//rect restrict_size(rect rt, QSize sz);
std::vector<rect> automark_scan(const image_array& bimage, unsigned int threshold, const unsigned int lineThreshold, bool boundingBox);
rect add_margin(const DocumentSettings& settings, int margin, rect rt, QSize sz);
rect transform_rect(const DocumentSettings& settings, rect rt);
QRectF scale_rect(const DocumentSettings& settings, rect rt, QSize sz, double rendered_scale, bool ignore_width);
std::vector<QRectF> autoMarkCombinedInternal(const QImage& qimage, const DocumentSettings& settings, unsigned int threshold, double dHeightThreshold, bool ignore_width, bool boundingBox, double rendered_scale);

#endif // AUTOMARKER_H
