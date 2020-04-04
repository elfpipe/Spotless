#ifndef DB101_SURFACES_HPP
#define DB101_SURFACES_HPP

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/bsdsocket.h>

#include <list>
#include <vector>
#include <string>
#include <algorithm>

#include "../widget.h"

using namespace std;

#define MAX(x, y) ((x)>(y)?(x):(y))
#define MIN(x, y) ((y)>(x)?(x):(y))

class Area {

private:
	
	float _x, _y;
	float _width, _height;

	float _maxWidth, _maxHeight;
	
public:

	Area () : _x(0.0f), _y(0.0f), _width(0.0f), _height(0.0f)
		{ }
	Area (float x, float y, float w, float h) : _x(x), _y(y), _width(w), _height(h)
		{ }
	~Area () {}
	
	void setX (float x) { _x = x; }
	float x() { return _x; }
	void setY (float y) { _y = y; }
	float y() { return _y; }
	void setWidth (float w) { _width = w; }
	float width() { return _width; }
	void setHeight (float h) { _height = h; }
	float height() { return _height; }
	
	void setDimensions (float x, float y, float w, float h) {
		_x = x;
		_y = y;
		_width = w;
		_height = h;
	}
	
	void setMaxDimensions (float w, float h) {
		_maxWidth = w;
		_maxHeight = h;
	}
	
	float maxWidth() { return _maxWidth; }
	float maxHeight() { return _maxHeight; }
	
	bool pointInsideArea (int x, int y) {
		if (x >= _x && x <= _x + _width && y >= _y && y <= _y + _height)
			return true;
		return false;
	}
};

class AreaTargeted : public Area
{
private:

	float _targetX, _targetY;
	float _targetWidth, _targetHeight;

public:

	AreaTargeted ()
		: Area (), _targetX(0.0f), _targetY(0.0f), _targetWidth(0.0f), _targetHeight(0.0f)
		{ }
	AreaTargeted (float x, float y, float w, float h)
		: Area (x, y, w, h), _targetX(x), _targetY(y), _targetWidth(w), _targetHeight(h)
		{ }
		
	void setTargetX (float x) { _targetX = x; }
	float targetX() { return _targetX; }
	void setTargetY (float y) { _targetY = y; }
	float targetY() { return _targetY; }
	void setTargetWidth (float w) { _targetWidth = w; }
	float targetWidth() { return _targetWidth; }
	void setTargetHeight (float h) { _targetHeight = h; }
	float targetHeight() { return _targetHeight; }	
};

void drawBoundedRectangleAlpha (struct BitMap *bm2, int x, int y, int w, int h, unsigned int color, float alpha) {
	struct BitMap *bm = IGraphics->AllocBitMapTags (1, 1, 32, BMATags_PixelFormat, PIXF_A8R8G8B8, TAG_DONE);
	if (bm) {
		struct RastPort rp;
		IGraphics->InitRastPort (&rp);
		rp.BitMap = bm;
	
		IGraphics->RectFillColor (&rp, 0, 0, 1, 1, color);

		float width = (float)w - (float)(x<0?x:0);
		float height = (float)h - (float)(y<0?y:0);
	
		IGraphics->CompositeTags(COMPOSITE_Src_Over_Dest, bm, bm2,
							COMPTAG_SrcX, 0,
							COMPTAG_SrcY, 0,
							COMPTAG_SrcWidth, 1,
							COMPTAG_SrcHeight, 1,
							COMPTAG_OffsetX, MAX(0, x),
							COMPTAG_OffsetY, MAX(0, y),
							//COMPTAG_DestX, MAX(0, x),
							//COMPTAG_DestY, MAX(0, y),
							//COMPTAG_DestWidth, (int)width(),
							//COMPTAG_DestHeight, (int)height(),
							COMPTAG_SrcAlpha, COMP_FLOAT_TO_FIX(alpha),
							COMPTAG_ScaleX, COMP_FLOAT_TO_FIX(width),
							COMPTAG_ScaleY, COMP_FLOAT_TO_FIX(height),
							TAG_DONE);
							
		IGraphics->FreeBitMap (bm);
	}
}

class Surface : public AreaTargeted
{
private:

	const float _animRatio = 0.1f;
	const float _veryLargeNumber = 10000.0f;
	
	std::string _name;
	ReactionWidget *widget;
	struct BitMap *_bitmap;
	int _originalWidth, _originalHeight;
	bool _isEmptySpace;
	bool _inFocus;
	bool _isSingleColor;
	bool _isDrag;
	
	static const unsigned int _color = 0xff44aa66;
	
public:

	Surface (bool emptySpace)
		: AreaTargeted(),
		_name(""),
		_bitmap(0),
		 _originalWidth(0),
		 _originalHeight(0),
		 _isEmptySpace(emptySpace),
		 _inFocus(false),
		 _isSingleColor(true),
		 _isDrag(false)
	{
		setMaxDimensions (_veryLargeNumber, _veryLargeNumber);
	}
	Surface (ReactionWidget *w, struct BitMap *bm, int originalWidth, int originalHeight)
		: AreaTargeted(),
		widget(w),
		_bitmap(bm),
		_originalWidth(originalWidth),
		_originalHeight(originalHeight),
		_isEmptySpace(false),
		_inFocus(false),
		_isSingleColor(false),
		_isDrag(false)
	{
		_name = widget->name();
		setMaxDimensions (_veryLargeNumber, _veryLargeNumber);
	}
	~Surface () {
	}
	
	float originalWidth() { return _originalWidth; }
	float originalHeight() { return _originalHeight; }
	void setFocus (bool f) { _inFocus = f; }
	bool isEmpty () { return _isEmptySpace; }
	void setDrag (bool isDrag) { _isDrag = isDrag; }
	
	void render (struct RastPort *rp) {
	
		if (_isEmptySpace) {
			IGraphics->RectFillColor (rp, x(), y(), x() + width(), y() + height(), 0xff666666);
			//drawBoundedRectangle (rp, x(), y(), width(), height(), 0xff666666);
			
			return;
		}

		if (_isSingleColor) {
			drawBoundedRectangleAlpha (rp->BitMap, x(), y(), width(), height(), _color, _isDrag ? 0.5 : 1.0);
		
			if (_inFocus) {
				unsigned int color = 0xff00ee22;
			
				//box
				IGraphics->RectFillColor (rp, (int)x() - 1, (int)y() - 1, (int)x() + (int)width() + 1, (int)y() + 4, color);
				IGraphics->RectFillColor (rp, (int)x() - 1, (int)y() - 1, (int)x() + 4, (int)y() + (int)height() + 1, color);
				IGraphics->RectFillColor (rp, (int)x() + (int)width() - 4, (int)y() - 1, (int)x() + (int)width() + 1, (int)y() + (int)height() + 1, color);
				IGraphics->RectFillColor (rp, (int)x() - 1, (int)y() + (int)height() - 4, (int)x() + (int)width() + 1, (int)y() + (int)height() + 1, color);
			}
			return;
		}
						
		float scalex = width() / (float)_originalWidth;
		float scaley = height() / (float)_originalHeight;

		float alpha = _isDrag ? 0.7 : 1.0;

		IGraphics->CompositeTags(COMPOSITE_Src_Over_Dest, _bitmap, rp->BitMap,
							COMPTAG_SrcX, 0,
							COMPTAG_SrcY, 0,
							COMPTAG_SrcWidth, _originalWidth,
							COMPTAG_SrcHeight, _originalHeight,
							COMPTAG_OffsetX, (int)x(),
							COMPTAG_OffsetY, (int)y(),
							COMPTAG_DestX, MAX(0, (int)x()),
							COMPTAG_DestY, MAX(0, (int)y()),
							COMPTAG_DestWidth, (int)width(),
							COMPTAG_DestHeight, (int)height(),
							COMPTAG_SrcAlpha, COMP_FLOAT_TO_FIX(alpha),
							COMPTAG_ScaleX, COMP_FLOAT_TO_FIX(scalex),
							COMPTAG_ScaleY, COMP_FLOAT_TO_FIX(scaley),
							TAG_DONE);
		if (_inFocus)
		{
			//box
			IGraphics->RectFillColor (rp, (int)x() - 1, (int)y() - 1, (int)x() + (int)width() + 1, (int)y() + 4, 0xff00ee22);
			IGraphics->RectFillColor (rp, (int)x() - 1, (int)y() - 1, (int)x() + 4, (int)y() + (int)height() + 1, 0xff00ee22);
			IGraphics->RectFillColor (rp, (int)x() + (int)width() - 4, (int)y() - 1, (int)x() + (int)width() + 1, (int)y() + (int)height() + 1, 0xff00ee22);
			IGraphics->RectFillColor (rp, (int)x() - 1, (int)y() + (int)height() - 4, (int)x() + (int)width() + 1, (int)y() + (int)height() + 1, 0xff00ee22);

			//text
			int length = IGraphics->TextLength (rp, _name.c_str(), _name.size());
			IGraphics->SetRPAttrs (rp, RPTAG_APenColor, 0xff00ee22, TAG_DONE);
			IGraphics->Move (rp, (int)x() + (int)width() / 2 - length / 2, (int)y() + (int)height() / 2 - 24 / 2);
			IGraphics->Text (rp, _name.c_str(), _name.size());
		}
	}
	
	bool animate () {
		float wdif = targetWidth() - width();
		float hdif = targetHeight() - height();
			
		setWidth(width() + wdif * _animRatio);
		setHeight(height() + hdif * _animRatio);

		float xdif = targetX() - x();
		float ydif = targetY() - y();

		setX(x() + xdif * _animRatio);
		setY(y() + ydif * _animRatio);

		if((int)wdif || (int)hdif || (int)xdif || (int)ydif)
			return true;
			
		return false;
	}
	
	bool surfaceUnderMouse (int mouseX, int mouseY) {
		return	mouseX >= x() &&
				mouseX <= x() + width() &&
				mouseY >= y() &&
				mouseY <= y() + height();
	}
	
	bool surfaceUnderMouse (int mouseX) {
		return	mouseX >= x() &&
				mouseX <= x() + width();
	}
};

class StageRow
{
	vector <Surface *> _surfaces;
	
	const float _outerSpace = 2.0f;
	const float _spaceBetween = 10.0f;

	float _y, _height;
	
public:

	StageRow () : _y(0.0f), _height(0.0f) { }
	StageRow (float y, float height, bool collapsed) : _y(y), _height(height)
		{ }
	~StageRow () {
		for (vector <Surface *>::iterator it = _surfaces.begin(); it != _surfaces.end(); it++)
			delete (*it);
	}		

	////////////////////
	

	struct Surface *previous (Surface *s) {
		vector <Surface *>::iterator it = std::find (_surfaces.begin(), _surfaces.end(), s);
		if (it != _surfaces.begin() && it != _surfaces.end())
			return *(--it);
		return 0;
	}
	struct Surface *next (Surface *s) {
		vector <Surface *>::iterator it = std::find (_surfaces.begin(), _surfaces.end(), s);
		if (it != _surfaces.end() && ++it != _surfaces.end())
			return *it;
		return 0;
	}

	//////////////////
	
	
	int size () { return _surfaces.size(); }
	float y() { return _y; }
	float height() { return _height; }


	//////////////////
	
	
	void addSurface (Surface *s) {
		_surfaces.push_back (s);
	}
	void insertSurface (Surface *s, int index) {
		vector <Surface *>::iterator its = _surfaces.begin() + index;
		_surfaces.insert (its, s);
	}
	void replaceSurface (Surface *s1, Surface *s2) {
		for (vector<Surface *>::iterator its = _surfaces.begin(); its != _surfaces.end(); its++)
			if ((*its) == s1) {
				vector<Surface *>::iterator it = _surfaces.erase(its);
				_surfaces.insert(it, s2);
				break;
			}
	}
	void eraseSurfacePattern (Surface *s) {
		for (vector<Surface *>::iterator its = _surfaces.begin(); its != _surfaces.end(); its++)
			if ((*its) == s) {
				_surfaces.erase (its);
				break;
			}
	}

	/////////////////////
	
	
	void setInitialDimensions (float y, float height) {
		_y = y;
		_height = height;
		for (vector <Surface *>::iterator it = _surfaces.begin(); it != _surfaces.end(); it++) {
			(*it)->setY (y);
			(*it)->setHeight (height);
		}
	}

	void setSurfaceInitialDimensions (Surface *s) {
		vector <Surface *>::iterator it = std::find (_surfaces.begin(), _surfaces.end(), s);
		if (it != _surfaces.end()) {
			float x = _outerSpace;
				
			Surface *n = previous (*it);
			if (n)
				x = n->x() + n->width() + _spaceBetween;
			
			n = next (*it);
			if(!n)	
				s->setX (x);
			else
				s->setX (x + (n->x() - _spaceBetween - x) / 2.0);
				
			s->setY (_y + _height / 2.0f);
			s->setWidth (0.0f);
			s->setHeight (0.0f);
		}
	}
	
	void setVerticalDimensions (float y, float height) {
		for (vector <Surface *>::iterator it = _surfaces.begin(); it != _surfaces.end(); it++) {
			(*it)->setTargetY (y);
			(*it)->setTargetHeight (height);
		}
		_y = y;
		_height = height;
	}
	
	void calculateHorizontalDimensions (float totalWidth) {
	
		///
	
		float realestate_width = totalWidth - _outerSpace * 2.0f - _spaceBetween * (float)(_surfaces.size() - 1);

		// pass 1: maxed out targets
	
		vector <Surface *> _playfield = _surfaces;

		while (_playfield.size())
		{
			float average_target_width = realestate_width / (float)(_playfield.size());

			vector <Surface *>::iterator it = _playfield.begin();
			for (; it != _playfield.end(); it++)
			{
				if ((*it)->maxWidth() < average_target_width)
				{
					(*it)->setTargetWidth ((*it)->maxWidth());
					realestate_width -= (*it)->targetWidth();
					_playfield.erase(it);
					it = _playfield.begin();
					
					break;
				}
			}
			if (it == _playfield.end())
				break;
		}

		if (_playfield.size() == 0)
			return;
			
		// pass 2: devide the leftover realestate
		
		float average_target_width_remainder = realestate_width / (float)(_playfield.size());
	
		for (vector <Surface *>::iterator it = _playfield.begin(); it != _playfield.end(); it++)
			(*it)->setTargetWidth (average_target_width_remainder);
		
		// pass 3: do the x offsets
		
		float x = _outerSpace;
		
		for (vector <Surface *>::iterator it = _surfaces.begin(); it != _surfaces.end(); it++) {
			(*it)->setTargetX (x);
			x += _spaceBetween;
			x += (*it)->targetWidth();
		}
	}

	///////////////
	

	void render (struct RastPort *rp) {
		for (vector <Surface *>::iterator it = _surfaces.begin(); it != _surfaces.end(); it++)
			(*it)->render (rp);
	}
	
	//returns false if no movement has taken place
	bool animate () {
		bool movement = false;
		for (vector <Surface *>::iterator it = _surfaces.begin(); it != _surfaces.end(); it++) {
			if ((*it)->animate () == true)
				movement = true;
		}
		return movement;
	}
	
	///////////////////////
	
	
	Surface *surfaceUnderMouse (int mouseX) {
		for (vector<Surface *>::iterator it = _surfaces.begin(); it != _surfaces.end(); it++)
			if ((*it)->surfaceUnderMouse (mouseX))
				return *it;
		return 0;
	}
	Surface *surfaceAt (int index) {
		vector<Surface *>::iterator it = _surfaces.begin() + index;
		if (it != _surfaces.end())
			return (*it);
		return 0;
	}
	int mouseAtSurfaceIndex (int mouseX) {
		for (unsigned int i = 0; i < _surfaces.size(); i++)
			if (_surfaces[i]->x() + _surfaces[i]->width() > mouseX)
				return i;
		return _surfaces.size();
	}
	int indexOfSurface (Surface *s) {
		for (unsigned int i = 0; i < _surfaces.size(); i++)
			if (_surfaces[i] == s)
				return i;
		return -1;
	}
	bool mouseOverRow (int mouseY) {
		return mouseY >= _y && mouseY <= _y + _height;
	}
	
	//////////////////////
	
	/*
	void dumpContent () {
		for (vector<Surface *>::iterator its = _surfaces.begin(); its != _surfaces.end(); its++)
			printf("0x%x ", (*its));
		printf("\n");
	}*/
};

class Stage {

private:
	float _totalWidth, _totalHeight;
	
	const float _spaceAbove = 55.0f;
	const float _spaceBetweenRows = 10.0f;
	const float _spaceBelow = 2.0f;
	
	vector <StageRow *> _rows;
	
public:

	Stage (float totalWidth, float totalHeight) : _totalWidth(totalWidth), _totalHeight(totalHeight)
		{ }
	~Stage () {
		for (vector <StageRow *>::iterator it = _rows.begin(); it != _rows.end(); it++)
			delete (*it);
	}
	
	/////////////////////
	
	
	float totalWidth () { return _totalWidth; }
	float totalHeight () { return _totalHeight; }

	int rows () { return _rows.size(); }


	////////////////////

	void expand (unsigned int rows) {
		if (rows >= _rows.size())
			for (unsigned int r = _rows.size(); r <= rows; r++)
				createRow ();
	}
	void contract () {
		for (vector<StageRow *>::iterator itr = _rows.begin(); itr != _rows.end(); itr++)
			if ((*itr)->size() == 0) {
				delete *itr;
				_rows.erase (itr);
				break;
			}
	}
	
	void addSurface (Surface *s, int row) {
		expand (row);		
		vector <StageRow *>::iterator itr = _rows.begin() + row;
		(*itr)->addSurface (s);
		(*itr)->setSurfaceInitialDimensions (s);
	}
	void insertSurface (Surface *s, int row, int index) {
		expand (row);
		vector <StageRow *>::iterator itr = _rows.begin() + row;
		(*itr)->insertSurface (s, index);
		(*itr)->setSurfaceInitialDimensions (s);
	}
	void replaceSurface (Surface *s1, Surface *s2) {
		if (s1 && s2)
		for (vector<StageRow *>::iterator itr = _rows.begin(); itr != _rows.end(); itr++) {
			(*itr)->replaceSurface (s1, s2);
		}
	}
	Surface *take(int row, int index)  {
		vector <StageRow *>::iterator itr = _rows.begin() + row;
		Surface *s = (*itr)->surfaceAt(index);
		(*itr)->eraseSurfacePattern(s);
		return s;
	}
			
	Surface *createEmptySurface (int row) {
		expand (row);
		vector <StageRow *>::iterator itr = _rows.begin() + row;
		Surface *s = new Surface(true);
		(*itr)->addSurface (s);
		(*itr)->setSurfaceInitialDimensions (s);
		return s;
	}				
	Surface *createEmptySurface (int row, int index) {
		expand (row);
		vector <StageRow *>::iterator itr = _rows.begin() + row;
		Surface *s = new Surface(true);
		(*itr)->insertSurface (s, index);
		(*itr)->setSurfaceInitialDimensions (s);
		return s;
	}
	Surface *createColoredSurface (int row) {
		expand (row);
		Surface *s = new Surface(false);
		vector <StageRow *>::iterator itr = _rows.begin() + row;
		(*itr)->addSurface (s);
		(*itr)->setSurfaceInitialDimensions (s);
		return s;
	}				
	Surface *createColoredSurface (int row, int index) {
		expand (row);
		Surface *s = new Surface(false);
		vector <StageRow *>::iterator itr = _rows.begin() + row;
		(*itr)->insertSurface (s, index);
		(*itr)->setSurfaceInitialDimensions (s);
		return s;
	}
	Surface *createSurface (ReactionWidget *w, struct BitMap *bm, int originalWidth, int originalHeight, int row, int index = 0) {
		expand (row);
		Surface *s = new Surface (w, bm, originalWidth, originalHeight);
		vector <StageRow *>::iterator itr = _rows.begin() + row;
		(*itr)->insertSurface (s, index);
		(*itr)->setSurfaceInitialDimensions (s);
		return s;
	}
	
	void createRow () {
		StageRow *r = new StageRow;
		_rows.push_back (r);
		
		setRowInitialDimensions (r);
	}
	void createRow (unsigned int rowIndex) {
		if (rowIndex < _rows.size()) {
			StageRow *r = new StageRow;
			vector <StageRow *>::iterator itr = _rows.begin() + rowIndex;
			_rows.insert(itr, r);
		
			setRowInitialDimensions (r);
		} else {
			expand (rowIndex);
		}
	}

	void destroySurface (Surface *s) {
		for (vector<StageRow *>::iterator itr = _rows.begin(); itr != _rows.end(); itr++)
			(*itr)->eraseSurfacePattern (s);
		contract ();
		delete s;
	}

	//////////////////
	
	

	void setRowInitialDimensions (StageRow *r) {
		vector <StageRow *>::iterator itr = std::find (_rows.begin(), _rows.end(), r);
		float y, height = 0.0f;
		if (itr == _rows.begin())
			y = _spaceAbove;
		else {
			vector <StageRow *>::iterator itp = itr - 1;
			y = (*itp)->y() + (*itp)->height() + _spaceBetweenRows;
		}
		
		(*itr)->setInitialDimensions (y, height);
	}
	
	void calculateTargetDimensions () {
		calculateTargetDimensions (_totalWidth, _totalHeight);
	}

	void calculateTargetDimensions (float totalWidth, float totalHeight) {
		float realestateHeight = totalHeight - _spaceAbove - (float)(_rows.size() - 1)  * _spaceBetweenRows - _spaceBelow;
   		float averageHeight = realestateHeight / (float)_rows.size();
		float y = _spaceAbove;
		for (vector <StageRow *>::iterator it = _rows.begin(); it != _rows.end(); it++) {
			(*it)->setVerticalDimensions (y, averageHeight);
			y += averageHeight + _spaceBetweenRows;
			(*it)->calculateHorizontalDimensions (totalWidth);
		}
	}

	void setSurfaceInitialDimensions (Surface *s) {
		for (vector<StageRow *>::iterator itr = _rows.begin(); itr != _rows.end(); itr++)
			if ((*itr)->indexOfSurface (s) >= 0)
				(*itr)->setSurfaceInitialDimensions (s);
	}
	
	//////////////////
	
	
	void render (struct RastPort *rp) {
		for (vector <StageRow *>::iterator it = _rows.begin(); it != _rows.end(); it++)
			(*it)->render (rp);
	}
	
	bool animate () {
		bool movement = false;
		for (vector <StageRow *>::iterator it = _rows.begin(); it != _rows.end(); it++) {
			if ((*it)-> animate () == true)
				movement = true;
		}
		return movement;
	}
	
	/////////////////////////
	
	
	Surface *surfaceUnderMouse (int mouseX, int mouseY) {
		for (vector <StageRow *>::iterator itr = _rows.begin(); itr != _rows.end(); itr++)
			if ((*itr)->mouseOverRow (mouseY))
				return (*itr)->surfaceUnderMouse (mouseX);
		return 0;
	}
	
	bool mouseNextToEmptyArea (int mouseX, int mouseY) {
		Surface *s = surfaceUnderMouse (mouseX, mouseY);
		// over-empty qualifies as next-to-empty
		if (s) {
			if (s->isEmpty())
				return true;
			else
				return false;
		}
		unsigned int row = mouseAtRowIndex (mouseY);
		if (mouseBetweenRows (mouseY)) {
			if (row < _rows.size()) {
				int index = _rows[row]->mouseAtSurfaceIndex (mouseX);
				if (index == _rows[row]->size())
					index--;
				Surface *s = _rows[row]->surfaceAt (index);
				if (s && s->isEmpty())
					return true;
			}
			if (row) {
				int index = _rows[row-1]->mouseAtSurfaceIndex (mouseX);
				if (index == _rows[row-1]->size())
					index--;
				Surface *s = _rows[row-1]->surfaceAt (index);
				return s ? s->isEmpty() : false;
			}
			return false;
		} else {
			int index = _rows[row]->mouseAtSurfaceIndex (mouseX);
			s = _rows[row]->surfaceAt (index);
			if (s && s->isEmpty())
				return true;
			if (index && (s = _rows[row]->surfaceAt (index-1)))
				return s->isEmpty();
		}
		return false;
	}
	
	bool mouseBetweenRows (int mouseY) {
		if (_rows.size()) {
			if (_rows[0]->y() > mouseY)
				return true;
			for (unsigned int i = 1; i < _rows.size(); i++)
				if (_rows[i-1]->y() + _rows[i-1]->height() < mouseY && _rows[i]->y() > mouseY)
					return true;
			if (_rows[_rows.size()-1]->y() + _rows[_rows.size()-1]->height() < mouseY || mouseY == _totalHeight-1)
				return true;
			return false;
		}
		return true;
	}
	
	int mouseAtRowIndex (int mouseY) {
		for (unsigned int i = 0; i < _rows.size(); i++)
			if (_rows[i]->y() + _rows[i]->height() > mouseY)
				return i;
		return _rows.size();
	}
	
	int mouseAtSurfaceIndex (int row, int mouseX) {
		return _rows[row]->mouseAtSurfaceIndex (mouseX);
	}
	
	///////////////////
	/*
	void dumpContent () {
		printf("--(dump)--\n");
		for (vector<StageRow *>::iterator itr = _rows.begin(); itr != _rows.end(); itr++)
			(*itr)->dumpContent ();
		printf("--(end of dump)--\n");
	}*/
};

class ThumbnailArea : public Area
{
private:

	vector <Surface *> _thumbnails;

	bool _inFocus;
	
public:

	ThumbnailArea (int x, int y, int w, int h, bool inFocus) : Area (x, y, w, h), _inFocus(inFocus)
		{ }
	~ThumbnailArea () {
		for (vector <Surface *>::iterator it = _thumbnails.begin(); it != _thumbnails.end(); it++)
			delete (*it);
	}
	
	void setFocus (bool focus) { _inFocus = focus; }
	
	void addThumbnail (Surface *s) {
		_thumbnails.push_back (s);
	}

	bool mouseOverThumbnailArea (int mouseX, int mouseY) {
		return pointInsideArea (mouseX, mouseY);
	}

	int mouseOverThumbnailByIndex (int mouseX, int mouseY) {
		for (unsigned int i = 0; i < _thumbnails.size(); i++)
			if (_thumbnails[i]->x() <= mouseX &&
				_thumbnails[i]->y() <= mouseY &&
				_thumbnails[i]->x() + _thumbnails[i]->width() >= mouseX &&
				_thumbnails[i]->y() + _thumbnails[i]->height() >= mouseY)
				return i;
		return _thumbnails.size()-1;		
	}
	
	struct Surface *detachThumbnail (int index) {
		vector<Surface *>::iterator its = _thumbnails.begin() + index;
		Surface *ret = (*its);
		_thumbnails.erase(its);
		return ret;
	}
	
	void calculateTargetDimensions ()
	{
		//paint the thumbnails at the top of the screen
	
		float x1 = width() / 2.0f - 200.0f * _thumbnails.size() / 2.0f;
		float y1 = 5.0f;
		float twidth = 150.0f;
		float theight = height() - 10.0f;
		float offset = 200.0f;
	
		for (vector<Surface *>::iterator it = _thumbnails.begin(); it != _thumbnails.end(); it++)
		{
			(*it)->setDimensions (x1, y1, twidth, theight);

			x1 += offset;
		}
	}
	
	void render (struct RastPort *rp)
	{
		if (_inFocus)
		{
			//box
			IGraphics->RectFillColor (rp, 0, 0, width(), 4, 0xff00ee22);
			IGraphics->RectFillColor (rp, 0, 0, 4, height(), 0xff00ee22);
			IGraphics->RectFillColor (rp, width() - 4, 0, width(), height(), 0xff00ee22);
			IGraphics->RectFillColor (rp, 0, height() - 4, width(), height(), 0xff00ee22);
		}
		calculateTargetDimensions ();
		for (vector<Surface *>::iterator it = _thumbnails.begin(); it != _thumbnails.end(); it++)
			(*it)->render(rp);	
	}
};

class StageInterface
{
private:

	const float _thumbnailSpace = 55.0f;

	ThumbnailArea _thumbnailArea;
	Stage _stage;

	struct Screen *_screen;
	struct BitMap *_renderBitmap;
	struct RastPort _rastPort;
	struct Window *_window;
	
	bool _mouseDown;
	Surface *_dragSurface;
	int _dragOriginRow, _dragOriginIndex;
	
	Surface *_focus;
	Surface *_empty;
	
public:

	StageInterface (int w, int h)
		: _thumbnailArea (0.0f, 0.0f, (float)w, (float)_thumbnailSpace, false),
		_stage (w, h),
		//_screen(screen),
		_mouseDown(false),
		_dragSurface(0),
		_focus(0),
		_empty(0)
		{ }
	~StageInterface ()
		{ }
	
	void createSurfacesFromElements (struct Window *windowPointer, list<ReactionWidget *> widgets) {
		_stage.createColoredSurface(0);
		_stage.createColoredSurface(0);
		_stage.createColoredSurface(1);
		_stage.createColoredSurface(1);
		_stage.createColoredSurface(2);

#if 0
		for (list <ReactionWidget *>::iterator it = widgets.begin (); it != widgets.end (); it++) {
			if (!(*it)->state() != ReactionWidget::STATE_EMBEDDED)
				continue;
									
	//		Object *guiObject = (*it)->systemObject();
			
			int left, top, width, height;
			
			(*it)->getDimensions(&left, &top, &width, &height);
			// IIntuition->GetAttrs (guiObject,
			// 	GA_Left,	&left,
			// 	GA_Top,		&top,
			// 	GA_Width,	&width,
			// 	GA_Height,	&height,
			// 	TAG_DONE);
		
			struct BitMap *bitmap = IGraphics->AllocBitMapTags (width, height, 32,
				BMATags_PixelFormat, PIXF_A8R8G8B8,
				BMATags_Friend, windowPointer->RPort->BitMap,
				TAG_DONE);

			if (bitmap) {
				uint8 *buffer = (uint8 *)IExec->AllocVecTags(width * height * 4, TAG_DONE);

				IGraphics->ReadPixelArray(windowPointer->RPort,
				left, top,
				buffer, 0, 0,
				4 * width, PIXF_A8R8G8B8,
				width, height);

				struct RastPort rastport;
				IGraphics->InitRastPort (&rastport);
				rastport.BitMap = bitmap;

				IGraphics->WritePixelArray(buffer,
					0, 0,
					4 * width, PIXF_A8R8G8B8,
					&rastport, 0, 0,
					width, height);

				IExec->FreeVec (buffer);
		
				_stage.createColoredSurface((*it)->row()); //((*it), bitmap, width, height, (*it)->row());
			}
		}
#endif
		_stage.calculateTargetDimensions();
	}
/*
	void populate (vector <Element *> elements) {
		for (vector <Element *>::iterator ite = elements.begin(); ite != elements.end(); ite++) {
			switch ((*ite)->state()) {
				case Element::STATE_EMBEDDED:
				{
					Surface *s = createSurfaceFromElement ((*ite));
					_stage.addSurface (s, (*ite)->embedInRow());
				}
				break;
				
				case Element::STATE_WINDOWED:
				{
					Surface *s = createSurfaceFromElement ((*ite));
					_stage.addThumbnail (s);
				}
				break;
				
				case Element::STATE_HIDDEN:
				break;
			}
		_stage.calculateTargetDimensions();
	}
	*/
	bool show (struct Screen *screen) {
		int _width = screen->Width;
		int _height = screen->Height;
        _thumbnailArea.setWidth (_width);
        
		_window = IIntuition->OpenWindowTags(NULL,
			WA_PubScreen,	screen,
			WA_Width,		_width,
			WA_Height, 		_height,
			WA_ReportMouse,	TRUE,
			WA_Activate,	TRUE,
			WA_IDCMP,		IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE|IDCMP_VANILLAKEY,
			TAG_DONE
		);

		if (_window) {
			_renderBitmap = IGraphics->AllocBitMapTags (
				_width, _height, 32,
				BMATags_PixelFormat, PIXF_A8R8G8B8,
				TAG_DONE
			);
			
			if (_renderBitmap)
			{
				IGraphics->InitRastPort (&_rastPort);
				_rastPort.BitMap = _renderBitmap;
				
				_stage.render (&_rastPort);
				return true;
			}
			IIntuition->CloseWindow (_window);
		}
		return false;
	}
	
	void close () {
		if (_renderBitmap) IGraphics->FreeBitMap (_renderBitmap);
		if (_window) IIntuition->CloseWindow (_window);
	}
	
	void doRender () {
	    IGraphics->RectFillColor (&_rastPort, 0, 0, _stage.totalWidth(), _stage.totalHeight(), 0xff000000);     
	    
		_stage.render (&_rastPort);
		
		if (_dragSurface)
			_dragSurface->render (&_rastPort);
		
		_thumbnailArea.render (&_rastPort);
		
		IGraphics->BltBitMapRastPort (_rastPort.BitMap, 0, 0, _window->RPort, 0, 0, _stage.totalWidth(), _stage.totalHeight(), 0xc0);
		IGraphics->WaitTOF();
	}
	
	bool loop () {
		struct TimeVal t = { 0L, 100L };
		bool timerwait = false;
		bool done = false;
		
		while (!done) {
			timerwait = _stage.animate ();
			doRender();
			
			t.Microseconds = 100;
			ULONG sig = 1 << _window->UserPort->mp_SigBit;
			
			ISocket->WaitSelect (0, NULL, NULL, NULL, (timerwait ? &t : NULL), &sig);

			struct IntuiMessage *msg = (struct IntuiMessage *)IExec->GetMsg (_window->UserPort);

			while (msg) {
				int mouseX = msg->MouseX;
				int mouseY = msg->MouseY;
				ULONG type = msg->Class;

				IExec->ReplyMsg ((struct Message *)msg);

				switch (type)
				{
					case IDCMP_MOUSEMOVE:
					
						doMouseMove (mouseX, mouseY);
						
						break;
					
					case IDCMP_MOUSEBUTTONS:
					
						if (msg->Code == SELECTDOWN)
							doMouseClick (mouseX, mouseY);
						else
							doMouseLetgo (mouseX, mouseY);
							
						break;
						
					case IDCMP_VANILLAKEY:

						done = true;
						
						break;

					default:
					
						break;
				}

				msg = (struct IntuiMessage *)IExec->GetMsg (_window->UserPort);
			}
		}
		return true;
	}
	
	void doMouseMove (int mouseX, int mouseY) {
		if (_thumbnailArea.mouseOverThumbnailArea (mouseX, mouseY))
			_thumbnailArea.setFocus (true);
		else
			_thumbnailArea.setFocus (false);
			
		if (!_mouseDown) {
			Surface *s = _stage.surfaceUnderMouse (mouseX, mouseY);
			if (_focus)
				_focus->setFocus (false);
			if (s)
				s->setFocus (true);
			_focus = s;
		} else {
			if (_focus) {
				_focus->setFocus (false);
				_focus = 0;
			}
			
			if (_dragSurface)
				placeDragSurface (_dragSurface, mouseX, mouseY);
			
			if (_thumbnailArea.mouseOverThumbnailArea (mouseX, mouseY)) {
				_stage.destroySurface (_empty);
				_empty = 0;
			}
				
			bool nextToEmpty = _stage.mouseNextToEmptyArea (mouseX, mouseY);
			
			if (!_stage.surfaceUnderMouse (mouseX, mouseY) &&
				!_thumbnailArea.mouseOverThumbnailArea (mouseX, mouseY) &&
				!nextToEmpty) {
			
				if (_empty)
					_stage.destroySurface (_empty);
				_empty = 0;
				
				//_stage.dumpContent ();
				
				int row = _stage.mouseAtRowIndex (mouseY);
				
				if (_stage.mouseBetweenRows (mouseY)) {
					_stage.createRow (row);
					_empty = _stage.createEmptySurface (row);
				} else {
					_empty = _stage.createEmptySurface (row, _stage.mouseAtSurfaceIndex (row, mouseX));
				}
			}
			_stage.calculateTargetDimensions ();
		}
	}
	
	void placeDragSurface (Surface *s, int mouseX, int mouseY) {
		s->setX (mouseX - s->width() / 2);
		s->setY (mouseY - s->height() / 2);
	}
	
	void doMouseClick (int mouseX, int mouseY) {
		_mouseDown = true;

		if (_thumbnailArea.mouseOverThumbnailArea (mouseX, mouseY)) {
			int index = _thumbnailArea.mouseOverThumbnailByIndex (mouseX, mouseY);
			_dragSurface = _thumbnailArea.detachThumbnail (index);
			return;
		}

		_dragSurface = _stage.surfaceUnderMouse (mouseX, mouseY);
		
		if (_dragSurface) {
			if (_empty)
				_stage.destroySurface (_empty);
							
			_empty = new Surface (true);
			_stage.replaceSurface (_dragSurface, _empty);
			_stage.setSurfaceInitialDimensions (_empty);
							
			_dragSurface->setDrag (true);
			placeDragSurface (_dragSurface, mouseX, mouseY);
			
			_stage.calculateTargetDimensions ();
		}
	}
	
	void doMouseLetgo (int mouseX, int mouseY) {
		_mouseDown = false;
		if (_dragSurface) {
			_dragSurface->setDrag (false);
			
			if (_thumbnailArea.mouseOverThumbnailArea (mouseX, mouseY)) {
				_thumbnailArea.addThumbnail(_dragSurface);
				if (_empty) _stage.destroySurface (_empty);
			}
			else if (_empty) {
				_stage.replaceSurface (_empty, _dragSurface);
				_stage.setSurfaceInitialDimensions (_dragSurface);
				delete _empty;
			}
				
			_empty = 0;
			_dragSurface = 0;				
			
			_stage.calculateTargetDimensions ();
		}
	}
};


#endif
