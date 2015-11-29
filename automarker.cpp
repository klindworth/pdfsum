#include "automarker.h"

#include <algorithm>
#include <boost/multi_array.hpp>

#include "documentsettings.h"

#include <QSize>
#include <QRectF>
#include <QImage>

#include "documentpage.h"

typedef image_array::index_range range;

//Returns first and last tainted pixel in a line
template<typename Iterator, typename _Predicate>
std::pair<Iterator, Iterator> bothside_search(Iterator first, Iterator end, _Predicate pred)
{
	typedef std::reverse_iterator<decltype(end)> rev_iter;

	auto res_start = std::find_if(first, end, pred);
	auto res_end = first;

	//if a first pixel was found, find a last one (search backwards)
	if(res_start != end)
	{
		auto it_last = std::find_if(rev_iter(end), rev_iter(res_start), pred);
		if(it_last != rev_iter(res_start))
			res_end = it_last.base();
	}

	return std::make_pair(res_start, res_end);
}

std::vector<rect> automark_scan(const image_array& bimage, unsigned int threshold, const unsigned int lineThreshold, bool boundingBox)
{
	std::vector<rect> result;

	const std::size_t lineLength = bimage.shape()[1];
	const std::size_t pageHeight = bimage.shape()[0];

	auto add_marker = [&](int top, int bottom, int left, int right) {
		assert(bottom > top && right > left);
		rect rt;
		rt.top = top;
		rt.bottom = bottom;
		rt.left = left;
		rt.right = right;
		result.push_back(rt);
	};

	auto bcolumn_view = [&](int col) {
		return bimage[boost::indices[range()][col]];
	};

	auto brow_view = [&](int row) {
		return bimage[boost::indices[row][range()]];
	};

	//returns true, if threshold is violated
	auto crit_pred = [=](unsigned char value){return value < threshold;};

	auto bline_crit = [&](const image_array::const_subarray<1>::type& view) {
		return std::any_of(view.begin(), view.end(), crit_pred);
	};

	if(!boundingBox)
	{
		unsigned int emptyLines = 0;
		int firstTaintedRow = -1; //-1 means no tainted block has started
		unsigned int lastTaintedRow = 0;
		unsigned int firstTaintedColumn = lineLength;
		unsigned int lastTaintedColumn = 0;

		for(std::size_t line = 0; line < pageHeight; ++line)
		{
			auto current_row = brow_view(line);
			auto end_it = current_row.end();

			auto trange = bothside_search(current_row.begin(), end_it, crit_pred);

			if(trange.first != end_it)
				firstTaintedColumn = std::min((unsigned int)std::distance(current_row.begin(), trange.first), firstTaintedColumn);
			if(trange.second != current_row.begin())
				lastTaintedColumn  = std::max((unsigned int)std::distance(current_row.begin(), trange.second), lastTaintedColumn);

			if(trange.first == end_it)
			{
				if(emptyLines > lineThreshold && firstTaintedRow != -1)
				{
					add_marker(firstTaintedRow, lastTaintedRow, firstTaintedColumn, lastTaintedColumn);

					firstTaintedRow = -1;
					firstTaintedColumn = lineLength;
					lastTaintedColumn = 0;
				}
				++emptyLines;
			}
			else
			{
				if(firstTaintedRow == -1)
					firstTaintedRow = line;

				emptyLines = 0;
				lastTaintedRow = line;
			}
		}

		if(firstTaintedRow != -1 && firstTaintedRow < pageHeight - lineThreshold)
			add_marker(firstTaintedRow, pageHeight-1, firstTaintedColumn, lastTaintedColumn);
	}
	else
	{
		unsigned int firstBoxRow = 0;
		unsigned int lastBoxRow = 0;

		unsigned int firstBoxColumn = 0;
		unsigned int lastBoxColumn = 0;

		auto row_start = bimage.begin();
		auto row_end = bimage.end();

		//scan for the first line for a pixel, that violates the threshold
		auto it_start = std::find_if(row_start, row_end, bline_crit);
		firstBoxRow = std::distance(row_start, it_start);

		//scan for the last line
		auto row_rstart = bimage.rbegin();
		auto row_rend = bimage.rend();

		for(auto it = row_rstart; it != row_rend; ++it)
		{
			if(std::any_of(it->begin(), it->end(), crit_pred))
			{
				lastBoxRow = pageHeight - std::distance(row_rstart, it);
				break;
			}
		}

		//scan for the first pixel that violates the threshold. pixel->line instead of line->pixel scanning!
		for(unsigned int column = 0; column < lineLength; ++column)
		{
			auto current_column = bcolumn_view(column);

			if(std::any_of(current_column.begin(), current_column.end(), crit_pred))
			{
				firstBoxColumn = column;
				break;
			}
		}

		//scan for the last column
		for(int column = lineLength-1; column >= 0; --column)
		{
			auto current_column = bcolumn_view(column);

			if(std::any_of(current_column.begin(), current_column.end(), crit_pred))
			{
				lastBoxColumn = column;
				break;
			}
		}

		//create marker
		if(firstBoxRow < lastBoxRow && firstBoxColumn < lastBoxColumn)
			add_marker(firstBoxRow, lastBoxRow, firstBoxColumn, lastBoxColumn);
		else
			qDebug("illegal coord. for bounding box");
	}

	return result;
}

/*rect restrict_size(rect rt, QSize sz)
{
	rt.left   = std::max(0, rt.left);
	rt.top    = std::max(0, rt.top);
	rt.right  = std::min(sz.width(),  rt.right);
	rt.bottom = std::min(sz.height(), rt.bottom);
}*/

rect add_margin(const DocumentSettings& settings, int margin, rect rt, QSize sz)
{
	//add the per marker margin
	rt.left   = std::max((int)settings.leftMargin(Unit::pixel), rt.left - margin);
	rt.top    = std::max((int)settings.topMargin(Unit::pixel),  rt.top  - margin);
	rt.right  = std::min((int)(sz.width()  - settings.rightMargin(Unit::pixel)),  rt.right  + margin);
	rt.bottom = std::min((int)(sz.height() - settings.bottomMargin(Unit::pixel)), rt.bottom + margin);

	assert(rt.right > rt.left);
	assert(rt.bottom > rt.top);

	return rt;
}

rect transform_rect(const DocumentSettings& settings, rect rt)
{
	//transform the coordinates due to the page borders
	rt.top    += settings.topMargin(Unit::pixel);
	rt.bottom += settings.topMargin(Unit::pixel);
	rt.left   += settings.leftMargin(Unit::pixel);
	rt.right  += settings.leftMargin(Unit::pixel);

	assert(rt.right > rt.left);
	assert(rt.bottom > rt.top);

	return rt;
}

document_units::rect<document_units::centimeter> add_page_margins(const DocumentSettings &settings, document_units::rect<document_units::centimeter> rt)
{
	return rt.move(document_units::centimeter(settings.leftMargin(Unit::cm)), document_units::centimeter(settings.topMargin(Unit::cm)));
}

document_units::rect<document_units::pixel> add_page_margins_pixel(const DocumentSettings &settings, document_units::rect<document_units::pixel> rt, double scale)
{
	return rt.move(document_units::pixel(settings.leftMargin(Unit::pixel)*scale), document_units::pixel(settings.topMargin(Unit::pixel)*scale));
}

QRectF scale_rect(const DocumentSettings& settings, rect rt, QSize sz, double rendered_scale, bool ignore_width)
{
	if(ignore_width)
		rt.left = settings.leftMargin(Unit::pixel);
	else
		rt.left /= rendered_scale;

	if(ignore_width)
		rt.right = sz.width()/rendered_scale - settings.rightMargin(Unit::pixel);
	else
		rt.right /= rendered_scale;

	int width  = rt.right - rt.left + 1;
	int height = rt.bottom - rt.top + 1;

	return QRectF(rt.left, rt.top/rendered_scale, width, height/rendered_scale);
}

document_units::rect<document_units::pixel> descale_and_transform(rect rt, double scale)
{
	int width  = rt.right - rt.left + 1;
	int height = rt.bottom - rt.top + 1;

	using namespace document_units;
	const document_units::coordinate<document_units::pixel> coord(pixel(rt.left/scale), pixel(rt.top/scale));
	const document_units::size<pixel> sz(pixel(width/scale), pixel(height/scale));

	return document_units::rect<pixel>(coord, sz);
}

std::vector<QRectF> autoMarkCombinedInternal(const QImage& qimage, const DocumentSettings& settings, unsigned int threshold, document_units::centimeter heightThreshold, bool ignore_width, bool boundingBox, double rendered_scale, DocumentPage* page)
{
	//prepare image
	const unsigned int lastPixel = qimage.width()  - settings.rightMargin(Unit::pixel) * rendered_scale;
	const unsigned int lastLine  = qimage.height() - settings.bottomMargin(Unit::pixel) * rendered_scale;
	const unsigned int startPixel = settings.leftMargin(Unit::pixel)*rendered_scale;
	const unsigned int startLine  = settings.topMargin(Unit::pixel) * rendered_scale;

	const std::size_t lineLength = lastPixel - startPixel;
	const std::size_t pageHeight = lastLine  - startLine;

	image_array bimage(boost::extents[pageHeight][lineLength]);
	for(std::size_t line = 0; line < pageHeight; ++line)
	{
		for(std::size_t i = 0; i < lineLength; ++i)
			bimage[line][i] = qGray(qimage.pixel(i+startPixel, line+startLine));
	}

	assert(pageHeight == bimage.shape()[0]);
	assert(lineLength == bimage.shape()[1]);

	//the scanning
	const unsigned int lineThreshold = settings.resolution().y_to<document_units::pixel>(heightThreshold).value*rendered_scale;
	const unsigned int margin = 2*rendered_scale;

	std::vector<QRectF> result;
	std::vector<rect> old_res = automark_scan(bimage, threshold, lineThreshold, boundingBox);
	for(rect rt : old_res)
	{
		QSize sz = qimage.size();
		auto rtst = descale_and_transform(rt, rendered_scale);

		//auto rect_with_borders = add_page_margins(settings, settings.resolution().to<document_units::centimeter>(rtst));
		//auto result_rect = rect_with_borders.bounded_grow(settings.resolution().y_to<document_units::centimeter>(document_units::pixel(margin)), settings.active_area(page->pageSize(settings.resolution()))); //TODO: margin wrong...

		auto rect_with_borders = add_page_margins_pixel(settings, rtst, rendered_scale);
		auto result_rect = rect_with_borders.bounded_grow(document_units::pixel(margin), settings.resolution().to<document_units::pixel>(settings.active_area(page->pageSize(settings.resolution())))); //TODO: margin wrong...


		QRectF compat_rect(QPointF(result_rect.start_point().x.value, result_rect.start_point().y.value), QSizeF(result_rect._size.width.value, result_rect._size.height.value));

		/*QRectF rtf = scale_rect(settings, rt, sz, rendered_scale, ignore_width);

		result.push_back(rtf);*/
		result.push_back(compat_rect);
	}

	return result;
}

/*std::vector<QRectF> autoMarkCombinedInternal(const QImage& qimage, const DocumentSettings& settings, unsigned int threshold, document_units::centimeter heightThreshold, bool ignore_width, bool boundingBox, double rendered_scale)
{
	//prepare image
	const unsigned int lastPixel = qimage.width()  - settings.rightMargin(Unit::pixel);
	const unsigned int lastLine  = qimage.height() - settings.bottomMargin(Unit::pixel);
	const unsigned int startPixel = settings.leftMargin(Unit::pixel);
	const unsigned int startLine  = settings.topMargin(Unit::pixel);

	const std::size_t lineLength = lastPixel - startPixel;
	const std::size_t pageHeight = lastLine  - startLine;

	image_array bimage(boost::extents[pageHeight][lineLength]);
	for(std::size_t line = 0; line < pageHeight; ++line)
	{
		for(std::size_t i = 0; i < lineLength; ++i)
			bimage[line][i] = qGray(qimage.pixel(i+startPixel, line+startLine));
	}

	assert(pageHeight == bimage.shape()[0]);
	assert(lineLength == bimage.shape()[1]);

	//the scanning
	const unsigned int lineThreshold = settings.resolution().y_to<document_units::pixel>(heightThreshold).value*rendered_scale;
	const unsigned int margin = 2*rendered_scale;

	std::vector<QRectF> result;
	std::vector<rect> old_res = automark_scan(bimage, threshold, lineThreshold, boundingBox);
	for(rect rt : old_res)
	{
		QSize sz = qimage.size();
		rt = transform_rect(settings, rt);
		rt = add_margin(settings, margin, rt, sz);

		QRectF rtf = scale_rect(settings, rt, sz, rendered_scale, ignore_width);

		result.push_back(rtf);
	}

	return result;
}*/

