#include "automarker.h"

#include <algorithm>
#include <boost/multi_array.hpp>

#include <QImage>

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

		if(firstTaintedRow != -1 && (unsigned int)firstTaintedRow < pageHeight - lineThreshold)
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

document_units::rect<document_units::centimeter> add_page_margins(const document_units::margins<document_units::centimeter>& margins , document_units::rect<document_units::centimeter> rt)
{
	return rt.move(margins.left, margins.top);
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

std::vector<document_units::rect<document_units::centimeter>> postprocess_results(const std::vector<rect>& old_res, automark_settings& settings, double rendered_scale, document_units::size<document_units::centimeter> pageSize)
{
	const document_units::centimeter margin(0.05);
	std::vector<document_units::rect<document_units::centimeter>> result;
	for(rect rt : old_res)
	{
		auto rtst = descale_and_transform(rt, rendered_scale);

		auto rect_with_borders = add_page_margins(settings.margins, settings.resolution.to<document_units::centimeter>(rtst));
		auto result_rect = rect_with_borders.bounded_grow(margin, settings.margins.active_area(pageSize));

		if(settings.full_width_marker)
		{
			auto pagearea = settings.margins.active_area(pageSize);
			result_rect._coordinate.x = pagearea._coordinate.x;
			result_rect._size.width = pagearea._size.width;
		}

		result.push_back(result_rect);
	}

	return result;
}

std::vector<document_units::rect<document_units::centimeter>> autoMarkCombinedInternal(const QImage& qimage, automark_settings settings, double rendered_scale, document_units::size<document_units::centimeter> pageSize)
{
	document_units::margins<document_units::pixel> margins = settings.resolution.to<document_units::pixel>(settings.margins);
	//prepare image
	const unsigned int lastPixel = qimage.width()  - margins.right.value * rendered_scale;
	const unsigned int lastLine  = qimage.height() - margins.bottom.value * rendered_scale;
	const unsigned int startPixel = margins.left.value*rendered_scale;
	const unsigned int startLine  = margins.top.value * rendered_scale;

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
	const unsigned int lineThreshold = settings.resolution.y_to<document_units::pixel>(settings.height_threshold).value*rendered_scale;

	std::vector<rect> old_res = automark_scan(bimage, settings.grey_threshold, lineThreshold, settings.bounding_box);

	return postprocess_results(old_res, settings, rendered_scale, pageSize);
}

